#include "keil_c51.h"
#include "ucos_ii.h"

#define UTASK_STK_SIZE APP_CFG_STARTUP_TASK_STK_SIZE
#define UTASK_STK_BTM(stk) (stk + UTASK_STK_SIZE - 1)

#define UTASK_BASE_PRIO APP_CFG_STARTUP_TASK_PRIO

static u8 UTASK1_STK[UTASK_STK_SIZE];
static u8 UTASK2_STK[UTASK_STK_SIZE];

#define UTASK1_PRIO (UTASK_BASE_PRIO + 0u)
#define UTASK2_PRIO (UTASK_BASE_PRIO + 1u)

static u8 strequ(u8 *sa, u8 *sb) {
    for (; *sa != '\0'; ++sa) {
        if (*sb == '\0') {
            return 0;
        }
        if (*sa != *sb) {
            return 0;
        }
        ++sb;
    }
    return *sb == '\0';
}

def_sbit(P1_1, P1 ^ 1);
static void utask1(void *p_arg) REENTRANT {
    p_arg = p_arg;
    
    InitTimer0();
    InitSerial();

    for (;;) {
        set_sbit(P1_1, 0);
        OSTimeDlyHMSM(0, 0, 0, 200);
        set_sbit(P1_1, 1);
        OSTimeDlyHMSM(0, 0, 0, 200);
    }
}

static void utask2(void *p_arg) REENTRANT {
    u16 c;
    u8 cmd_buf[256];
    u8 cmd_len = 0;

    p_arg = p_arg;

    OS_ENTER_CRITICAL();
    SerialPrintStrUnlocked("UCOS2 C51\r\n");
    SerialPrintStrUnlocked("Author: Xuyang Wang (201883290090)\r\n");
    OS_EXIT_CRITICAL();

    for (;;) {
        c = SerialGetChar();
        if (c != 0xFFFFU) {
            if (cmd_len != 255) {
                OS_ENTER_CRITICAL();
                SerialPutCharUnlocked(c);
                OS_EXIT_CRITICAL();
                if (c == '\r') {
                    if (strequ(cmd_buf, "help")) {
                        OS_ENTER_CRITICAL();
                        SerialPrintStrUnlocked("coming soon...\r\n");
                        OS_EXIT_CRITICAL();
                    } else {
                        OS_ENTER_CRITICAL();
                        SerialPrintStrUnlocked("unknown command\r\n");
                        OS_EXIT_CRITICAL();
                    }
                    cmd_len = 0;
                } else {
                    cmd_buf[cmd_len++] = c;
                    cmd_buf[cmd_len] = '\0';
                }
            }
        }
    }
}

int main() {
    OSInit();
    OSTaskCreate(utask1, NULL, UTASK_STK_BTM(UTASK1_STK), UTASK1_PRIO);
    OSTaskCreate(utask2, NULL, UTASK_STK_BTM(UTASK2_STK), UTASK2_PRIO);
    OSStart();
    return 0;
}
