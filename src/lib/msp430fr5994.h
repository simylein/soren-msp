#pragma once

#define pm5ctl0 (*(volatile unsigned char *)0x0130)

#define csctl0 (*(volatile unsigned char *)0x0161)
#define csctl3 (*(volatile unsigned short *)0x0166)

#define p1sel0 (*(volatile unsigned char *)0x020a)
#define p1sel1 (*(volatile unsigned char *)0x020c)
#define p4out (*(volatile unsigned char *)0x0223)
#define p4dir (*(volatile unsigned char *)0x0225)
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

#define adc12ctl0 (*(volatile unsigned short *)0x0800)
#define adc12ctl1 (*(volatile unsigned short *)0x0802)
#define adc12ctl2 (*(volatile unsigned short *)0x0804)
#define adc12ifgr0 (*(volatile unsigned short *)0x080c)
#define adc12mctl0 (*(volatile unsigned short *)0x0820)
#define adc12mem0 (*(volatile unsigned short *)0x0860)
