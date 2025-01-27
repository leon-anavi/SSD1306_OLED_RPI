

/*!
	@file SSD1306_OLED_RPI/examples/src/SPEED_TEST/main.cpp
	@author Gavin Lyons
	@brief Test file for SSD1306_OLED library,  showing fps frame rate per second
	
	Project Name: SSD1306_OLED_RPI
	
	@test
		-# Test 601 FPS test frame rate per second
		
	@details
		-# results data for test file
		-# I2C_Speed = 0 Fps 7 bcm2835_i2c_set_baudrate(100000) 
		-# I2C_Speed = 2500 Fps 6 bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_2500) 
		-# I2C_Speed = 626 Fps 16 bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_626) 
		-# I2C_Speed = 150 Fps 24 bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_150) 
		-# I2C_Speed = 148 FPS 25 bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_148) 
*/

#include <time.h>
#include <stdio.h>
#include <bcm2835.h>
#include "SSD1306_OLED.hpp"

#define myOLEDwidth  128
#define myOLEDheight 64
const uint16_t I2C_Speed = 626; //  bcm2835I2CClockDivider 
const uint8_t I2C_Address = 0x3C;
SSD1306 myOLED(myOLEDwidth ,myOLEDheight) ; // instantiate  an object

// vars for the test
uint16_t count  = 0;
bool colour = 1;
uint64_t  previousCounter =0;
uint8_t  screenBuffer[myOLEDwidth * (myOLEDheight / 8) ]; 

// =============== Function prototype ================
uint8_t  setup(void);
void myLoop(void);
void display_buffer(long , int );
void EndTests(void);
static uint64_t counter( void );

	
// ======================= Main ===================
int main(int argc, char **argv)
{

	if (!setup())
	{
		return -1;
	}
	printf("OLED Frame rate per second test , ends at 1000\r\n");
	myLoop();
	EndTests();
	return 0;
}
// ======================= End of main  ===================

void EndTests()
{
	myOLED.OLEDPowerDown(); //Switch off display
	bcm2835_close(); // Close the library
	printf("OLED End\r\n");
}

uint8_t setup()
{
	if(!bcm2835_init())
	{
		printf("Error 1201 Cannot init bcm2835 library\n");
		return -1;
	}
	bcm2835_delay(50);
	printf("OLED Begin\r\n");
	myOLED.OLEDbegin(I2C_Speed, I2C_Address); // initialize the OLED
	myOLED.OLEDFillScreen(0x01, 0); //splash screen bars
	bcm2835_delay(1500);
	myOLED.setTextColor(WHITE);
	myOLED.setTextSize(1);
	myOLED.buffer = (uint8_t*) &screenBuffer;  
	return 1;
}

void myLoop() {

		while (count < 1000)
		{
			static long framerate = 0;
			display_buffer(framerate, count);
			framerate++;
			count++;
		}

}

void display_buffer(long currentFramerate, int count)
{
	// Values to count frame rate per second
	static long lastFramerate = 0;
	static uint16_t fps;
	uint64_t currentCounter = counter();
	if(currentCounter - previousCounter >= 1000000000)
	{
		//(Rolls over every 1 second)
		fps = currentFramerate - lastFramerate;
		lastFramerate = currentFramerate ;
		previousCounter = currentCounter;
		colour = !colour;
	}
	
	myOLED.OLEDclearBuffer();
	myOLED.setCursor(0, 0);
	myOLED.print("SSD1306");

	myOLED.setCursor(0, 20);
	myOLED.print("G Lyons");

	myOLED.setCursor(0, 30);
	myOLED.print(count);

	myOLED.setCursor(0, 40);
	myOLED.print(fps);
	myOLED.print(" fps");
	myOLED.setCursor(0, 50);
	myOLED.print("V 1.4.0");
	myOLED.drawFastVLine(64, 0, 63, WHITE);

	myOLED.fillRect(70, 10, 20, 20, colour);
	myOLED.fillCircle(110, 20, 10, !colour);
	myOLED.drawRoundRect(80, 40, 40, 20, 10, WHITE);

	myOLED.OLEDupdate();
}

// This func returns nano-seconds as a 64-bit unsigned number, 
// monotonically increasing, probably since system boot.
// The actual resolution looks like microseconds. returns nanoseconds
static uint64_t counter( void )
{
  struct timespec now;
  clock_gettime( CLOCK_MONOTONIC, &now );
  return  ((uint64_t)now.tv_sec * 1000000000U) + (uint64_t)now.tv_nsec; 
}
