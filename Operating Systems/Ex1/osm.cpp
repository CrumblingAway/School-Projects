#include "osm.h"
#include <iostream>
#include <sys/time.h>
#define UNROLLING_FACTOR 10
#define BILLION 1000000000.0
#define MICRO_SEC 1000000.0

double osm_operation_time(unsigned int iterations){
    if (iterations==0){
        return -1;
    }
    timeval tv1 = {0,0};
    timeval tv2 = {0,0};
    while (iterations % UNROLLING_FACTOR != 0){
        iterations+=1;
    }
    gettimeofday(&tv1, NULL);
    for (unsigned int i=0; i<iterations; i+=UNROLLING_FACTOR){
        2+4;
        2+4;
        2+4;
        2+4;
        2+4;
        2+4;
        2+4;
        2+4;
        2+4;
        2+4;
    }
    gettimeofday(&tv2,NULL);
    double tv1Time = (double)tv1.tv_sec+tv1.tv_usec/MICRO_SEC;
    double tv2Time = (double)tv2.tv_sec+tv2.tv_usec/MICRO_SEC;
    return (tv2Time-tv1Time)*BILLION/iterations;
}

void emptyFunc(){}

double osm_function_time(unsigned int iterations){
    if (iterations==0){
        return -1;
    }
    timeval tv1 = {0,0};
    timeval tv2 = {0,0};
    while (iterations % UNROLLING_FACTOR != 0){
        iterations+=1;
    }
    gettimeofday(&tv1,NULL);
    for (unsigned int i=0; i<iterations; i+=UNROLLING_FACTOR){
        emptyFunc();
        emptyFunc();
        emptyFunc();
        emptyFunc();
        emptyFunc();
        emptyFunc();
        emptyFunc();
        emptyFunc();
        emptyFunc();
        emptyFunc();
    }
    gettimeofday(&tv2,NULL);
    double tv1Time = (double)tv1.tv_sec+tv1.tv_usec/MICRO_SEC;
    double tv2Time = (double)tv2.tv_sec+tv2.tv_usec/MICRO_SEC;
    return (tv2Time-tv1Time)*BILLION/iterations;
}

double osm_syscall_time(unsigned int iterations){
    if (iterations==0){
        return -1;
    }
    timeval tv1 = {0,0};
    timeval tv2 = {0,0};
    while (iterations % UNROLLING_FACTOR != 0){
        iterations+=1;
    }
    gettimeofday(&tv1,NULL);
    for (unsigned int i=0; i<iterations; i+=UNROLLING_FACTOR){
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
        OSM_NULLSYSCALL;
    }
    gettimeofday(&tv2,NULL);
    double tv1Time = (double)tv1.tv_sec+tv1.tv_usec/MICRO_SEC;
    double tv2Time = (double)tv2.tv_sec+tv2.tv_usec/MICRO_SEC;
    return (tv2Time-tv1Time)*BILLION/iterations;
}
