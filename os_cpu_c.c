#include "ucos_ii.h"

OS_STK *OSTaskStkInit(void (*task)(void *p_arg), void *p_arg, OS_STK *ptos,
                      INT16U opt) REENTRANT {
    OS_STK *stk;

    p_arg = p_arg;
    opt = opt;

    stk = ptos;

    *(stk--) = 0x07; // R7
    *(stk--) = 0x06; // R6
    *(stk--) = 0x05; // R5
    *(stk--) = 0x04; // R4
    *(stk--) = 0x03; // R3
    *(stk--) = 0x02; // R2
    *(stk--) = 0x01; // R1
    *(stk--) = 0x00; // R0
    *(stk--) = 0xF1; // DPH
    *(stk--) = 0xF0; // DPL
    *(stk--) = 0x0B; // B
    *(stk--) = 0x0A; // ACC
    *(stk--) = 0x00; // PSW
    *(stk--) = (INT16U)task >> 8;
    *(stk--) = (INT16U)task & 0xFF;
    *(stk--) = 15U; // real_stack_len

    return stk + 1;
}


void OSTaskSwHook(void) REENTRANT {}

void OSTaskDelHook(OS_TCB *ptcb) REENTRANT { ptcb = ptcb; }

void OSTaskIdleHook(void) REENTRANT {}

void OSTaskReturnHook(OS_TCB *ptcb) REENTRANT { ptcb = ptcb; }

void OSTaskStatHook(void) REENTRANT {}

void OSTCBInitHook(OS_TCB *ptcb) REENTRANT { ptcb = ptcb; }

def_sbit(P1_0, P1 ^ 0);
void OSTimeTickHook(void) REENTRANT {
    static XDATA u8 time_tick_cnt = 0;
    u8 x = ++time_tick_cnt;
    if (x % (OS_TICKS_PER_SEC / 2) == 0) {
        time_tick_cnt = 0;
        flip_sbit(P1_0);
    }
}

void OSTaskCreateHook(OS_TCB *ptcb) REENTRANT { ptcb = ptcb; }

void OSInitHookBegin(void) REENTRANT {}

void OSInitHookEnd(void) REENTRANT {}

void OSDebugInit(void) REENTRANT {}

u8 TX_BUF[SERIAL_BUF_LEN];
u8 RX_BUF[SERIAL_BUF_LEN];
u8 TX_head, TX_tail;
u8 RX_head, RX_tail;

void DoInitSerial(void) REENTRANT {
    TX_head = TX_tail = 0;
    RX_head = RX_tail = 0;
}

void DoSerialISR(void) REENTRANT {
    u8 len;
    u8 b;
    if (get_sbit(RI)) {
        len = RX_tail - RX_head;
        if (len < SERIAL_BUF_LEN) {
            b = get_sfr(SBUF);
            RX_BUF[RX_tail++] = b;
        }
        set_sbit(RI, 0);
    }
    if (get_sbit(TI)) {
        len = TX_tail - TX_head;
        if (len > 0) {
            b = TX_BUF[TX_head++];
            set_sfr(SBUF, b);
        }
        set_sbit(TI, 0);
    }
}

u8 SerialPutCharUnlocked(u8 ch) {
    u8 len;
    len = TX_tail - TX_head;
    if (len < SERIAL_BUF_LEN) {
        TX_BUF[TX_tail++] = ch;
    }
    set_sbit(TI, 1);
    return len;
}

u8 SerialPrintStrUnlocked(u8 *s) {
    u8 len;
    len = TX_tail - TX_head;
    for (; *s != '\0'; ++s) {
        if (len < SERIAL_BUF_LEN) {
            TX_BUF[TX_tail++] = *s;
            ++len;
        }
    }
    set_sbit(TI, 1);
    return len;
}

u16 SerialGetChar(void) REENTRANT {
    u8 len;
    u8 b;

    len = RX_tail - RX_head;
    if (len > 0) {
        b = RX_BUF[RX_head++];
        return (u16)b;
    } else {
        return 0xFFFFU;
    }
}
