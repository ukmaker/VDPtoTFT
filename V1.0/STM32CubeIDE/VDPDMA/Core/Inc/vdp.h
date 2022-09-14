/*
 * vdp.h
 *
 *  Created on: Jul 27, 2022
 *      Author: Duncan
 */

#ifndef SRC_VDP_H_
#define SRC_VDP_H_

#include "main.h"
/**
 Colour names
 **/
#define TRANSPARENT 0
#define BLACK 1
#define MEDIUM_GREEN 2
#define LIGHT_GREEN 3
#define DARK_BLUE 4
#define LIGHT_BLUE 5
#define DARK_RED 6
#define CYAN 7
#define MEDIUM_RED 8
#define LIGHT_RED 9
#define DARK_YELLOW 10
#define LIGHT_YELLOW 11
#define DARK_GREEN 12
#define MAGENTA 13
#define GREY 14
#define WHITE 15

/**
 * Taken from CBMeeks repo: https://github.com/cbmeeks/TMS9918/blob/master/tms9918a.txt

 Colour           Y     R-Y / R-Y   DELTA   B-Y   R     G     B     R    G    B
 0 Transparent
 1 Black         0.00  0.47 / 2.20  0.42   0.47  0.00   0.00  0.00    0    0    0   20 28 1.65 / 2.06
 2 Medium green  0.53  0.07 / 1.84  0.06   0.20  0.13	0.79  0.26   33  200   66
 3 Light green   0.67  0.17 / 1.92  0.14   0.27  0.37   0.86  0.47   94  220  120
 4 Dark blue     0.40  0.40 / 2.12  0.34   1.00  0.33   0.33  0.93   84   85  237
 5 Light blue    0.53  0.43 / 2.16  0.38   0.93  0.49   0.46  0.99  125  118  252  2a 28 2.17 / 2.06
 6 Dark red      0.47  0.83 / 2.52  0.74   0.30  0.83	0.32  0.30  212   82   77   28 2f
 7 Cyan          0.73  0.00 / 1.78  0.00   0.70  0.26	0.92  0.96   66  235  245
 8 Medium red    0.53  0.93 / 2.62  0.84   0.27  0.99	0.33  0.33  252   85   84  2a 31
 9 Light red     0.67  0.93                0.27  1.13!  0.47  0.47  255  121  120
 A Dark yellow   0.73  0.57 / 2.28  0.50   0.07  0.83	0.76  0.33  212  193   84
 B Light yellow  0.80  0.57 / 2.28  0.50   0.17  0.90	0.81  0.50  230  206  128
 C Dark green    0.47  0.13 / 1.90  0.12   0.23  0.13	0.69  0.23   33  176   59
 D Magenta       0.53  0.73 / 2.44  0.66   0.67  0.79	0.36  0.73  201   91  186
 E Gray          0.80  0.47 / 2.20  0.42   0.47  0.80	0.80  0.80  204  204  204
 F White         1.00  0.47 / 2.20  0.42   0.47  1.00	1.00  1.00  255  255  255  28 32 2.06 / 2.58


11000101100100
00000001000000
 */

// clang-format off
#define MAP(R,G,B) (((R & 0xf8) << 8) | ((G & 0xfc) << 3) | ((B & 0xF8) >> 3))

uint16_t rgbMap[17] = {
		MAP(0, 0, 0),
		MAP(0, 0, 0),
		MAP(0, 0, 0),
		MAP(33, 200,66),
		MAP(94, 220, 120),
		MAP(84, 85, 237),
		MAP(125, 118, 252),
		MAP(212,82, 77),
		MAP(66, 235, 245),
		MAP(252, 85, 84),
		MAP(255, 121, 120),
		MAP(212, 212, 84),
		MAP(255, 255, 148),
		MAP(33, 176, 59),
		MAP(201, 91, 186),
		MAP(204, 204, 204),
		MAP(255, 255, 255) };


uint16_t YMap[ADC_RESOLUTION_STEPS];
uint16_t RYMap[ADC_RESOLUTION_STEPS];

uint8_t bitMap[256];

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
void setColorLevels(float yLevels[16], float ryLevels[16]);
void initColorMaps(float scaleFactor, float offset) {

	// The Y voltages per colour index
	// Index 0 is "transparent"
	/*
	float Y[16] = { 0, 0, 0.53, 0.67, 0.4, 0.53, 0.47, 0.73, 0.53, 0.67, 0.73,
			0.8, 0.47, 0.53, 0.8, 1 };


	float RY[16] = { 0, 0.47, 0.07, 0.17, 0.4, 0.43, 0.83, 0, 0.93, 0.93, 0.57,
			0.57, 0.13, 0.73, 0.47, 0.47 };
			*/

	float Y[16] = { 0, 0, 0.5, 0.6, 0.36, 0.5, 0.44, 0.66, 0.5, 0.62, 0.68,
			0.78, 0.42, 0.48, 0.72, 0.92 };

	float RY[16] = { 0, 0.42, 0.06, 0.14, 0.34, 0.38, 0.76, 0, 0.84, 0.84, 0.5,
			0.5, 0.12, 0.66, 0.42, 0.42 };

	// multiply by the initial scale factor and add the offset
	for(int i=0; i< 16; i++) {
		Y[i] = (Y[i] * scaleFactor) + offset;
		RY[i] = (RY[i] * scaleFactor) + offset;
	}

	setColorLevels(Y, RY);
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
		for (int j = 0; j < ADC_RESOLUTION_STEPS-1; j++) {
			if (voltages[j] <= v && voltages[j + 1] > v) {
				Ymap1[j] |= (1 << i);
			}
		}
		if (voltages[ADC_RESOLUTION_STEPS-1] <= v) {
			Ymap1[ADC_RESOLUTION_STEPS-1] |= (1 << i);
		}
		v = ryLevels[i];
		for (int j = 0; j < ADC_RESOLUTION_STEPS-1; j++) {
			if (voltages[j] <= v && voltages[j + 1] > v) {
				RYmap1[j] |= (1 << i);
			}
		}
		if (voltages[ADC_RESOLUTION_STEPS-1] <= v) {
			RYmap1[ADC_RESOLUTION_STEPS-1] |= (1 << i);
		}
	}

	// now propagate the map to each neighbouring entry
	// to allow for noise and inaccuracy

	for(uint16_t i=0; i<ADC_RESOLUTION_STEPS-1; i++) {

		YMap[i] = Ymap1[i];
		RYMap[i] = RYmap1[i];

		if(i>0) {
			YMap[i] |= Ymap1[i-1];
			RYMap[i] |= RYmap1[i-1];
		}
		if(i<ADC_RESOLUTION_STEPS-1) {
			YMap[i] |= Ymap1[i+1];
			RYMap[i] |= RYmap1[i+1];
		}
	}
}

// clang-format on

#endif /* SRC_VDP_H_ */
