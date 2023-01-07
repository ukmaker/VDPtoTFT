/*
 * mymain.cpp
 *
 *  Created on: Jul 19, 2022
 *      Author: Duncan
 */

#include "main.h"
#include "spi.h"
#include "vdp.h"

#include "GFX_HW_SPI.h"
#include "GFX_HW_PAR16.h"
#include "GFX_ILI9325.h"
#include "GFX_ILI9341.h"

#ifdef PAR_TFT
GFX_HW_PAR16 hwspi = GFX_HW_PAR16(GPIOC,
GPIOB, GPIO_PIN_11,
GPIOB, GPIO_PIN_12,
GPIOB, GPIO_PIN_2,
GPIOB, GPIO_PIN_14,
GPIOB, GPIO_PIN_15);

GFX_ILI9325 tft = GFX_ILI9325(hwspi);
#else
GFX_HW_SPI hwspi = GFX_HW_SPI(&hspi2, GPIOB, GPIO_PIN_9, GPIOB, GPIO_PIN_8, GPIOD, GPIO_PIN_2);
GFX_ILI9341 tft = GFX_ILI9341(hwspi);
#endif

#define BLOCKS_SIZE 9
#define BLOCKS_SPACING 3
#define BLOCK_TO_BLOCK (BLOCKS_SIZE + BLOCKS_SPACING)
#define BLOCKS_HEIGHT ((BLOCKS_SIZE + BLOCKS_SPACING) * 16)
#define BLOCKS_TOP ((240 - BLOCKS_HEIGHT) / 2)
#define BLOCKS_LEFT 3
#define BLOCK_POINTER_LEFT (BLOCKS_LEFT + BLOCKS_SIZE + 1)
#define BLOCK_POINTER_SIZE BLOCKS_SIZE

extern "C" {
	void mysetup();

	float setupYLevel;
	float setupRYLevel;
	float setupBYLevel;

void vsync() {
	tft.end();
	tft.setAddressWindow((320 - 256) / 2, (240 - 192) / 2, 256, 192);
}

void mysetup() {
	tft.init();
	tft.setRotation(3);
	HAL_Delay(10);
	tft.drawFilledRectangle(0,0,320,240,rgbMap[WHITE+1]);
}

void enableSetupGUI() {

	tft.drawFilledRectangle(BLOCKS_LEFT, BLOCKS_TOP, BLOCKS_SIZE, BLOCKS_HEIGHT,rgbMap[WHITE+1]);

	// write the colors as blocks down the left
	for (int i = 0; i < 15; i++) {
		uint16_t color = rgbMap[i + 1];
		tft.drawFilledRectangle(BLOCKS_LEFT, BLOCKS_TOP + i * BLOCK_TO_BLOCK, BLOCKS_SIZE, BLOCKS_SIZE,color);
	}

	vsync();
}

void disableSetupGUI() {
	tft.drawFilledRectangle(0,0,320,240,rgbMap[WHITE+1]);
	vsync();
}

void pushpixel(uint16_t c) {
	tft.pushPixel(c);
}

void initSetupColour(uint8_t c) {
	// draw a block of the given colour down the right hand side
	tft.drawFilledRectangle(300,30,10,160, rgbMap[c+1]);

	setupYLevel = YVolts[c];
	setupRYLevel = RYVolts[c];

	// clear the LHS pointer
	tft.drawFilledRectangle(BLOCK_POINTER_LEFT,0,BLOCK_POINTER_LEFT+BLOCK_POINTER_SIZE,192, rgbMap[WHITE+1]);

	uint16_t y = BLOCKS_TOP + BLOCKS_SIZE/2 + c * BLOCK_TO_BLOCK;
	tft.fillTriangle(
			BLOCK_POINTER_LEFT, y,
			BLOCK_POINTER_LEFT + BLOCK_POINTER_SIZE, y - BLOCK_POINTER_SIZE/2,
			BLOCK_POINTER_LEFT + BLOCK_POINTER_SIZE, y + BLOCK_POINTER_SIZE/2,
			rgbMap[BLACK+1]);
}

void drawVoltageSetting(uint16_t colour, float datasheetLevel, float adjustedLevel) {
	// draw a block of the given colour down the right hand side
	tft.drawFilledRectangle(300,30,20,160, rgbMap[colour+1]);
	// add a triangle inside to indicate the spreadsheet voltage level
	uint16_t triangleColor = ~rgbMap[colour+1];
	float vDS = datasheetLevel + offset;
	uint16_t y = 30 + 130 * vDS / VREF;
	tft.fillTriangle(301, y-5, 309, y, 301, y+5,triangleColor);

	// draw a triangle at the new setting level
	y = 30 + 130 * adjustedLevel / VREF;
	tft.fillTriangle(319,y-5,311,y,319,y+5,triangleColor);

	setColorLevels(YVolts, RYVolts);
}

void tuneY(uint8_t c, int s) {
	float adjustedLevel = setupYLevel + (s - ADC_RESOLUTION_STEPS/2) * ONE_ADC_BIT / 2;
	YVolts[c] = adjustedLevel;
	drawVoltageSetting(c, YVoltsDatasheet[c], adjustedLevel);
}

void tuneRY(uint8_t c, int s) {
	float adjustedLevel = setupRYLevel + (s - ADC_RESOLUTION_STEPS/2) * ONE_ADC_BIT / 2;
	RYVolts[c] = adjustedLevel;
	drawVoltageSetting(c, RYVoltsDatasheet[c], adjustedLevel);
}

}
