#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <cmath>

/**
 * Returns value from address in range start to finish, where address is read right to left
 * @param address
 * @param start
 * @param finish
 * @return
 */
uint64_t valueAt(uint64_t address, int start, int finish)
{
    if (finish+1 < start || finish > VIRTUAL_ADDRESS_WIDTH || start > VIRTUAL_ADDRESS_WIDTH)
    {
        exit(1);
    }

    int offset = finish - start + 1;
    return (address >> start) & ((1 << offset) - 1);
}

/**
 * Calculate index in physical memory, takes into account the possiblity that frame 0 can have different size
 * @param frameNum
 * @param offset
 * @return
 */
int calcAddr(word_t frameNum, int offset)
{
	return frameNum * PAGE_SIZE + offset;
}

/**
 * Traverse tree in physical memory to find next available frame (if any).
 * If an unused frame is found return its index, otherwise return -1. This
 * function also keeps track of the max frame it iterated over.
 * @param curFrame current frame being checked
 * @param parentFrame parent of curFrame
 * @param numOfFrames amount of different frames we've checked
 * @param frameSize size of frame (frame 0 might have less lines)
 * @param depth depth of curFrame
 * @param lineNum parentFrame[lineNum] = curFrame
 * @param maxFrameIndex max frame checked
 * @param leavesArr array of frames that hold pages, frame's id == frame's index in array
 * @param pageNum page number, only releveant when updating leavesArr
 * @param bannedFrame empty frame we're not allowed to allocate
 * @return number of available frame if such exists, otherwise -1
 */
int traversingTree(int curFrame, int parentFrame, int numOfFrames, int frameSize, int depth, int lineNum, int *maxFrameIndex,
				   int *leavesArr, int pageNum, word_t bannedFrame)
{
    if (depth >= TABLES_DEPTH) //we got to a leaf
    {
        //Update the leaves array
        leavesArr[curFrame] = pageNum;
        return -1;
    }
    if (numOfFrames >= NUM_FRAMES) //base case, we passed all the frames
    {
        return -1;
    }
    int availableFrame = -1; // flag indicates we passed a sub tree and didn't find available frame.
    bool isEmpty = true; // flag indicates that frame is empty
    for (int i = 0; i < frameSize; i++)
    {
        word_t w;
        PMread(calcAddr(curFrame, i), &w);
        if (w != 0)
        {
            isEmpty = false;
            *maxFrameIndex = std::fmax(*maxFrameIndex, w);
            availableFrame = traversingTree(w, curFrame, ++numOfFrames, PAGE_SIZE, depth + 1, i, maxFrameIndex, leavesArr,
											(pageNum << OFFSET_WIDTH) + i, bannedFrame);
            if (availableFrame != -1) // a parentFrame of a frame is not available.
            {
                return availableFrame;
            }
        }
    }
    if (curFrame != bannedFrame && isEmpty) // valid empty frame
    {
        PMwrite(calcAddr(parentFrame, lineNum), 0);
        return curFrame;
    }
    else
    {
        return -1;
    }
}


/**
 * Third case of finding frame, where all frames are taken and we need to evict a page
 * @param pageSwappedIn
 * @param leafFrames
 * @param victimFrame
 * @param victimPage
 * @return
 */
uint64_t phase3(int pageSwappedIn, const int *leafFrames, uint64_t victimFrame, uint64_t victimPage)
{
	int distance = 0;
	for (int i = 0; i < NUM_FRAMES; i++)
	{
		if (leafFrames[i] != -1)
		{
			int abs = std::abs(pageSwappedIn - leafFrames[i]);
			int candidate = std::fmin((int) NUM_PAGES - abs, abs);
			if (candidate > distance)
			{
				distance = candidate;
				victimFrame = i;
				victimPage = leafFrames[i];
			}
		}
	}
	PMevict(victimFrame, victimPage);
	
	// update parent of victimFrame
	int start = TABLES_DEPTH * OFFSET_WIDTH - OFFSET_WIDTH;
	int finish = VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH - 1;
	int curFrame = 0;
	while (start > 0)
	{
		uint64_t curOffset = valueAt(victimPage, start, finish);
		PMread(calcAddr(curFrame, curOffset), &curFrame);
		finish = start - 1;
		start -= OFFSET_WIDTH;
	}
	PMwrite(calcAddr(curFrame, valueAt(victimPage, start, finish)), 0);
	return victimFrame;
}

/**
 * Find available frame
 * @return: The index of the victim frame
 */
int findVictim(int pageSwappedIn, word_t bannedFrame)
{
	// Init array that will hold frames containing pages
    int leafFrames[NUM_FRAMES];
    for (int i = 0; i < NUM_FRAMES; i++)
    {
        leafFrames[i] = -1;
    }
    
    int maxFrameIndex = 0;
    int frameSize = 1 << (VIRTUAL_ADDRESS_WIDTH - TABLES_DEPTH * OFFSET_WIDTH);
    int v = traversingTree(0, 0, 0, frameSize, 0, 0, &maxFrameIndex, leafFrames, 0, bannedFrame);
    
    //phase 1: found valid empty frame
    if (v != -1)
    {
        return v;
    }
    
    //phase 2: didn't find empty frame using DFS but not all frames are occupied
    if (maxFrameIndex + 1 < NUM_FRAMES)
    {
        return maxFrameIndex + 1;
    }
    
    //phase 3: all frames are taken, calculate farthest page, evict it and allocat its frame
    uint64_t victimFrame = phase3(pageSwappedIn, leafFrames, 0, 0);
	
	return victimFrame;
}


void clearTable(uint64_t frameIndex)
{
    for (uint64_t i = 0; i < PAGE_SIZE; ++i)
    {
        PMwrite(frameIndex * PAGE_SIZE + i, 0);
    }
}

void VMinitialize()
{
    clearTable(0);
}

/**
 * Algorithm for iterating over tree
 * @param virtualAddress
 * @param start stores the index of least significant bit of offset
 * @param finish stores the index of most significant bit of offset
 * @param curFrame
 */
void VMalgorithm(uint64_t virtualAddress, int &start, int &finish, word_t &curFrame)
{
    start = TABLES_DEPTH * OFFSET_WIDTH;
    finish = VIRTUAL_ADDRESS_WIDTH - 1;
    curFrame = 0;
    int pageSwappedIn = valueAt(virtualAddress, OFFSET_WIDTH, VIRTUAL_ADDRESS_WIDTH - 1);
    
    // Iterate over virtualAddress and build tree accordingly
    while (start > 0)
	{
		uint64_t curOffset = valueAt(virtualAddress, start, finish);
		int tempFrame = 0;
		PMread(calcAddr(curFrame, curOffset), &tempFrame);
		if (tempFrame == 0)
		{
			int f = findVictim(pageSwappedIn, curFrame);
			//initialize the victim frame
			if (start > OFFSET_WIDTH)
			{
				clearTable(f);
			}
			PMwrite(calcAddr(curFrame, curOffset), f);
			curFrame = f;
		}
		else
		{
			curFrame = tempFrame;
		}
		finish = start - 1;
		start -= OFFSET_WIDTH;
	}
    PMrestore(curFrame, pageSwappedIn);
}

int VMread(uint64_t virtualAddress, word_t *value)
{
	try
	{
		if (virtualAddress >= VIRTUAL_MEMORY_SIZE)
		{
			return 0;
		}
		int start;
		int finish;
		word_t curFrame;
		VMalgorithm(virtualAddress, start, finish, curFrame);
		PMread(calcAddr(curFrame, valueAt(virtualAddress, start, finish)), value);
		return 1;
	}
	catch(int e)
	{
		return 0;
	}
	
}

int VMwrite(uint64_t virtualAddress, word_t value)
{
	try
	{
		if (virtualAddress >= VIRTUAL_MEMORY_SIZE)
		{
			return 0;
		}
		int start;
		int finish;
		word_t curFrame;
		VMalgorithm(virtualAddress, start, finish, curFrame);
		PMwrite(calcAddr(curFrame, valueAt(virtualAddress, start, finish)), value);
		return 1;
	}
	catch(int e)
	{
		return 0;
	}
}
