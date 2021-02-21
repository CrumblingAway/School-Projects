#include "MapReduceFramework.h"
#include <pthread.h>
#include <iostream>
#include <atomic>
#include "Barrier.h"

#define SYS_ERR "system error : "
#define THREAD_ERR "Can't create thread"
#define JOIN_ERR "Can't join threads"

/* ====================================================================================== */

/* Structs for thread contexts */
typedef struct
{
    std::vector<IntermediatePair> _vec;
    int _oldMapVal;
    pthread_mutex_t _mutex;
} MapContext;

typedef struct
{
    int *_threadIndices;
} ShuffleContext;


typedef struct
{
	std::atomic<int> *_counter; // items in inputVec
	std::atomic<int> *_mapCounter; // finished map threads
	std::atomic<uint64_t> *_stateBuffer; // 64 bit variable for storing progress atomically
	
    int _multiThreadLevel;
    const MapReduceClient *_client;
    
    pthread_t *_threads; // all the threads of the job
    MapContext *_mapContexts; // contexts of threads that start with client map function
    ShuffleContext *_shuffleContext; // context of thread that starts with client shuffle function
    int *_threadIndices; // for use in shuffle, represents last "shuffled" item in vector of every mapping thread
    
    const InputVec *_inputVec;
    OutputVec *_outputVec;
    pthread_mutex_t _oVecMutex;
    pthread_mutex_t _stateMutex;
    JobState _state;
    
    IntermediateMap _iMap;
    std::vector<K2 *> *_keysVec; // vector of keys in _iMap
    int _keysSize; // size of _keysVec
    JobState _tempState; // To allow for simultaneous assignment of percentage and stage
    
    Barrier _barrier = Barrier(0);
} JobContext;

typedef struct
{
    int _oldVal;
    JobContext *_jc;
    // TODO: define reduce context, outputVec can be retrieved through Map and Shuffle contexts
} ReduceContext;



/* ====================================================================================== */

/* Structs for arguments of Map */
typedef struct
{
    JobContext *_jc;
    const InputVec *_inputVec;
    MapContext *_context;
} MapArgs;

/* ====================================================================================== */
void initJobContext(const MapReduceClient &client, OutputVec &outputVec, int multiThreadLevel, JobContext *jc,
                    const InputVec &inputVec, pthread_t *&threads, MapContext *&mapContexts,
                    ShuffleContext *&shuffleContext)
{
    threads= new pthread_t[multiThreadLevel];
    mapContexts= new MapContext[multiThreadLevel - 1];
    shuffleContext= new ShuffleContext;
    jc->_counter = new std::atomic<int>(0);
    jc->_mapCounter = new std::atomic<int>(0);
    jc->_stateBuffer = new std::atomic<uint64_t>(0);
    jc->_state.stage = UNDEFINED_STAGE;
    jc->_multiThreadLevel = multiThreadLevel;
    jc->_barrier = Barrier(multiThreadLevel);
    jc->_client = &client;
    jc->_inputVec = &inputVec;
    jc->_outputVec = &outputVec;
    jc->_oVecMutex = PTHREAD_MUTEX_INITIALIZER;
    jc->_stateMutex = PTHREAD_MUTEX_INITIALIZER;

    /* Start job with threads (n-1 map and 1 shuffle) */
    jc->_threads = threads;
    jc->_mapContexts = mapContexts;

}

/**
 * Initialize map context
 * @param mc
 */
void initMapContext(MapContext *mc)
{
    mc->_oldMapVal = 0;
    mc->_mutex = PTHREAD_MUTEX_INITIALIZER;
}

void initReduceContext(ReduceContext *rc, JobContext *jc)
{
    rc->_oldVal = 0;
    rc->_jc = jc;
}

/* ====================================================================================== */

/**
 * Calculate percentage of progress
 * @param jc
 * @return
 */
float getPercentage(const JobContext *jc)
{
    return ((float) (jc->_stateBuffer->load() & (unsigned long) (0x7fffffff))) /
           (float) (jc->_stateBuffer->load() >> 31u & ((unsigned long) 0x7fffffff)) * 100;
}

/* Wrappers for threads, one for those that start with map and one for shuffle */

void *reduceWrapper(ReduceContext *rc, JobContext *jc)
{
	/* Calculate oldValue, run client reduce on relevant input, adjust percentage */
    rc->_oldVal = (*(jc->_counter))++;
    while (rc->_oldVal < jc->_keysSize)
    {
        K2 *k = (*(jc->_keysVec))[rc->_oldVal];
        jc->_client->reduce(k, jc->_iMap[k], rc);
        (*(jc->_stateBuffer))++;
        pthread_mutex_lock(&jc->_stateMutex);
        jc->_state.percentage = getPercentage(jc);
		pthread_mutex_unlock(&jc->_stateMutex);
        rc->_oldVal = (*(jc->_counter))++;
    }
    return nullptr;
}

void *mapWrapper(void *args)
{
    /* Calculate oldValue, run client map on relevant input, adjust percentage */
    auto *mapArgs = (MapArgs *) args;
    mapArgs->_context->_oldMapVal = (*(mapArgs->_jc->_counter))++;
    while (mapArgs->_context->_oldMapVal < mapArgs->_inputVec->size())
    {
        mapArgs->_jc->_client->map((*mapArgs->_inputVec)[mapArgs->_context->_oldMapVal].first,
                                   (*mapArgs->_inputVec)[mapArgs->_context->_oldMapVal].second,
                                   mapArgs->_context);
        (*(mapArgs->_jc->_stateBuffer))++;
		pthread_mutex_lock(&mapArgs->_jc->_stateMutex);
        mapArgs->_jc->_state.percentage = getPercentage(mapArgs->_jc);
		pthread_mutex_unlock(&mapArgs->_jc->_stateMutex);
        mapArgs->_context->_oldMapVal = (*(mapArgs->_jc->_counter))++;
    }
    (*(mapArgs->_jc->_mapCounter))++; // increment counter to be checked by shuffle thread
    
    /* Reduce stage */
    mapArgs->_jc->_barrier.barrier();
    ReduceContext rc;
    initReduceContext(&rc, mapArgs->_jc);
    reduceWrapper(&rc, mapArgs->_jc);
    delete mapArgs;
    return nullptr;
}

void *shuffleWrapper(void *args)
{
    auto *jc = (JobContext *) args;
    bool isMap = true;
    bool update = false;
    float totalAmount = 0.0;
    int amount = 0;

    while (isMap)
    {
    	/* Switch stages from Map to Shuffle */
        if (*(jc->_mapCounter) == jc->_multiThreadLevel - 1)
        {
            *(jc->_stateBuffer) = (2ul << 62u);
            isMap = false;
            update = true;
            for (int j = 0; j < jc->_multiThreadLevel - 1; ++j)
            {
                totalAmount += (jc->_mapContexts[j]._vec.size());
            }
            (*(jc->_stateBuffer)) += ((unsigned long) (totalAmount) << 31u);
            (*(jc->_stateBuffer)) += amount;
			jc->_tempState.percentage = getPercentage(jc);
			jc->_tempState.stage = (stage_t) (jc->_stateBuffer->load() >> 62u);
			jc->_state = jc->_tempState;
        }

        /* Starting from last saved index iterate over vectors of threads and push results to map */
        for (int i = 0; i < jc->_multiThreadLevel - 1; ++i)
        {
            auto &mc = jc->_mapContexts[i];
            pthread_mutex_lock(&(mc._mutex));
            for (int j = jc->_shuffleContext->_threadIndices[i]; j < (int) mc._vec.size(); ++j)
            {
                auto &pair = mc._vec[j];
                jc->_iMap[pair.first].emplace_back(pair.second);
                amount++;
                if (update)
                {
                    (*(jc->_stateBuffer))++;
                    jc->_state.percentage = getPercentage(jc);
                }
                (jc->_shuffleContext->_threadIndices[i])++;
            }
            pthread_mutex_unlock(&(mc._mutex));
        }
    }

    /* Reduce stage */
    *(jc->_stateBuffer) = (3ul << 62u);
	jc->_tempState.stage = (stage_t) (jc->_stateBuffer->load() >> 62u);
	jc->_tempState.percentage = 0.0;
	jc->_state = jc->_tempState;
    *(jc->_counter) = 0;
    jc->_keysVec = new std::vector<K2 *>;
    for (const auto &elem: jc->_iMap)
    {
        jc->_keysVec->emplace_back(elem.first);
    }
    jc->_keysSize = (int) jc->_keysVec->size();
    *(jc->_stateBuffer) += ((unsigned long) (jc->_keysSize) << 31u);
    jc->_barrier.barrier();
    ReduceContext rc;
    initReduceContext(&rc, jc);
    reduceWrapper(&rc, jc);
    return nullptr;
}

/* ====================================================================================== */

void emit2(K2 *key, V2 *value, void *context)
{
    auto *mc = (MapContext *) context;
    pthread_mutex_lock(&(mc->_mutex));
    mc->_vec.emplace_back(key, value);
    pthread_mutex_unlock(&(mc->_mutex));
}

void emit3(K3 *key, V3 *value, void *context)
{
    auto *rc = (ReduceContext *) context;
    //std::cerr<<"starting emit 3\n";
    pthread_mutex_lock(&(rc->_jc->_oVecMutex));
    rc->_jc->_outputVec->emplace_back(key, value);
    pthread_mutex_unlock(&(rc->_jc->_oVecMutex));
    //std::cerr<<"finished emit 3\n";
}

/**
 * Initialize client map arguments
 * @param ma pointer to map arguments structure
 * @param inputVec pointer to vector of inputs over which to iterate
 * @param mc context of calling thread
 * @param jc context of job of thread
 */
void initMapArgs(MapArgs *ma, const InputVec *inputVec, MapContext *mc, JobContext *jc)
{
    ma->_inputVec = inputVec;
    ma->_context = mc;
    ma->_jc = jc;
}

/**
 * Initialize shuffle thread context
 * @param sc pointer to shuffleContext for initialization
 * @param threadIndices represent shuffle progress within vectors of amp threads
 */
void initShuffleContext(ShuffleContext *sc, int *threadIndices)
{
    sc->_threadIndices = threadIndices;
}

JobHandle startMapReduceJob(const MapReduceClient &client,
                            const InputVec &inputVec, OutputVec &outputVec,
                            int multiThreadLevel)
{
    /* Declare JobContext to initialize its atomic counter */
    auto *jc = new JobContext;
	pthread_t *threads;
	MapContext *mapContexts;
	ShuffleContext *shuffleContext;
    initJobContext(client, outputVec, multiThreadLevel, jc, inputVec, threads, mapContexts, shuffleContext);
	
	/* Create map threads */
    *(jc->_stateBuffer) = (1ul << 62u);
    jc->_state.stage = (stage_t) (jc->_stateBuffer->load() >> 62u);
    jc->_state.percentage = 0.0;
    *(jc->_stateBuffer) += ((unsigned long) (inputVec.size()) << 31u);

    for (int i = 0; i < multiThreadLevel - 1; ++i)
    {
        initMapContext(&mapContexts[i]);
        /* Create arguments for map */
        auto *ma = new MapArgs;
        initMapArgs(ma, &inputVec, &mapContexts[i], jc);
        if (pthread_create(&(threads[i]), nullptr, mapWrapper, ma) != 0)
        {
            std::cerr << SYS_ERR << THREAD_ERR << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    /* Create shuffle thread */
    initShuffleContext(shuffleContext, new int[multiThreadLevel - 1]);
    //TODO: maybe change array of indices to vector
    for (int i = 0; i < multiThreadLevel - 1; ++i)
    {
        shuffleContext->_threadIndices[i] = 0;
    }
    jc->_shuffleContext = shuffleContext;
    if (pthread_create(&(threads[multiThreadLevel - 1]), nullptr, shuffleWrapper, jc) != 0)
    {
        std::cerr << SYS_ERR << THREAD_ERR << std::endl;
        exit(EXIT_FAILURE);
    }

    return jc;
}


void waitForJob(JobHandle job)
{
    auto *jc = (JobContext *) job;
    for (int i = 0; i < jc->_multiThreadLevel; i++)
    {
        if (pthread_join(jc->_threads[i], nullptr) != 0)
        {
            std::cerr << SYS_ERR << JOIN_ERR << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

void getJobState(JobHandle job, JobState *state)
{
    auto *jc = (JobContext *) job;
    *state = jc->_state;
}

void destroyJob(JobHandle job)
{
/*    auto *jc = (JobContext *) job;
    delete jc->_counter;
    delete jc->_stateBuffer;
    delete jc->_mapCounter;
    delete[] jc->_shuffleContext->_threadIndices;
    delete jc->_shuffleContext;
    delete[] jc->_mapContexts;
    delete jc->_keysVec;
    pthread_mutex_destroy(&jc->_oVecMutex);
    pthread_mutex_destroy(&jc->_stateMutex);
    delete[] jc->_threads;
    delete jc;*/
}

void closeJobHandle(JobHandle job)
{
    waitForJob(job);
    destroyJob(job);
}