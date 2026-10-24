#pragma once

#define pm5ctl0 (*(volatile unsigned char *)0x0130)

#define p6in (*(volatile unsigned char *)0x0241)
#define p6out (*(volatile unsigned char *)0x0243)
#define p6dir (*(volatile unsigned char *)0x0245)
#define p6sel0 (*(volatile unsigned char *)0x024b)
#define p6sel1 (*(volatile unsigned char *)0x024d)
#define p8out (*(volatile unsigned char *)0x0263)
#define p8dir (*(volatile unsigned char *)0x0265)

#define ta0ctl (*(volatile unsigned short *)0x0340)
#define ta0cctl0 (*(volatile unsigned short *)0x0342)
#define ta0r (*(volatile unsigned short *)0x0350)
#define ta0ccr0 (*(volatile unsigned short *)0x0352)
#define ta0ccr1 (*(volatile unsigned short *)0x0354)
