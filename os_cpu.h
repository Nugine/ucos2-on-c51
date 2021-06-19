#ifndef  OS_CPU_H
#define  OS_CPU_H

#include <keil_c51.h>

typedef u8 BOOLEAN;
typedef u8 INT8U;
typedef i8 INT8S;
typedef u16 INT16U;
typedef i16 INT16S;
typedef u32 INT32U;
typedef i32 INT32S;
typedef f32 FP32;
typedef f64 FP64;

typedef u8 OS_STK;
typedef u8 OS_CPU_SR;

#define OS_CRITICAL_METHOD 1

#if OS_CRITICAL_METHOD == 1
#ifdef __KEIL_C51
#define OS_ENTER_CRITICAL() EA = 0
#define OS_EXIT_CRITICAL() EA = 1
#else
#define OS_ENTER_CRITICAL()
#define OS_EXIT_CRITICAL()
#endif
#endif

#define OS_STK_GROWTH 1u

#define OS_TASK_SW() OSCtxSw()

void OSStartHighRdy(void);
void OSIntCtxSw(void);
void OSCtxSw(void);
void InitTimer0(void);
void InitSerial(void);
u8 SerialPutCharUnlocked(u8 ch);
u8 SerialPrintStrUnlocked(u8 *s);
u16 SerialGetChar(void) REENTRANT;

#define SERIAL_BUF_LEN 128


#endif