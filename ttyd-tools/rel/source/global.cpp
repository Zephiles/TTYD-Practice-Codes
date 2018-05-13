#include <ttyd/global.h>

uint32_t mResetCounter = 0;
float NoPointer = 0;

#ifdef TTYD_US
uint32_t r13 = 0x8041CF20;
char *las_25 = (char *) 0x802C0A84;
char *tik_07 = (char *) 0x802C0308;
char *CurrentRoom = (char *) 0x8041E5C8;
#endif

#ifdef TTYD_JP
uint32_t r13 = 0x80417260;
char *las_25 = (char *) 0x802C2804;
char *tik_07 = (char *) 0x802C2088;
char *CurrentRoom = (char *) 0x804183A8;
#endif

#ifdef TTYD_EU
uint32_t r13 = 0x80429760;
char *las_25 = (char *) 0x802CC604;
char *tik_07 = (char *) 0x802CBE88;
char *CurrentRoom = (char *) 0x8042AEE8;
#endif
