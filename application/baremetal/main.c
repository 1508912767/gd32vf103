// See LICENSE for license details.
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "nuclei_sdk_soc.h"

#define RECORD_START()
#define RECORD_END()

// If define SWIRQ_INTLEVEL_HIGHER equals 1 the software interrupt will have a higher interrupt level.
// the software interrupt will run during timer interrupt.
// If define SWIRQ_INTLEVEL_HIGHER equals 0 the software interrupt will have a lower interrupt level.
// the software interrupt will run after timer interrupt.
#define SWIRQ_INTLEVEL_HIGHER   0
#define HIGHER_INTLEVEL         2
#define LOWER_INTLEVEL          1

// 100ms
#define TIMER_TICKS             (SOC_TIMER_FREQ / 10)


static volatile uint32_t int_check_cnt = 0;

// timer interrupt handler
// non-vector mode interrupt
void eclic_mtip_handler(void)
{
    static uint32_t int_t_cnt = 0;    /* timer interrupt counter */
    printf("-------------------\r\n");
    printf("[IN TIMER INTERRUPT]timer interrupt hit %d times\r\n", int_t_cnt++);

    printf("[IN TIMER INTERRUPT]trigger software interrupt\r\n");
#if SWIRQ_INTLEVEL_HIGHER == 1
    printf("[IN TIMER INTERRUPT]software interrupt will run during timer interrupt\r\n");
#else
    printf("[IN TIMER INTERRUPT]software interrupt will run when timer interrupt finished\r\n");
#endif
    // trigger software interrupt
    SysTimer_SetSWIRQ();

    // Reload Timer Interrupt
    SysTick_Reload(TIMER_TICKS);

    printf("[IN TIMER INTERRUPT]timer interrupt end\r\n");
}

// timer software interrupt handler
// vector mode interrupt
__INTERRUPT void eclic_msip_handler(void)
{
    static uint32_t int_sw_cnt = 0;   /* software interrupt counter */

    // save CSR context
    SAVE_IRQ_CSR_CONTEXT();

    SysTimer_ClearSWIRQ();

    printf("[IN SOFTWARE INTERRUPT]software interrupt hit %d times\r\n", int_sw_cnt++);
    printf("[IN SOFTWARE INTERRUPT]software interrupt end\r\n");

    int_check_cnt ++;
    // restore CSR context
    RESTORE_IRQ_CSR_CONTEXT();
}

void print_misa(void)
{
    CSR_MISA_Type misa_bits;
    char misa_chars[30];
    uint8_t index = 0;

    misa_bits.d = __RV_CSR_READ(CSR_MISA);
    if (misa_bits.b.mxl == 1) {
        misa_chars[index++] = '3';
        misa_chars[index++] = '2';
    } else if (misa_bits.b.mxl == 2) {
        misa_chars[index++] = '6';
        misa_chars[index++] = '4';
    } else if (misa_bits.b.mxl == 3) {
        misa_chars[index++] = '1';
        misa_chars[index++] = '2';
        misa_chars[index++] = '8';
    }
    if (misa_bits.b.i) {
        misa_chars[index++] = 'I';
    }
    if (misa_bits.b.m) {
        misa_chars[index++] = 'M';
    }
    if (misa_bits.b.a) {
        misa_chars[index++] = 'A';
    }
    if (misa_bits.b.b) {
        misa_chars[index++] = 'B';
    }
    if (misa_bits.b.c) {
        misa_chars[index++] = 'C';
    }
    if (misa_bits.b.e) {
        misa_chars[index++] = 'E';
    }
    if (misa_bits.b.f) {
        misa_chars[index++] = 'F';
    }
    if (misa_bits.b.d) {
        misa_chars[index++] = 'D';
    }
    if (misa_bits.b.q) {
        misa_chars[index++] = 'Q';
    }
    if (misa_bits.b.h) {
        misa_chars[index++] = 'H';
    }
    if (misa_bits.b.j) {
        misa_chars[index++] = 'J';
    }
    if (misa_bits.b.l) {
        misa_chars[index++] = 'L';
    }
    if (misa_bits.b.n) {
        misa_chars[index++] = 'N';
    }
    if (misa_bits.b.s) {
        misa_chars[index++] = 'S';
    }
    if (misa_bits.b.p) {
        misa_chars[index++] = 'P';
    }
    if (misa_bits.b.t) {
        misa_chars[index++] = 'T';
    }
    if (misa_bits.b.u) {
        misa_chars[index++] = 'U';
    }
    if (misa_bits.b.v) {
        misa_chars[index++] = 'V';
    }
    if (misa_bits.b.x) {
        misa_chars[index++] = 'X';
    }

    misa_chars[index++] = '\0';

    printf("MISA: RV%s\r\n", misa_chars);
}

int main(void)
{
    uint32_t rval, seed;
    unsigned long hartid, clusterid;
    rv_csr_t misa;

    // get hart id of current cluster
    hartid = __get_hart_id();
    clusterid = __get_cluster_id();
    misa = __RV_CSR_READ(CSR_MISA);

    printf("Cluster %lu, Hart %lu, MISA: 0x%lx\r\n", clusterid, hartid, misa);
    print_misa();

    // Generate random value with seed
    seed = (uint32_t)(__get_rv_cycle()  | __get_rv_instret() | __RV_CSR_READ(CSR_MCYCLE));
    srand(seed);
    rval = rand();
    printf("Got rand integer %ld using seed %ld.\r\n", seed, rval);
    printf("Hello World From Nuclei RISC-V Processor!\r\n");


    uint8_t timer_intlevel, swirq_intlevel;
    int32_t returnCode;
    // Set TIMER Interrupt and Software Interrupt Level
    // According to the macro SWIRQ_INTLEVEL_HIGHER setting
#if SWIRQ_INTLEVEL_HIGHER == 1
    timer_intlevel = LOWER_INTLEVEL;
    swirq_intlevel = HIGHER_INTLEVEL;
#else
    timer_intlevel = HIGHER_INTLEVEL;
    swirq_intlevel = LOWER_INTLEVEL;
#endif

    // initialize timer
    printf("Initialize timer and start timer interrupt periodically\n\r");
    SysTick_Config(TIMER_TICKS);

    // initialize software interrupt as vector interrupt
    returnCode = ECLIC_Register_IRQ(SysTimerSW_IRQn, ECLIC_VECTOR_INTERRUPT,
                                    ECLIC_LEVEL_TRIGGER, swirq_intlevel, 0, (void*)eclic_msip_handler);

    // inital timer interrupt as non-vector interrupt
    returnCode = ECLIC_Register_IRQ(SysTimer_IRQn, ECLIC_NON_VECTOR_INTERRUPT,
                                    ECLIC_LEVEL_TRIGGER, timer_intlevel, 0, (void*)eclic_mtip_handler);

    // Enable interrupts in general.
    __enable_irq();
    // Wait for timer interrupt and software interrupt
    // triggered periodically
    while (int_check_cnt < 1);
    __disable_irq();
    printf("ECLIC Demo finished sucessfully in %d loops\n", 1);


    volatile uint64_t start, end;
    __disable_irq();
    // need to adapt the tick according to your SoC
    SysTick_Config(1);
    // SysTimer_Start();
    start = __get_rv_cycle();
    RECORD_START();
    // Should not enter interrupt handler due to irq disabled
    __WFI();
    RECORD_END();
    end = __get_rv_cycle();

    printf("CSV, WFI Start/End, %lu/%lu\n", (unsigned long)start, (unsigned long)end);
    printf("CSV, WFI Cost, %lu\n", (unsigned long)(end - start));


    /* enable the LED clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* configure LED GPIO port */
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    gpio_bit_reset(GPIOA, GPIO_PIN_0 | GPIO_PIN_3);

    while(1){
		gpio_bit_set(GPIOA, GPIO_PIN_3);
		delay_1ms(1000);
		gpio_bit_reset(GPIOA, GPIO_PIN_3);
		delay_1ms(1000);
    }
}

