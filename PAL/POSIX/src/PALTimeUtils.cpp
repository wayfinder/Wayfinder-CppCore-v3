#include "PALTimeUtils.h"

#include <time.h>
#include <sys/time.h>

void pal_time_timeOfDay(palTime_t& timeStruct)
{
   struct timeval tv;
   gettimeofday (&tv, NULL);
   struct tm* tm = localtime (&tv.tv_sec);

   timeStruct.year = tm->tm_year + 1900 - 2000;
   timeStruct.month = tm->tm_mon + 1;
   timeStruct.day = tm->tm_mday;
   timeStruct.hour = tm->tm_hour;
   timeStruct.min = tm->tm_min;
   timeStruct.sec = tm->tm_sec;
}

wf_uint32 pal_time_millis()
{
   time_t now = time(NULL);
   struct tm broken = {0};
   localtime_r(&now, &broken);
   // Seconds
   broken.tm_sec  = 0; 
   // Minutes
   broken.tm_min  = 0; 
   // Hours
   broken.tm_hour = 0; 
   time_t midnight = mktime(&broken);   
   // Standard method
   struct ::timeval tv;
   ::gettimeofday(&tv, NULL);
   tv.tv_sec -= midnight;
   return (tv.tv_usec/1000 + tv.tv_sec * 1000);
}

wf_uint32 pal_current_time_millis()
{
   struct timeval tv;
   gettimeofday(&tv, 0);
   return (tv.tv_usec/1000 + tv.tv_sec * 1000);
}

