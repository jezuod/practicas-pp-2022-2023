
#include <sys/time.h>

#include <stdlib.h>

extern double wtime()
{
   /* Use a generic timer */
  //int ti,si;
  //struct timeval *tv;
  //struct timezone *tz;

  //tv=(struct timeval *) malloc(sizeof(struct timeval));
  //tz=(struct timezone *) malloc(sizeof(struct timezone));
  
  //gettimeofday(tv,tz);
  //si=(tv->tv_sec);
  //ti=(tv->tv_usec);
  //ti=si*1000000+ti;
  //return ti/1000000;
   static int sec = -1;
   struct timeval tv;
   gettimeofday(&tv, NULL);
   if (sec < 0) sec = tv.tv_sec;
   return (tv.tv_sec - sec) + 1.0e-6*tv.tv_usec;

}

    
