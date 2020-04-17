#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

int j = 0, k = 0;
unsigned char clor = 0x3E;
void WriteStringToScreen(int val, int x, int y, unsigned char color);
void ShowICsStatuses(unsigned char color);
void interrupt new_8(void);
void interrupt new_9(void);
void interrupt new_10(void);
void interrupt new_11(void);
void interrupt new_12(void);
void interrupt new_13(void);
void interrupt new_14(void);
void interrupt new_15(void);
void interrupt new_70(void);
void interrupt new_71(void);
void interrupt new_72(void);
void interrupt new_73(void);
void interrupt new_74(void);
void interrupt new_75(void);
void interrupt new_76(void);
void interrupt new_77(void);
void interrupt (*old_8)(void);
void interrupt (*old_9)(void);
void interrupt (*old_10)(void);
void interrupt (*old_11)(void);
void interrupt (*old_12)(void);
void interrupt (*old_13)(void);
void interrupt (*old_14)(void);
void interrupt (*old_15)(void);
void interrupt (*old_70)(void);
void interrupt (*old_71)(void);
void interrupt (*old_72)(void);
void interrupt (*old_73)(void);
void interrupt (*old_74)(void);
void interrupt (*old_75)(void);
void interrupt (*old_76)(void);
void interrupt (*old_77)(void);
void RestoreInterrupts();


int main()
{
	unsigned far* fp;
    unsigned char value;
	// изменяем таблицу векторов прерывания
	old_8 = getvect(0x8);      // IRQ0 прерывание таймера, возникает 18,2 раза в секунду.
	old_9 = getvect(0x9);      // IRQ1 прерывание от клавиатуры.
	old_10 = getvect(0xA);     // IRQ2 используется для каскадирования аппаратных прерываний
	old_11 = getvect(0xB);     // IRQ3 прерывание асинхронного порта COM2.
	old_12 = getvect(0xC);     // IRQ4 прерывание асинхронного порта COM1.
	old_13 = getvect(0xD);     // IRQ5 прерывание от контроллера жесткого диска для XT.
	old_14 = getvect(0xE);     // IRQ6 прерывание генерируется контроллером флоппи диска после завершения операции
	old_15 = getvect(0xF);     //IRQ7 прерывание принтера.

	old_70 = getvect(0x70);    //IRQ8 прерывание от часов реального времени.
	old_71 = getvect(0x71);    //IRQ9 прерывание от контроллера EGA.
	old_72 = getvect(0x72);    //IRQ10 зарезервировано.
	old_73 = getvect(0x73);    //IRQ11 зарезервировано.
	old_74 = getvect(0x74);    //IRQ12 зарезервировано.
	old_75 = getvect(0x75);    //IRQ13 прерывание от математического сопроцессора.
	old_76 = getvect(0x76);    //IRQ14 прерывание от контроллера жесткого диска.
	old_77 = getvect(0x77);    //IRQ15 зарезервировано.

	setvect(0x90, new_8);
	setvect(0x91, new_9);
	setvect(0x92, new_10);
	setvect(0x93, new_11);
	setvect(0x94, new_12);
	setvect(0x95, new_13);
	setvect(0x96, new_14);
	setvect(0x97, new_15);

	setvect(0x8, new_70);
	setvect(0x9, new_71);
	setvect(0xA, new_72);
	setvect(0xB, new_73);
	setvect(0xC, new_74);
	setvect(0xD, new_75);
	setvect(0xE, new_76);
	setvect(0xF, new_77);

	//                                        ИНИЦИАЛИЗАЦИЯ
	_disable();
	//     Ведущий контроллер
	//value = inp(0x21);
	outp(0x20, 0x11);   // ICW1 (инициализация ведущего контроллера) - ICW4 будет вызвана,
					    // размер вектора прерывания - 8 байт
	outp(0x21, 0x90);   // ICW2
	outp(0x21, 0x04);   // ICW3
	outp(0x21, 0x01);   // ICW4
	//outp(0x21, value);
	//     Ведомый контроллер
	//value = inp(0xA1);
	outp(0xA0, 0x11);   // ICW1
	outp(0xA1, 0x80);   // ICW2
	outp(0xA1, 0x02);	// ICW3
	outp(0xA1, 0x01);	// ICW4
	//outp(0xA1, value);
	_enable();
	clrscr();
	FP_SEG(fp) = _psp;
	FP_OFF(fp) = 0x2c;
	_dos_keep(0,(_DS-_CS)+(_SP/16)+1); // Оставить программу резидентной
	return 0;
}


void WriteStringToScreen(int val, int x, int y, unsigned char color)
{
	int i = 0;
	unsigned char far* screen = (unsigned char far*)MK_FP(0xB800, 0);
	screen += x+160*y;
	for(i = 7; i >= 0; i--)
	{
	    screen--;
	    *screen = (char)(val % 2 + '0');
	    screen++;
	    *screen = color;
	    screen-=2;
	    val /= 2;
	}
}

void ShowICsStatuses(unsigned char color)
{
	unsigned char far* screen = (unsigned char far*)MK_FP(0xB800, 0);
	unsigned char* mess = "Master ISR, IRR, MASK: ";
	int i;

	int isr_master, isr_slave; // Interrupt Service Register - Регистр обслуживаемых прерываний
	int irr_master, irr_slave; // Interrupt Request Register - Регистр запросов на прерывания
	int imr_master, imr_slave; // Interrupt Mask Register  - Регистр масок

	imr_master = inp(0x21);
	imr_slave = inp(0xA1);

	outp(0x20, 0x0A);
	irr_master = inp(0x20);
	outp(0x20, 0x0B);
	isr_master = inp(0x20);

	outp(0xA0,0x0A);
	irr_slave = inp(0xA0);
	outp(0xA0,0x0B);
	isr_slave = inp(0xA0);

	for (i = 0; i < 24; i++) {
		*screen = *(mess + i);
		screen++;
		*screen = color;
		screen++;
	}

	WriteStringToScreen(isr_master, 63, 0, color);


	WriteStringToScreen(irr_master, 81, 0, color);


	WriteStringToScreen(imr_master, 99, 0, color);

	mess = "Slave ISR, IRR, MASK:  ";
	screen = (unsigned char far*)MK_FP(0xB800, 0);
	screen += 160;
	for (i = 0; i < 24; i++) {
		*screen = *(mess + i);
		screen++;
		*screen = color;
		screen++;
	}


	WriteStringToScreen(isr_slave, 63, 1, color);


	WriteStringToScreen(irr_slave, 81, 1, color);


	WriteStringToScreen(imr_slave, 99, 1, color);
}


void interrupt new_8(void)
{
	ShowICsStatuses(clor);
	(*old_8)();
}

void interrupt new_9(void)
{
	unsigned char value;
	if (clor == 0x3E && j == 1) {
		clor = 0x6E;
		k = 1;
		j = 0;
	}
	else {
		if (k == 0) {
			clor = 0x3E;
			j++;
		}
		else { k--; j = -1;}
	}

	ShowICsStatuses(clor);
	(*old_9)();

	value = inp(0x60);

	if (value == 0x01)
	{
		RestoreInterrupts();
		clrscr();
	}
}

void interrupt new_10(void)
{
	ShowICsStatuses(clor);
	(*old_10)();
}
void interrupt new_11(void)
{
	ShowICsStatuses(clor);
	(*old_11)();
}
void interrupt new_12(void)
{
	ShowICsStatuses(clor);
	(*old_12)();
}
void interrupt new_13(void)
{
	ShowICsStatuses(clor);
	(*old_13)();
}
void interrupt new_14(void)
{
	ShowICsStatuses(clor);
	(*old_14)();
}
void interrupt new_15(void)
{
	ShowICsStatuses(clor);
	(*old_15)();
}
void interrupt new_70(void)
{
	ShowICsStatuses(clor);
	(*old_70)();
}
void interrupt new_71(void)
{
	ShowICsStatuses(clor);
	(*old_71)();
}
void interrupt new_72(void)
{
	ShowICsStatuses(clor);
	(*old_72)();
}
void interrupt new_73(void)
{
	ShowICsStatuses(clor);
	(*old_73)();
}
void interrupt new_74(void)
{
	ShowICsStatuses(clor);
	(*old_74)();
}
void interrupt new_75(void)
{
	ShowICsStatuses(clor);
	(*old_75)();
}
void interrupt new_76(void)
{
	ShowICsStatuses(clor);
	(*old_76)();
}
void interrupt new_77(void)
{
	ShowICsStatuses(clor);
	(*old_77)();
}

void RestoreInterrupts()
{
	disable();

	setvect(0x90, old_8);
	setvect(0x91, old_9);
	setvect(0x92, old_10);
	setvect(0x93, old_11);
	setvect(0x94, old_12);
	setvect(0x95, old_13);
	setvect(0x96, old_14);
	setvect(0x97, old_15);

	setvect(0x8, old_70);
	setvect(0x9, old_71);
	setvect(0xA, old_72);
	setvect(0xB, old_73);
	setvect(0xC, old_74);
	setvect(0xD, old_75);
	setvect(0xE, old_76);
	setvect(0xF, old_77);

	enable();
}