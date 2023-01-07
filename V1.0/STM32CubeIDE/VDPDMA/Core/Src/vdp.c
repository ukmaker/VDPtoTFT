/*
 * vdp.c
 *
 *  Created on: Sep 13, 2022
 *      Author: Duncan
 */

#include "vdp.h"
uint16_t rgbMap[17] = { MAP(0, 0, 0), MAP(0, 0, 0), MAP(0, 0, 0), MAP(33, 200,
		66), MAP(94, 220, 120), MAP(84, 85, 237), MAP(125, 118, 252), MAP(212,
		82, 77), MAP(66, 235, 245), MAP(252, 85, 84), MAP(255, 121, 120), MAP(
		212, 212, 84), MAP(255, 255, 148), MAP(33, 176, 59), MAP(201, 91, 186),
		MAP(204, 204, 204), MAP(255, 255, 255) };

uint16_t YMap[ADC_RESOLUTION_STEPS];
uint16_t RYMap[ADC_RESOLUTION_STEPS];

uint8_t bitMap[256];
float offset = 0.0;

// The Y voltages per colour index
// Index 0 is "transparent"
/*
 *
 * These are the datasheet values
 * */
float YVoltsDatasheet[16] = { 0, 0, 0.53, 0.67, 0.4, 0.53, 0.47, 0.73, 0.53,
		0.67, 0.73, 0.8, 0.47, 0.53, 0.8, 1 };
// these are the values measured on my board
float YVolts[16] = { 0, 0, 0.5, 0.6, 0.36, 0.5, 0.44, 0.66, 0.5, 0.62, 0.68,
		0.78, 0.42, 0.48, 0.72, 0.92 };

float RYVoltsDatasheet[16] = { 0, 0.47, 0.07, 0.17, 0.4, 0.43, 0.83, 0, 0.93,
		0.93, 0.57, 0.57, 0.13, 0.73, 0.47, 0.47 };

float RYVolts[16] = { 0, 0.42, 0.06, 0.14, 0.34, 0.38, 0.76, 0, 0.84, 0.84, 0.5,
		0.5, 0.12, 0.66, 0.42, 0.42 };

// bitMap is a quick lookup table to return the index of the most-signifcant set bit
// 00000000 -> 0
// 00000001 -> 1
// 00000010 -> 2
// 00000011 -> 2
// etc
void initBitMap() {
	for (int i = 0; i < 8; i++) {
		int bit = 1 << i;
		for (int j = 0; j < bit; j++) {
			bitMap[bit + j] = i + 1;
		}
	}
}

void swapRGBBytes() {
	for (int i = 0; i < 16; i++) {
		uint16_t rgb = rgbMap[i];
		rgbMap[i] = (rgb >> 8) | ((rgb & 0xff) << 8);
	}
}

void initColorMaps(float scaleFactor, float offsetVoltage) {
	offset = offsetVoltage;
	// multiply by the initial scale factor and add the offset
	for (int i = 0; i < 16; i++) {
		YVolts[i] = (YVolts[i] * scaleFactor) + offset;
		RYVolts[i] = (RYVolts[i] * scaleFactor) + offset;
	}

	setColorLevels(YVolts, RYVolts);
}

void setColorLevels(float yLevels[16], float ryLevels[16]) {
	float v = 0;
	float oneBit = VREF / ADC_RESOLUTION_STEPS;

	uint16_t Ymap1[ADC_RESOLUTION_STEPS];
	uint16_t RYmap1[ADC_RESOLUTION_STEPS];

	float voltages[ADC_RESOLUTION_STEPS];

	// voltage to adc reading map
	for (int i = 0; i < ADC_RESOLUTION_STEPS; i++) {
		voltages[i] = v;
		v += oneBit;
	}

	// Clear the color maps
	for (int j = 0; j < ADC_RESOLUTION_STEPS; j++) {
		YMap[j] = 0;
		RYMap[j] = 0;
		Ymap1[j] = 0;
		RYmap1[j] = 0;
	}

	// Create the voltage to color name bitmaps
	for (int i = 0; i < 16; i++) {

		v = yLevels[i];
		for (int j = 0; j < ADC_RESOLUTION_STEPS - 1; j++) {
			if (voltages[j] <= v && voltages[j + 1] > v) {
				Ymap1[j] |= (1 << i);
			}
		}
		if (voltages[ADC_RESOLUTION_STEPS - 1] <= v) {
			Ymap1[ADC_RESOLUTION_STEPS - 1] |= (1 << i);
		}
		v = ryLevels[i];
		for (int j = 0; j < ADC_RESOLUTION_STEPS - 1; j++) {
			if (voltages[j] <= v && voltages[j + 1] > v) {
				RYmap1[j] |= (1 << i);
			}
		}
		if (voltages[ADC_RESOLUTION_STEPS - 1] <= v) {
			RYmap1[ADC_RESOLUTION_STEPS - 1] |= (1 << i);
		}
	}

	// now propagate the map to each neighbouring entry
	// to allow for noise and inaccuracy

	for (uint16_t i = 0; i < ADC_RESOLUTION_STEPS - 1; i++) {

		YMap[i] = Ymap1[i];
		RYMap[i] = RYmap1[i];

		if (i > 0) {
			YMap[i] |= Ymap1[i - 1];
			RYMap[i] |= RYmap1[i - 1];
		}
		if (i < ADC_RESOLUTION_STEPS - 1) {
			YMap[i] |= Ymap1[i + 1];
			RYMap[i] |= RYmap1[i + 1];
		}
	}
}
