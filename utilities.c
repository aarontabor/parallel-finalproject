#include "utilities.h"


double deg2rad(double d) {
	return d * (3.1415926/180);
}

void currentTimestamp(double *seconds) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	*seconds = tv.tv_sec + tv.tv_usec/1000000.0;
}
