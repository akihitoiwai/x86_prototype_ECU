/* TimeRecorder.c


*/


#include "sys/time.h"
#include "TimeRecorder.h"

static TimeRecorderData data[TR_ID_MAX];

void TimeRecorderInit() {
  memset(data, 0, sizeof(data));
}


void TimeRecorderStart(int id) {
  data[id].start = getNow();
}

void TimeRecorderStop(int id) {
  data[id].past += getNow() - data[id].start;
  data[id].count += 1;
}

TimeRecorderData* TimeRecorderGetData(int id) {
  return &data[id];
}



// 現在時刻をマイクロ秒単位で取得
uint64 getNow() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

void printTime(const char* msg, uint32 id, uint64 total) {
  printf("%s: %.3fsec (%.2f%%)\n", msg,
	 TR_PERSEC(TimeRecorderGetData(id)->past),
	 100.0 * TimeRecorderGetData(id)->past / total);
}
