	//============ Address_ map_arms============= 
	.equ  DDR_BASE,             0x00000000
	.equ  DDR_END,              0x3FFFFFFF
	.equ  A9_ONCHIP_BASE,       0xFFFF0000
	.equ  A9_ONCHIP_END,        0xFFFFFFFF
	.equ  SDRAM_BASE,           0xC0000000
	.equ  SDRAM_END,            0xC3FFFFFF
	.equ  FPGA_ONCHIP_BASE,     0xC8000000
	.equ  FPGA_ONCHIP_END,      0xC803FFFF
	.equ  FPGA_CHAR_BASE,       0xC9000000
	.equ  FPGA_CHAR_END,        0xC9001FFF
	/* Cyclone V FPGA devices */
	.equ  LEDR_BASE,             0xFF200000
	.equ  HEX3_HEX0_BASE,        0xFF200020
	.equ  HEX5_HEX4_BASE,        0xFF200030
	.equ  SW_BASE,               0xFF200040
	.equ  KEY_BASE,              0xFF200050
	.equ  JP1_BASE,              0xFF200060
	.equ  JP2_BASE,              0xFF200070
	.equ  PS2_BASE,              0xFF200100
	.equ  PS2_DUAL_BASE,         0xFF200108
	.equ  JTAG_UART_BASE,        0xFF201000
	.equ  JTAG_UART_2_BASE,      0xFF201008
	.equ  IrDA_BASE,             0xFF201020
	.equ  TIMER_BASE,            0xFF202000
	.equ  TIMER_2_BASE,          0xFF202020
	.equ  AV_CONFIG_BASE,        0xFF203000
	.equ  PIXEL_BUF_CTRL_BASE,   0xFF203020
	.equ  CHAR_BUF_CTRL_BASE,    0xFF203030
	.equ  AUDIO_BASE,            0xFF203040
	.equ  VIDEO_IN_BASE,         0xFF203060
	.equ  ADC_BASE,              0xFF204000

/* Cyclone V HPS devices */
	.equ   HPS_GPIO1_BASE,       0xFF709000
	.equ   I2C0_BASE,            0xFFC04000
	.equ   I2C1_BASE,            0xFFC05000
	.equ   I2C2_BASE,            0xFFC06000
	.equ   I2C3_BASE,            0xFFC07000
	.equ   HPS_TIMER0_BASE,      0xFFC08000
	.equ   HPS_TIMER1_BASE,      0xFFC09000
	.equ   HPS_TIMER2_BASE,      0xFFD00000
	.equ   HPS_TIMER3_BASE,      0xFFD01000
	.equ   FPGA_BRIDGE,          0xFFD0501C

/* ARM A9 MPCORE devices */
	.equ   PERIPH_BASE,          0xFFFEC000   /* base address of peripheral devices */
	.equ   MPCORE_PRIV_TIMER,    0xFFFEC600   /* PERIPH_BASE + 0x0600 */

	/* Interrupt controller (GIC) CPU interface(s) */
	.equ   MPCORE_GIC_CPUIF,     0xFFFEC100   /* PERIPH_BASE + 0x100 */
	.equ   ICCICR,               0x00         /* CPU interface control register */
	.equ   ICCPMR,               0x04         /* interrupt priority mask register */
	.equ   ICCIAR,               0x0C         /* interrupt acknowledge register */
	.equ   ICCEOIR,              0x10         /* end of interrupt register */
	/* Interrupt controller (GIC) distributor interface(s) */
	.equ   MPCORE_GIC_DIST,      0xFFFED000   /* PERIPH_BASE + 0x1000 */
	.equ   ICDDCR,               0x00         /* distributor control register */
	.equ   ICDISER,              0x100        /* interrupt set-enable registers */
	.equ   ICDICER,              0x180        /* interrupt clear-enable registers */
	.equ   ICDIPTR,              0x800        /* interrupt processor targets registers */
	.equ   ICDICFR,              0xC00        /* interrupt configuration registers */
//============ Address_ map_arms=============

/* define.s */
	.equ	EDGE_TRIGGERED,  0x1
	.equ	LEVEL_SENSITIVE, 0x0
	.equ	CPU0,         	0x01	// bit-mask; bit 0 represents cpu0
	.equ	ENABLE, 	0x1
	.equ	KEY0, 		0b0001 	// 1
	.equ	KEY1, 		0b0010 	// 2
	.equ	KEY2,		0b0100 	// 4
	.equ	KEY3,		0b1000 	// 8
	.equ	NONE,		0b0000 	// 0
	.equ	RIGHT,		1
	.equ	LEFT,		2
	.equ	USER_MODE,	0b10000		// processor mode
	.equ	FIQ_MODE,	0b10001
	.equ	IRQ_MODE,	0b10010
	.equ	SVC_MODE,	0b10011
	.equ	ABORT_MODE,	0b10111
	.equ	UNDEF_MODE,	0b11011
	.equ	SYS_MODE,	0b11111
	.equ	INT_ENABLE,	0b01000000	// bit 7,6 =01 
	.equ	INT_DISABLE, 	0b11000000	// bit 7,6= 11
/* This files provides interrupt IDs */

/* FPGA interrupts (there are 64 in total; only a few are defined below) */
	.equ	FPGA_IRQ0, 			72
	.equ	INTERVAL_TIMER_IRQ, 		72
	.equ	FPGA_IRQ1, 			73
	.equ	KEYS_IRQ, 			73
/* ARM A9 MPCORE devices (there are many; only a few are defined below) */
	.equ	MPCORE_GLOBAL_TIMER_IRQ,	27
	.equ	MPCORE_PRIV_TIMER_IRQ,		29
	.equ	MPCORE_WATCHDOG_IRQ,		30




/*********************************************************************************
 * Initialize the exception vector table
 ********************************************************************************/
	.section .vectors, "ax" //벡터테이블 설정 ppt DE1-SoC 컴퓨터 64p 설명 참조
	B 	_start			// reset vector         0x000
	B 	SERVICE_UND		// undefined instruction vector 0x004
	B 	SERVICE_SVC		// software interrrupt vector 0x008
	B 	SERVICE_ABT_INST	// aborted prefetch vector  0x00C
	B 	SERVICE_ABT_DATA	// aborted data vector 0x010
	.word 	0			// unused vector    0x014  존재하지 않음 
	B 	SERVICE_IRQ		// IRQ interrupt vector 0x018
	B 	SERVICE_FIQ		// FIQ interrupt vector   0x01c
	.text
	.global	_start
_start:	
	/* Set up stack pointers for IRQ and SVC processor modes */
	MOV	R1, #INT_DISABLE | IRQ_MODE
	MSR	CPSR_c, R1			// change to IRQ mode // IRQ 모드를-> 상테레지스터에 저장 

	LDR	SP, =A9_ONCHIP_END - 3		// set IRQ stack to top of A9 onchip memory    address=  0xFFFFFFFF -3
	/* Change to SVC (supervisor) mode with interrupts disabled */
	MOV	R1, #INT_DISABLE | SVC_MODE   
	MSR	CPSR, R1			// change to supervisor mode  SUPERVISOR CALL 모드로 상테레지스터 변환 
	LDR	SP, =DDR_END - 3		// set SVC stack to top of DDR3 memory   SVC  -  DDR3 stack top으로 설정   0x3FFFFFFF -3

	BL	CONFIG_GIC			// configure the ARM generic interrupt controller

	// write to the pushbutton KEY interrupt mask register  P41  PIO components 참조 
	LDR	R0, =KEY_BASE			// push button key base address          .equ  KEY_BASE,              0xFF200050
	MOV	R1, #0xF			// enable interrupts from all four KEYs    Key 입력에대한 인터럽트전부 활성화  1111
	STR	R1, [R0, #0x8]			// interrupt mask register is (base + 8)   (key base + 8) 주소에  mask bit // 버튼클릭 인터럽트 활성화 

	// enable IRQ interrupts in the processor
	MOV	R0, #INT_ENABLE | SVC_MODE	// IRQ unmasked, MODE = SVC
	MSR	CPSR_c, R0
IDLE:
	B	IDLE				// main program simply idles
	.global CONFIG_GIC
CONFIG_GIC:
	/* To configure the FPGA KEYS interrupt (ID 73):
	 *	1. set the target to cpu0 in the ICDIPTRn register (addr 0xFFFED848) 
	 *	2. enable the interrupt in the ICDISERn register (addr 0xFFFED108) */
	LDR	R0, =0xFFFED848		// ICDIPTRn: processor targets register  0x48 = 72
	LDR	R1, =0x00000100		// set targets to cpu0 
	STR	R1, [R0]    //cpus offset 위치 0xFFFED800 ~ 0xFFFEDC00

	LDR	R0, =0xFFFED108		// ICDISERn: set enable register
	LDR	R1, =0x00000200		// set interrupt enable
	STR	R1, [R0]

	/* configure the GIC CPU interface */
    	LDR	R0, =MPCORE_GIC_CPUIF	// base address of CPU interface      0xFFFEC100

    	/* Set Interrupt Priority Mask Register (ICCPMR) */  //인터럽트 우선순위 마스크 레지스터 값보다 우선순위가 높은 인터럽트만 프로세스에 보낼수 있다 .
    	LDR	R1, =0xFFFF 		// 0xFFFF enables interrupts of all priorities levels */
    	STR	R1, [R0, #ICCPMR]	  //0xFFFEC104  set 1111

    	/* Set the enable bit in the CPU Interface Control Register (ICCICR). This bit
	 * allows interrupts to be forwarded to the CPU(s) */
    	MOV	R1, #0x1
    	STR	R1, [R0, #ICCICR]   0xFFFEC100  E=1  // 인터럽트가 cpu로 전송되는것을 활성화 
    
    	/* Set the enable bit in the Distributor Control Register (ICDDCR). This bit
	 * allows the distributor to forward interrupts to the CPU interface(s) */
//CPU 인터페이스에 보유중인 인터럽트를 전달할수 있다 .
    	LDR	R0, =MPCORE_GIC_DIST   // 0xFFFED000  ICDDCR E =1 활성화 -> 인터럽트를 인터페이스에 출력설정?
    	STR	R1, [R0, #ICDDCR]	// R1=0x1    
    
    	BX     	LR



/* Define the exception serice routines */
/*--- Undefined instructions --------------------------------------------------*/
SERVICE_UND:
B SERVICE_UND 
/*--- Software interrupts -----------------------------------------------------*/
SERVICE_SVC:	
    	B SERVICE_SVC 
/*--- Aborted data reads ------------------------------------------------------*/
SERVICE_ABT_DATA:
    	B SERVICE_ABT_DATA 
/*--- Aborted instruction fetch -----------------------------------------------*/
SERVICE_ABT_INST:
    	B SERVICE_ABT_INST 
/*--- IRQ ---------------------------------------------------------------------*/
SERVICE_IRQ:
    	PUSH	{R0-R7, LR} //R0~R7 내용  PUSH 보존용?
    
    	/* Read the ICCIAR in the CPU interface */
    	LDR	R4, =MPCORE_GIC_CPUIF   // 0xFFFEC100

    	LDR	R5, [R4, #ICCIAR] 	// read the interrupt ID   0xFFFEC10C

FPGA_IRQ1_HANDLER:
    	CMP	R5, #FPGA_IRQ1		// KEYS_IRQ 73인데 확인차 검사
UNEXPECTED:	
	BNE	UNEXPECTED    		// if not recognized, stop here
    	BL	KEY_ISR

EXIT_IRQ:
    	/* Write to the End of Interrupt Register (ICCEOIR) */
    	STR	R5, [R4, #ICCEOIR]   0XFFFEC110
    
    	POP	{R0-R7, LR}
    	SUBS	PC, LR, #4		// return from ISR

/*--- FIQ ---------------------------------------------------------------------*/
SERVICE_FIQ:
    	B	SERVICE_FIQ 
		
// -----------------ISR------------------------
.global	KEY_ISR
KEY_ISR:	
	LDR	R0, =KEY_BASE		// base address of pushbutton KEY parallel port  0xFF200050
	LDR	R1, [R0, #0xC]		// read edge capture register 
	STR	R1, [R0, #0xC]		// clear the interrupt
	LDR	R0, =KEY_HOLD		// base address of pushbutton KEY parallel port
	LDR	R2, [R0]		// read global variable
	EOR	R1, R2, R1		// toggle the bits of the global
	STR	R1, [R0]		// write to the global variable

	LDR	R0, =HEX3_HEX0_BASE	// based address of HEX display
	MOV	R2, #0			// blank the display by default

CHECK_KEY0:
	MOV	R3, #KEY0
	ANDS	R3, R3, R1		// check for KEY0
	BEQ	CHECK_KEY1
	MOV	R2, #0b00111111		// 0x3F (0), HEX0

CHECK_KEY1:
	MOV	R3, #KEY1
	ANDS	R3, R3, R1		// check for KEY1
	BEQ	CHECK_KEY2
	MOV	R3, #0b00000110		// 0x06 (1)
	ORR	R2, R2, R3, LSL #8	// bit 15-8 (HEX1)

CHECK_KEY2:
	MOV	R3, #KEY2
	ANDS	R3, R3, R1		// check for KEY2
	BEQ	CHECK_KEY3
	MOV	R3, #0b01011011		// 0x5B (2)
	ORR	R2, R2, R3, LSL #16	// bit 23-16 (HEX2)

CHECK_KEY3:
	MOV	R3, #KEY3
	ANDS	R3, R3, R1		// check for KEY2
	BEQ	END_KEY_ISR
	MOV	R3, #0b01001111		// 0x4F (3)
	ORR	R2, R2, R3, LSL #24	// bit 31-24 (HEX3)

END_KEY_ISR:
	STR	R2, [R0]		// display " "
	BX	LR

	.global	KEY_HOLD
KEY_HOLD:	.word	0b0000		// remembers which KEY is pressed/not
//--------------------ISR end-----------------------
