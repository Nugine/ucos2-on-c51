$NOMOD51

;   asm module name
NAME    OS_CPU_A

;   SFR locations
SP      DATA    081H
DPL     DATA    082H
DPH     DATA    083H
PSW     DATA    0D0H
ACC     DATA    0E0H
B       DATA    0F0H
TL0     DATA    08AH
TL1     DATA    08BH
TH0     DATA    08CH
TH1     DATA    08DH
TMOD    DATA    089H
SCON    DATA    098H
PCON    DATA    087H

;   BIT locations
TR0     BIT     08CH
TR1     BIT     08EH
EA      BIT     0AFH
ET0     BIT     0A9H
ES      BIT     0ACH

;   constant symbols
;   CLK = 11.0592MHz, T0_F = 50Hz
;       0B800H = 47104 = 65536 - (11.0592M / 12 / 50)
T0_N    EQU 0B800H
;   CLK = 11.0592MHz, baud rate = 9600bps
;       0FDH = 253 = 256 - 11.0592M / 12 / 32 / 9600
T1_N    EQU 0FDH


;   stack segment
?STACK  SEGMENT IDATA

;   push registers into the real stack
;       PSW, ACC, B, DPL, DPH, R0 ~ R7
;       13 Bytes
PUSHALL MACRO
    PUSH    PSW
    PUSH    ACC
    PUSH    B
    PUSH    DPL
    PUSH    DPH
    MOV     A,  R0
    PUSH    ACC
    MOV     A,  R1
    PUSH    ACC
    MOV     A,  R2
    PUSH    ACC
    MOV     A,  R3
    PUSH    ACC
    MOV     A,  R4
    PUSH    ACC
    MOV     A,  R5
    PUSH    ACC
    MOV     A,  R6
    PUSH    ACC
    MOV     A,  R7
    PUSH    ACC
ENDM

;   pop registers from the real stack
;       inverse of PUSHALL
POPALL  MACRO
    POP     ACC
    MOV     R7, A
    POP     ACC
    MOV     R6, A
    POP     ACC
    MOV     R5, A
    POP     ACC
    MOV     R4, A
    POP     ACC
    MOV     R3, A
    POP     ACC
    MOV     R2, A
    POP     ACC
    MOV     R1, A
    POP     ACC
    MOV     R0, A
    POP     DPH
    POP     DPL
    POP     B
    POP     ACC
    POP     PSW
ENDM

;   external symbols
EXTRN   DATA    (?C_IBP)
EXTRN   DATA    (?C_PBP)
EXTRN   DATA    (?C_XBP)
EXTRN   XDATA   (OSRunning)
EXTRN   XDATA   (OSTCBHighRdy)
EXTRN   XDATA   (OSTCBCur)
EXTRN   XDATA   (OSPrioHighRdy)
EXTRN   XDATA   (OSPrioCur)
EXTRN   CODE    (_?OSTaskSwHook)
EXTRN   CODE    (_?OSIntEnter)
EXTRN   CODE    (_?OSIntExit)
EXTRN   CODE    (_?OSTimeTick)
EXTRN   CODE    (_?DoSerialISR)
EXTRN   CODE    (_?DoInitSerial)

;   function segments
?PR?OSStartHighRdy?OS_CPU_A     SEGMENT CODE
?PR?OSCtxSw?OS_CPU_A            SEGMENT CODE
?PR?OSIntCtxSw?OS_CPU_A         SEGMENT CODE
?PR?OSTickISR?OS_CPU_A          SEGMENT CODE
?PR?InitTimer0?OS_CPU_A         SEGMENT CODE
?PR?SerialISR?OS_CPU_A          SEGMENT CODE
?PR?InitSerial?OS_CPU_A         SEGMENT CODE

;   function symbols
PUBLIC  OSStartHighRdy
PUBLIC  OSCtxSw
PUBLIC  OSIntCtxSw
PUBLIC  InitTimer0
PUBLIC  InitSerial

;   function OSStartHighRdy
RSEG    ?PR?OSStartHighRdy?OS_CPU_A
OSStartHighRdy:
    LCALL   _?OSTaskSwHook
    ;   OSRunning = TRUE
    MOV     DPTR,   #OSRunning
    MOV     A,      #01H
    MOVX    @DPTR,  A
__OSCtxSw_tailcall:
    ;   DPTR = OSTCBHighRdy->OSTCBStkPtr
    MOV     DPTR,   #OSTCBHighRdy+1
    MOVX    A,      @DPTR
    MOV     R0,     A
    INC     DPTR
    MOVX    A,      @DPTR
    MOV     DPL,    A
    MOV     DPH,    R0
    INC     DPTR
    MOVX    A,      @DPTR
    MOV     R0,     A
    INC     DPTR
    MOVX    A,      @DPTR
    MOV     DPL,    A
    MOV     DPH,    R0
    ;   R2 = real_stack_len
    ;   R1 = #?STACK
    MOVX    A,      @DPTR
    MOV     R2,     A
    INC     DPTR
    MOV     R1,     #?STACK
    ;   do {
    ;       @R1 = @DPTR;
    ;       ++DPTR, ++R1;
    ;   } while((--R2)!=0);
__restore_real_stack:
    MOVX    A,      @DPTR
    MOV     @R1,    A
    INC     DPTR
    INC     R1
    DJNZ    R2,     __restore_real_stack
    ;   restore SP
    DEC     R1
    MOV     A,      R1
    MOV     SP,     A
    ;   restore XBP
    MOV     ?C_XBP,     DPH
    MOV     ?C_XBP+1,   DPL
    ;   restore registers
    POPALL
    ;   return to task
    SETB    EA
    RETI

;   function OSCtxSw
RSEG    ?PR?OSCtxSw?OS_CPU_A
OSCtxSw:
    ;   save registers
    PUSHALL
__OSIntCtxSw_tailcall:
    ;   R2 = real_stack_len
    MOV     A,      SP
    CLR     C
    SUBB    A,      #?STACK-1
    MOV     R2,     A
    ;   DPTR = ?C_XBP - R2 - 1
    SETB    C
    MOV     A,      ?C_XBP+1
    SUBB    A,      R2
    MOV     DPL,    A
    MOV     A,      ?C_XBP
    SUBB    A,      #0
    MOV     DPH,    A
    ;   (R6, R7) = DPTR
    MOV     R7,     DPL
    MOV     R6,     DPH
    ;   save real_stack_len
    MOV     A,      R2
    MOVX    @DPTR,  A
    INC     DPTR
    ;   R1 = #?STACK
    MOV     R1,     #?STACK
    ;   do {
    ;       @DPTR = @R1;
    ;       ++DPTR, ++R1;
    ;   while((--R2)!=0);
__save_real_stack:
    MOV     A,      @R1
    MOVX    @DPTR,  A
    INC     DPTR
    INC     R1
    DJNZ    R2,     __save_real_stack
    ;   OSTCBCur->OSTCBStkPtr = (R6, R7)
    MOV     DPTR,   #OSTCBCur+1
    MOVX    A,      @DPTR
    MOV     R0,     A
    INC     DPTR
    MOVX    A,      @DPTR
    MOV     DPL,    A
    MOV     DPH,    R0
    INC     DPTR
    MOV     A,      R6
    MOVX    @DPTR,  A
    INC     DPTR
    MOV     A,      R7
    MOVX    @DPTR,  A
    ;   call function OSTaskSwHook
    LCALL   _?OSTaskSwHook
    ;   OSTCBCur = OSTCBHighRdy
    MOV     DPTR,   #OSTCBHighRdy+1
    MOVX    A,      @DPTR
    MOV     DPTR,   #OSTCBCur+1
    MOVX    @DPTR,  A
    MOV     DPTR,   #OSTCBHighRdy+2
    MOVX    A,      @DPTR
    MOV     DPTR,   #OSTCBCur+2
    MOVX    @DPTR,  A
    ;   OSPrioCur = OSPrioHighRdy
    MOV     DPTR,   #OSPrioHighRdy
    MOVX    A,      @DPTR
    MOV     DPTR,   #OSPrioCur
    MOVX    @DPTR,  A
    ; resume high rdy
    LJMP    __OSCtxSw_tailcall

;   function OSIntCtxSw
RSEG    ?PR?OSIntCtxSw?OS_CPU_A
OSIntCtxSw:
    ;   remove the return address of OSIntExit and OSIntCtxSw
    MOV     A,      SP
    CLR     C
    SUBB    A,      #04H
    MOV     SP,     A
    LJMP    __OSIntCtxSw_tailcall

;   Timer 0 ISR
CSEG    AT  000BH
    LJMP    OSTickISR

;   function OSTickISR
RSEG    ?PR?OSTickISR?OS_CPU_A
OSTickISR:
    CLR     EA
    PUSHALL
    LCALL   _?OSIntEnter
    CLR     TR0
    LCALL   _?OSTimeTick
    MOV     TL0,    #LOW(T0_N)
    MOV     TH0,    #HIGH(T0_N)
    SETB    TR0
    LCALL   _?OSIntExit
    POPALL
    SETB    EA
    RETI

;   function InitTimer0
RSEG    ?PR?InitTimer0?OS_CPU_A
InitTimer0:
    CLR     EA
    ANL     TMOD,   #0F0H
    ORL     TMOD,   #001H
    MOV     TL0,    #LOW(T0_N)
    MOV     TH0,    #HIGH(T0_N)
    SETB    TR0
    SETB    ET0
    SETB    EA
    RET

;   Serial ISR
CSEG    AT  0023H
    LJMP    SerialISR

;   function SerialISR
RSEG    ?PR?SerialISR?OS_CPU_A
SerialISR:
    CLR     EA
    PUSHALL
    LCALL   _?OSIntEnter
    LCALL   _?DoSerialISR
    LCALL   _?OSIntExit
    POPALL
    SETB    EA
    RETI

;   function InitSerial
RSEG    ?PR?InitSerial?OS_CPU_A
InitSerial:
    CLR     EA
    LCALL   _?DoInitSerial
    ANL     TMOD,   #0FH
    ORL     TMOD,   #20H
    MOV     TH1,    #T1_N
    MOV     TL1,    #T1_N
    MOV     SCON,   #50H
    MOV     PCON,   #00H
    SETB    TR1
    SETB    ES
    SETB    EA
    RET

END
