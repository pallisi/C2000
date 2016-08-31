/*
 *
 * Port of
 * RobG's LCD graphic library
 * this uses hardware SPI
 *
 * Ported on: 12-24-2012
 *      Author: RobG /  Timotet
 *
 * main.c
 */


#include "colorLCD.h"
#include "typedefs.h"
//#include "memory.h"

#include "DSP28x_Project.h" // Device Header file and Examples Include File

#include "f2802x_common/include/clk.h"
#include "f2802x_common/include/flash.h"
#include "f2802x_common/include/gpio.h"
#include "f2802x_common/include/pie.h"
#include "f2802x_common/include/pll.h"
#include "f2802x_common/include/wdog.h"
#include "f2802x_common/include/spi.h"
#include "f2802x_common/include/adc.h"
#include "f2802x_common/include/pwm.h"



/* function prototypes */
void gpio_init(void);

interrupt void spiRxIsr(void);
void spi_init(void);

ADC_Handle myAdc;
CLK_Handle myClk;
FLASH_Handle myFlash;
GPIO_Handle myGpio;
PIE_Handle myPie;
SPI_Handle mySpi;
CPU_Handle myCpu;
PLL_Handle myPll;
WDOG_Handle myWDog;

void main(void) {

	// Initialize all the handles needed for this application
	    myAdc = ADC_init((void *)ADC_BASE_ADDR, sizeof(ADC_Obj));
	    myClk = CLK_init((void *)CLK_BASE_ADDR, sizeof(CLK_Obj));
	    myCpu = CPU_init((void *)NULL, sizeof(CPU_Obj));
	    myFlash = FLASH_init((void *)FLASH_BASE_ADDR, sizeof(FLASH_Obj));
	    myGpio = GPIO_init((void *)GPIO_BASE_ADDR, sizeof(GPIO_Obj));
	    myPie = PIE_init((void *)PIE_BASE_ADDR, sizeof(PIE_Obj));
	    myPll = PLL_init((void *)PLL_BASE_ADDR, sizeof(PLL_Obj));
	    myWDog = WDOG_init((void *)WDOG_BASE_ADDR, sizeof(WDOG_Obj));
	    mySpi = SPI_init((void *)SPIA_BASE_ADDR, sizeof(SPI_Obj));

	    // Perform basic system initialization
	       WDOG_disable(myWDog);
	       CLK_enableAdcClock(myClk);
	       (*Device_cal)();
	       CLK_disableAdcClock(myClk);

	       //Select the internal oscillator 1 as the clock source
	       CLK_setOscSrc(myClk, CLK_OscSrc_Internal);

	       // Setup the PLL for x12 /2 which will yield 60Mhz = 10Mhz * 12 / 2
	       PLL_setup(myPll, PLL_Multiplier_12, PLL_DivideSelect_ClkIn_by_2);

	       // Disable the PIE and all interrupts
	       PIE_setDebugIntVectorTable(myPie);
	       PIE_enable(myPie);
	       CPU_disableGlobalInts(myCpu);
	       CPU_clearIntFlags(myCpu);
	       CPU_enableGlobalInts(myCpu);

	    // If running from flash copy RAM only functions to RAM
	#ifdef _FLASH
	    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
	#endif

	gpio_init();

	spi_init();
	initLCD(); // or whatever LCD you are using

	for(;;){

		clearScreen(1);
				setColor(COLOR_16_RED );
				drawString(15,20, 5,"KALA");
				setColor(COLOR_16_RED );
				drawString(15,80, 5,"XRISTOUGENNA");
				setColor(COLOR_16_GREEN);
				drawString(15,150, 5,"STIN KORINA");
				setColor(COLOR_16_GREEN);
				drawString(15,200, 5,"KAI TIN ARIANNA!");
				DSP28x_usDelay(50000000);

		}

}



void gpio_init(void){

	// set up GPIO's for 2.2" RobG booster pack
	// see colorLCD.h for setup of AIO pin defines
	GPIO_setMode(myGpio, GPIO_Number_0  , GPIO_0_Mode_GeneralPurpose);
	GPIO_setMode(myGpio, GPIO_Number_1 , GPIO_0_Mode_GeneralPurpose);
	GPIO_setMode(myGpio, GPIO_Number_2 , GPIO_0_Mode_GeneralPurpose);
	GPIO_setMode(myGpio, GPIO_Number_3 , GPIO_0_Mode_GeneralPurpose);
	GPIO_setMode(myGpio, LCD_SCE_PIN  , GPIO_0_Mode_GeneralPurpose);
	GPIO_setMode(myGpio, LCD_DC_PIN  , GPIO_0_Mode_GeneralPurpose);

	GPIO_setDirection(myGpio,  GPIO_Number_0 , GPIO_Direction_Output);
	GPIO_setDirection(myGpio,  GPIO_Number_1 , GPIO_Direction_Output);
	GPIO_setDirection(myGpio,  GPIO_Number_2 , GPIO_Direction_Output);
	GPIO_setDirection(myGpio,  GPIO_Number_3 , GPIO_Direction_Output);
	GPIO_setDirection(myGpio,  LCD_SCE_PIN , GPIO_Direction_Output);
	GPIO_setDirection(myGpio,  LCD_DC_PIN , GPIO_Direction_Output);

	GPIO_setPullUp(myGpio, GPIO_Number_0 , GPIO_PullUp_Disable);
	GPIO_setPullUp(myGpio, GPIO_Number_1 , GPIO_PullUp_Disable);
	GPIO_setPullUp(myGpio, GPIO_Number_2 , GPIO_PullUp_Disable);
	GPIO_setPullUp(myGpio, GPIO_Number_3 , GPIO_PullUp_Disable);
	GPIO_setPullUp(myGpio, LCD_SCE_PIN , GPIO_PullUp_Disable);
	GPIO_setPullUp(myGpio, LCD_DC_PIN , GPIO_PullUp_Disable);

	GPIO_setHigh(myGpio, GPIO_Number_0);
	GPIO_setHigh(myGpio, GPIO_Number_1);
	GPIO_setHigh(myGpio, GPIO_Number_2);
	GPIO_setHigh(myGpio, GPIO_Number_3);
	GPIO_setHigh(myGpio, LCD_SCE_PIN);
	GPIO_setLow(myGpio, LCD_DC_PIN);

}

void spi_init(void){

	// enable Spia clock
	CLK_enableSpiaClock(myClk);
	//set up GPIO's for SPI
	GPIO_setPullUp(myGpio, LCD_MISO_PIN, GPIO_PullUp_Disable);
    GPIO_setPullUp(myGpio, LCD_SCLK_PIN, GPIO_PullUp_Disable);
    GPIO_setPullUp(myGpio, LCD_MOSI_PIN, GPIO_PullUp_Disable);
    GPIO_setPullUp(myGpio, SPISTE_PIN, GPIO_PullUp_Disable);     // hardware chip select

    GPIO_setQualification(myGpio, LCD_MISO_PIN, GPIO_Qual_ASync);
    GPIO_setQualification(myGpio, LCD_SCLK_PIN, GPIO_Qual_ASync);
    GPIO_setQualification(myGpio, LCD_MOSI_PIN, GPIO_Qual_ASync);
    GPIO_setQualification(myGpio, SPISTE_PIN, GPIO_Qual_ASync);

    GPIO_setMode(myGpio, LCD_MISO_PIN, GPIO_16_Mode_SPISIMOA);
    GPIO_setMode(myGpio, LCD_SCLK_PIN, GPIO_18_Mode_SPICLKA);
    GPIO_setMode(myGpio, LCD_MOSI_PIN, GPIO_17_Mode_SPISOMIA);
    GPIO_setMode(myGpio, SPISTE_PIN, GPIO_19_Mode_SPISTEA_NOT);


    // Resets the serial peripheral interface (SPI)
    SPI_reset(mySpi);
    // Initializes the serial peripheral interface (SPI) object handle
    SPI_enable(mySpi);
    // Enables the serial peripheral interface (SPI) transmit and receive channels
    SPI_enableChannels(mySpi);
    // Enable master mode
    SPI_setMode(mySpi, SPI_Mode_Master);
    // Enables the serial peripheral interface (SPI) masater/slave transmit mode
    SPI_enableTx(mySpi);
    // Sets the serial peripheral interface (SPI) character length
    SPI_setCharLength(mySpi, SPI_CharLength_8_Bits);
    // Sets the serial peripheral interface (SPI) clock phase
    SPI_setClkPhase(mySpi, SPI_ClkPhase_Normal);
    //Sets the serial peripheral interface (SPI) clock polarity
    SPI_setClkPolarity(mySpi, SPI_ClkPolarity_OutputFallingEdge_InputRisingEdge);
    //Enables tri wire or four wire SPI
    SPI_setTriWire(mySpi, SPI_TriWire_NormalFourWire);
    //Set STE pin to active low
    SPI_setSteInv(mySpi, SPI_SteInv_ActiveLow);
    //Enables the serial peripheral interface (SPI) interrupt
    SPI_enableInt(mySpi);
    //Sets the serial peripheral interface (SPI) baud rate
    SPI_setBaudRate(mySpi, (SPI_BaudRate_e)0x00);
    //enable the SPI
    SPI_enable(mySpi);
    //Set so breakpoints don't disturb xmission
    SPI_setPriority(mySpi, SPI_Priority_FreeRun);

    PIE_registerPieIntHandler(myPie, PIE_GroupNumber_6, PIE_SubGroupNumber_1, (intVec_t)&spiRxIsr);
    PIE_enableInt(myPie, PIE_GroupNumber_6, PIE_InterruptSource_SPIARX);

    CPU_enableInt(myCpu, CPU_IntNumber_6);

}

interrupt void spiRxIsr(void)
{
    LCD_DESELECT; //set Sce high

    SPI_read(mySpi);

    // Issue PIE ack
    PIE_clearInt(myPie, PIE_GroupNumber_6);
    GPIO_toggle(myGpio, GPIO_Number_0); //toggle led0 on C2000 launchpad

}



