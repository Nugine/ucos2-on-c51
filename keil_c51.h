#ifndef __KEIL_C51_H__
#define __KEIL_C51_H__

// ---------------------------------------------

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

typedef signed char i8;
typedef signed int i16;
typedef signed long i32;

typedef float f32;
typedef double f64;

// ---------------------------------------------

#ifdef __KEIL_C51
#define SFR(name, addr) sfr name = addr;
#define SBIT(name, addr) sbit name = addr;
#else
#define SFR(name, addr)
#define SBIT(name, addr)
#endif

/*  BYTE Register  */
SFR(P0, 0x80)
SFR(P1, 0x90)
SFR(P2, 0xA0)
SFR(P3, 0xB0)
SFR(PSW, 0xD0)
SFR(ACC, 0xE0)
SFR(B, 0xF0)
SFR(SP, 0x81)
SFR(DPL, 0x82)
SFR(DPH, 0x83)
SFR(PCON, 0x87)
SFR(TCON, 0x88)
SFR(TMOD, 0x89)
SFR(TL0, 0x8A)
SFR(TL1, 0x8B)
SFR(TH0, 0x8C)
SFR(TH1, 0x8D)
SFR(IE, 0xA8)
SFR(IP, 0xB8)
SFR(SCON, 0x98)
SFR(SBUF, 0x99)

/*  BIT Register  */
/*  PSW   */
SBIT(CY, 0xD7)
SBIT(AC, 0xD6)
SBIT(F0, 0xD5)
SBIT(RS1, 0xD4)
SBIT(RS0, 0xD3)
SBIT(OV, 0xD2)
SBIT(P, 0xD0)

/*  TCON  */
SBIT(TF1, 0x8F)
SBIT(TR1, 0x8E)
SBIT(TF0, 0x8D)
SBIT(TR0, 0x8C)
SBIT(IE1, 0x8B)
SBIT(IT1, 0x8A)
SBIT(IE0, 0x89)
SBIT(IT0, 0x88)

/*  IE   */
SBIT(EA, 0xAF)
SBIT(ES, 0xAC)
SBIT(ET1, 0xAB)
SBIT(EX1, 0xAA)
SBIT(ET0, 0xA9)
SBIT(EX0, 0xA8)

/*  IP   */
SBIT(PS, 0xBC)
SBIT(PT1, 0xBB)
SBIT(PX1, 0xBA)
SBIT(PT0, 0xB9)
SBIT(PX0, 0xB8)

/*  P3  */
SBIT(RD, 0xB7)
SBIT(WR, 0xB6)
SBIT(T1, 0xB5)
SBIT(T0, 0xB4)
SBIT(INT1, 0xB3)
SBIT(INT0, 0xB2)
SBIT(TXD, 0xB1)
SBIT(RXD, 0xB0)

/*  SCON  */
SBIT(SM0, 0x9F)
SBIT(SM1, 0x9E)
SBIT(SM2, 0x9D)
SBIT(REN, 0x9C)
SBIT(TB8, 0x9B)
SBIT(RB8, 0x9A)
SBIT(TI, 0x99)
SBIT(RI, 0x98)

#undef SFR
#undef SBIT

// ---------------------------------------------

#ifdef __KEIL_C51
#define def_sbit(name, addr) sbit name = (addr)
#define set_sbit(name, bit) (name = bit)
#define set_sfr(name, byte) (name = byte)
#define get_sfr(name)   (name)
#define flip_sbit(name) (name = !name)
#define get_sbit(name)  (name)
#else
#define def_sbit(name, pos)
#define set_sbit(name, bit)
#define set_sfr(name, byte)
#define get_sfr(name)   (1u)
#define flip_sbit(name)
#define get_sbit(name) (1u)
#endif

#ifdef __KEIL_C51
#define DATA data
#define IDATA idata
#define PDATA pdata
#define XDATA xdata
#else
#define DATA
#define IDATA
#define PDATA
#define XDATA
#endif

#ifdef __KEIL_C51
#define INTERRUPT(n) interrupt n
#define REENTRANT reentrant
#else
#define INTERRUPT(n)
#define REENTRANT
#endif

// ---------------------------------------------

extern void _nop_ (void);

#endif
