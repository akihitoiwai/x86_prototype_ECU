/* Dem.c

DEM  Diagnostic Event Manager

  contains the declarations of the API services of the Dem used by the FlexRay Interface

  @note   DemÇÃé¿ëïÇÕébíËÇ≈Ç∑ÅB

  Copyright(c) 2008 CATS corp.
  */

#include <time.h>
#include "Std_Types.h"
#include "Platform_Types.h"
#include "Dem.h"

Dem_EventIdType gLastErrorId = DEM_E_NOERROR;
uint8 gLastDevelopmentErrorId = DEM_E_NOERROR;

void Dem_ReportErrorStatus(Dem_EventIdType id, Dem_EventStatusType status) {
  DebugOut2("Dem_ReportErrorStatus: EventId:%d Status:%d\n", id, status);
  gLastErrorId = id;
}

void Dem_SetDevelopmentError(uint8 id) {
  gLastDevelopmentErrorId = id;
}

Dem_EventIdType Dem_GetLastError() {
  Dem_EventIdType err = gLastErrorId;
  gLastErrorId = DEM_E_NOERROR;
  return err;
}

uint8 Dem_GetLastDevelopmentError() {
  uint8 err = gLastDevelopmentErrorId;
  gLastDevelopmentErrorId = DEM_E_NOERROR;
  return err;
}



/* É ïbÇÃSleep */

void Dem_Sleep(uint32 usec) {
#ifdef WIN32
  Sleep(usec/1000);
#else
  struct timespec req, rem;
  req.tv_sec = usec / 1000000;
  req.tv_nsec = (usec % 1000000) * 1000;
  rem.tv_sec = 0;
  rem.tv_nsec = 0;
  while (nanosleep(&req, &rem) == -1) {
    /* Ç‹ÇæìûíBÇµÇƒÇ¢Ç»Ç¢ */
    req = rem;
    if (req.tv_sec == 0 && req.tv_nsec == 0) break;
  }
#endif
}
