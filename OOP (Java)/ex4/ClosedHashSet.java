import java.util.Arrays;

public class ClosedHashSet extends SimpleHashSet
{
    //Fields
    private String[] mainList;
    private static final String DELETED = "deleted"; //reference for deleted cell

    /**
     * @return The current capacity (number of cells) of the table.
     */
    @Override
    public int capacity()
    {
        return this.ownCap;
    }

    /**
     * Add a specified element to the set if it's not already in it.
     * @param newValue New value to add to the set
     * @return False iff newValue already exists in the set
     */
    @Override
    public boolean add(String newValue)
    {
        if(this.contains(newValue)) //newValue already in array
        {
            return false;
        }

        int index = findIndex(newValue);
        this.mainList[index] = newValue;
        this.size++;

        float ratio = (float)this.size/this.ownCap;
        if(ratio > this.highCap)
        {
            rehash(GROW_FACTOR);
        }

        return true;
    }

    /**
     * Adding item int a guaranteed set without load factor problems
     * @param newValue
     * @return
     */
    private void newAdd(String newValue)
    {
        int index = findIndex(newValue);
        this.mainList[index] = newValue;
        this.size++;
    }

    /**
     * Look for a specified value in the set.
     * @param searchVal Value to search for
     * @return True iff searchVal is found in the set
     */
    @Override
    public boolean contains(String searchVal)
    {
        int index;

        for(int i=0; i < this.ownCap; i++)
        {
            index = clamp(closedCode(searchVal, i));

            //reached empty cell
            if(this.mainList[index]==null)
            {
                break;
            }

            //reached deleted cell
            else if(this.mainList[index]==DELETED)
            {
                continue;
            }

            //found word
            if(this.mainList[index].equals(searchVal))
            {
                return true;
            }
        }

        return false;
    }

    /**
     * Remove the input element from the set.
     * @param toDelete Value to delete
     * @return True iff toDelete is found and deleted
     */
    @Override
    public boolean delete(String toDelete)
    {
        if(!contains(toDelete)) //toDelete not in set
        {
            return false;
        }

        int i = 0;
        int index = clamp(closedCode(toDelete, i));

        while(!this.mainList[index].equals(toDelete))
        {
            i++;
            index = clamp(closedCode(toDelete, i));
        }

        this.mainList[index] = DELETED;
        this.size--;

        float ratio = (float)this.size/this.ownCap;
        if(ratio < this.lowCap)
        {
            rehash(SHRINK_FACTOR);
        }

        return true;
    }

    private int closedCode(String word, int i)
    {
        return word.hashCode() + (i+i*i)/2;
    }


    /**
     * @return The number of elements currently in the set
     */
    @Override
    public int size()
    {
        return this.size;
    }

    /**
     * Creates a new array depending on the resizeFactor (double or half) and
     * rehashes the elements from the old array to the new one.
     * @param resizeFactor decides the size of the new array
     */
    private void rehash(double resizeFactor)
    {
        String[] oldArray = this.mainList; //place holder for original array

        //Create array of new size
        this.ownCap = (int)(this.ownCap*resizeFactor);
        this.size = 0;
        this.mainList = new String[this.ownCap];
        for(int i=0; i<this.ownCap; i++)
        {
            this.mainList[i] = null;
        }

        for(String word: oldArray)
        {
            if(word!=DELETED && word!=null)
            {
                this.newAdd(word);
            }
        }
    }

    private int findIndex(String word)
    {
        int i = 0;
        int index = clamp(closedCode(word, i));

        while(this.mainList[index]!=DELETED && this.mainList[index]!=null)
        {
            i++;
            index = clamp(closedCode(word, i));
        }

        return index;
    }

    /**
     * Constructs a new, empty table with the specified load factors, and the
     * default initial capacity (16)
     * @param upperLoadFactor The upper load factor of the hash table
     * @param lowerLoadFactor The lower load factor of the hash table
     */
    public ClosedHashSet(float upperLoadFactor, float lowerLoadFactor)
    {
        super(upperLoadFactor, lowerLoadFactor);
        this.mainList = new String[this.ownCap];
    }

    /**
     * builds the hash set by adding the elements one by one. Duplicate
     * values should be ignored. The new table has the default values of
     * initial capacity (16), upper load factor (0.75), and lower load factor
     * (0.25).
     * @param data Values to add to the set
     */
    public ClosedHashSet(java.lang.String[] data)
    {
        super();
        this.mainList = new String[this.ownCap];

        for(String word: data)
        {
            this.add(word);
        }
    }

    /**
     * Default constructor with references to EMPTY placeholder
     */
    public ClosedHashSet()
    {
        super();
        this.mainList = new String[this.ownCap];
    }

}
