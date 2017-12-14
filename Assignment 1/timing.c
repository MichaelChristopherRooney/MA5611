////////////////////////////////////////////////////////////
// Functions for timing execution
////////////////////////////////////////////////////////////

#include "common.h"

static struct timeval start_time;
static struct timeval end_time;
static long long time_taken;

void start_timer(){
#if TIME_EXECUTION
	gettimeofday(&start_time, NULL);
#endif
}

void end_timer(char *description){
#if TIME_EXECUTION
	gettimeofday(&end_time, NULL);
	time_taken = (end_time.tv_sec - start_time.tv_sec) * 1000000L + (end_time.tv_usec - start_time.tv_usec);
	printf("%s took: %lld microseconds.\n", description, time_taken);
#endif
}
