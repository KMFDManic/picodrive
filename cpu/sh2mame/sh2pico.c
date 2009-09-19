#include <string.h>

// MAME types
typedef signed char  INT8;
typedef signed short INT16;
typedef signed int   INT32;
typedef unsigned int   UINT32;
typedef unsigned short UINT16;
typedef unsigned char  UINT8;

// pico memhandlers
unsigned int pico32x_read8(unsigned int a);
unsigned int pico32x_read16(unsigned int a);
unsigned int pico32x_read32(unsigned int a);
void pico32x_write8(unsigned int a, unsigned int d);
void pico32x_write16(unsigned int a, unsigned int d);
void pico32x_write32(unsigned int a, unsigned int d);

#define RB pico32x_read8
#define RW pico32x_read16
#define RL pico32x_read32
#define WB pico32x_write8
#define WW pico32x_write16
#define WL pico32x_write32

// some stuff from sh2comn.h
#define T	0x00000001
#define S	0x00000002
#define I	0x000000f0
#define Q	0x00000100
#define M	0x00000200

#define AM	0xc7ffffff

#define FLAGS	(M|Q|I|S|T)

#define Rn	((opcode>>8)&15)
#define Rm	((opcode>>4)&15)

#include "sh2.c"

void sh2_reset(SH2 *sh2)
{
	int save_is_slave;
//	cpu_irq_callback save_irqcallback;

//	save_irqcallback = sh2->irq_callback;
	save_is_slave = sh2->is_slave;

	memset(sh2, 0, sizeof(SH2));

	sh2->is_slave = save_is_slave;
//	sh2->irq_callback = save_irqcallback;

	sh2->pc = RL(0);
	sh2->r[15] = RL(4);
	sh2->sr = I;

	sh2->internal_irq_level = -1;
}

/* Execute cycles - returns number of cycles actually run */
int sh2_execute(SH2 *sh2_, int cycles)
{
	sh2 = sh2_;
	sh2_icount = cycles;

	do
	{
		UINT32 opcode;

		if (sh2->delay)
		{
			opcode = RW(sh2->delay);
			sh2->pc -= 2;
		}
		else
			opcode = RW(sh2->pc);

		sh2->delay = 0;
		sh2->pc += 2;
		sh2->ppc = sh2->pc;

		switch (opcode & ( 15 << 12))
		{
		case  0<<12: op0000(opcode); break;
		case  1<<12: op0001(opcode); break;
		case  2<<12: op0010(opcode); break;
		case  3<<12: op0011(opcode); break;
		case  4<<12: op0100(opcode); break;
		case  5<<12: op0101(opcode); break;
		case  6<<12: op0110(opcode); break;
		case  7<<12: op0111(opcode); break;
		case  8<<12: op1000(opcode); break;
		case  9<<12: op1001(opcode); break;
		case 10<<12: op1010(opcode); break;
		case 11<<12: op1011(opcode); break;
		case 12<<12: op1100(opcode); break;
		case 13<<12: op1101(opcode); break;
		case 14<<12: op1110(opcode); break;
		default: op1111(opcode); break;
		}

		if (sh2->test_irq && !sh2->delay)
		{
//			CHECK_PENDING_IRQ("mame_sh2_execute");
			sh2->test_irq = 0;
		}
		sh2_icount--;
	}
	while (sh2_icount > 0);

	return cycles - sh2_icount;
}

void sh2_init(SH2 *sh2)
{
	memset(sh2, 0, sizeof(*sh2));
}

