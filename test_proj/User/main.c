/**********************************************************
 * main.c
 * simple test project that demonstrates:
 * - use of the UART for input/output
 * - use of the GPIO for input/output
 * - use of the Standby mode
 * Microcontroller: CH32V003F4P6 (20-pin TSSOP)
 * Connections:
 * - UART1 Tx/Rx: PD5/PD6 goes to Prog. Rx/Tx respectively
 * - PC1: Push-button switch to ground
 * - PC2: LED cathode (Anode to V+ via a resistor)
 * shabaz - rev 1 - feb 2024
 ***********************************************************/

// ************ includes *****************
#include "debug.h"

// ************* defines *****************
#define FOREVER 1
#define delay_msec Delay_Ms
// LED control (low = LED off, high = LED on)
#define LED_OFF GPIO_SetBits(GPIOC, GPIO_Pin_2)
#define LED_ON GPIO_ResetBits(GPIOC, GPIO_Pin_2)
// button pressed is active low
#define BUTTON_PRESSED (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1) == 0)
#define BUTTON_UNPRESSED (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1) != 0)

// *********** global variables **********

// *********** function prototypes ********

// ************* functions ****************
void print_welcome(void) {
    printf("\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n");
    printf("#     #                             \r\n");                             
    printf("#     # ###### #      #       ####  \r\n");  
    printf("#     # #      #      #      #    # \r\n");
    printf("####### #####  #      #      #    # \r\n");
    printf("#     # #      #      #      #    # \r\n");
    printf("#     # #      #      #      #    # \r\n");
    printf("#     # ###### ###### ######  ####  \r\n");
    printf("\r\n");
    printf("Test Program 1\r\n");
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
}

// this function is used to provide a delay at startup.
// It is useful to allow time for a user to press the reset button
// for a debugger to connect, if there is any issue with the code.
// In production code this function can be omitted.
void start_delay(void) {
    // wait 2 seconds
    int i;
    for (i=0; i<5; i++) {
        printf("\r-");
        delay_msec(100);
        printf("\b\\");
        delay_msec(100);
        printf("\b|");
        delay_msec(100);
        printf("\b/");
        delay_msec(100);
    }
    printf("\b ");
}

// uart_config_rxtx is only needed if UART input/output is required
// otherwise, this function can be omitted, and just use 
// USART_Printf_Init(115200) and the printf() function
void uart_config_rxtx(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

// this function acts as a delay, but also checks the UART for a character,
// and exits early if a character was pressed. The character is returned in val.
uint8_t check_uart_input_msec(uint32_t msec, uint8_t *val) {
    int i;
    uint16_t ch;
    i = msec/16; // check every 16 msec
    if (i<1) {
        i=1;
    }
    while (i--) {
        if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET) {
            ch = USART_ReceiveData(USART1);
            *val = (uint8_t)(ch & 0xFF);
            return 1; // character received
        }
        delay_msec(16);
    }
    return 0; // no character received
}

// this function initializes the board (GPIO, etc)
void board_init(void) {
    // set up PC1 as input
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    // set up PC2 as output
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    // output mode has speed option (2/10/50 MHz)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // set all unused pins to inputs with pullups, to save power.
    // Note: this can prevent the debugger from connecting. A workaround is to
    // use the start_delay function before board_init is called, so that the 
    // reset button can be pressed within the first two seconds.
    // Port C:
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & ~(GPIO_Pin_1 | GPIO_Pin_2);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    // Port D:
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    // Port A:
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // switch off the LED initially
    LED_OFF;

    // enable external interrupt for PC1
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    // set EXTI1 bits to port C in the AFIO EXTICR register
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);
    // enable the interrupt in the NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn; // external line interrupts
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


// ***************************************
// ************** main function **********
// ***************************************
int main(void)
{
    uint8_t res;
    uint8_t val;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init(); // this sets up the SystemCoreClock rate
    USART_Printf_Init(115200); // this enables the use of printf()
    start_delay(); // allows time for debugger to connect if needed
    print_welcome();

    board_init(); // initialize GPIO etc.
    uart_config_rxtx(); // initialize UART for both directions

    while(FOREVER) {
        // sleep until the GPIO button is pressed
        PWR_EnterSTANDBYMode(PWR_STANDBYEntry_WFE);
        // GPIO button has been pressed
        LED_ON; // we've been woken by the user pressing the GPIO button!
        // wait for up to 1 second, checking the UART port
        res = check_uart_input_msec(1000, &val); // check for UART input for 1000 msec
        if(res) {
            printf("char pressed: '%c'\r\n", val);
        }
        LED_OFF;
        // we are done. Go back to sleep when this code loops.
        printf("Entering standby again.\r\n");
    }

    return(0); // this won't ever execute; OK for a warning on this line!
}
