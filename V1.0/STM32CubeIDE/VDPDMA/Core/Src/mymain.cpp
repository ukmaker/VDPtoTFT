/*
 * mymain.cpp
 *
 *  Created on: Jul 19, 2022
 *      Author: Duncan
 */

#include "main.h"
#include "spi.h"
#include "adc.h"

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

extern "C" {
	void mysetup();

void vsync() {
	tft.end();
	tft.setAddressWindow((320 - 256) / 2, (240 - 192) / 2, 256, 192);
}

void mysetup() {
	tft.init();
	tft.setRotation(3);
	HAL_Delay(10);
	tft.setAddressWindow(0, 0, 320, 240);
	HAL_Delay(10);
	for (int i = 0; i < (320 * 240); i++) {

		tft.pushPixel(0xffff);
	}
}
#define MAP(R,G,B) (((R & 0xf8) << 8) | ((G & 0xfc) << 3) | ((B & 0xF8) >> 3))
void setLeftWindow() {
	uint16_t rgbMap[17] = { MAP(0, 0, 0), MAP(0, 0, 0), MAP(0, 0, 0), MAP(33, 200,
			66), MAP(94, 220, 120), MAP(84, 85, 237), MAP(125, 118, 252), MAP(212,
			82, 77), MAP(66, 235, 245), MAP(252, 85, 84), MAP(255, 121, 120), MAP(
			212, 193, 84), MAP(230, 206, 128), MAP(33, 176, 59), MAP(201, 91, 186),
			MAP(204, 204, 204), MAP(255, 255, 255) };

	tft.setAddressWindow(10,30,10,160);


	// write the colors as blocks down the left
	for (int i = 0; i < 15; i++) {
		uint16_t color = rgbMap[i + 1];
		for (int y = 0; y < 8; y++) {
			// fill the gaps
			tft.pushPixel(0xffff);
			tft.pushPixel(0xffff);
			for (int x = 0; x < 8; x++) {
				tft.pushPixel(color);
			}

		}

		for (int z = 0; z < 20; z++) {
			tft.pushPixel(0xffff);
		}
	}

	vsync();
}

void startDMA() {

}

void pushpixel(uint16_t c) {
	tft.pushPixel(c);
}

}
