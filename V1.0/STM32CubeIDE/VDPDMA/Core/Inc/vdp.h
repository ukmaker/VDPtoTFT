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
 1 Black         0.00  0.47 / 2.20  0.42   0.47  0.00   0.00  0.00    0    0    0
 2 Medium green  0.53  0.07 / 1.84  0.06   0.20  0.13	0.79  0.26   33  200   66
 3 Light green   0.67  0.17 / 1.92  0.14   0.27  0.37   0.86  0.47   94  220  120
 4 Dark blue     0.40  0.40 / 2.12  0.34   1.00  0.33   0.33  0.93   84   85  237
 5 Light blue    0.53  0.43 / 2.16  0.38   0.93  0.49   0.46  0.99  125  118  252
 6 Dark red      0.47  0.83 / 2.52  0.74   0.30  0.83	0.32  0.30  212   82   77
 7 Cyan          0.73  0.00 / 1.78  0.00   0.70  0.26	0.92  0.96   66  235  245
 8 Medium red    0.53  0.93 / 2.62  0.84   0.27  0.99	0.33  0.33  252   85   84
 9 Light red     0.67  0.93                0.27  1.13!  0.47  0.47  255  121  120
 A Dark yellow   0.73  0.57 / 2.28  0.50   0.07  0.83	0.76  0.33  212  193   84
 B Light yellow  0.80  0.57 / 2.28  0.50   0.17  0.90	0.81  0.50  230  206  128
 C Dark green    0.47  0.13 / 1.90  0.12   0.23  0.13	0.69  0.23   33  176   59
 D Magenta       0.53  0.73 / 2.44  0.66   0.67  0.79	0.36  0.73  201   91  186
 E Gray          0.80  0.47 / 2.20  0.42   0.47  0.80	0.80  0.80  204  204  204
 F White         1.00  0.47 / 2.20  0.42   0.47  1.00	1.00  1.00  255  255  255

 */
#ifdef __cplusplus
extern "C" {
#endif
// clang-format off
#define MAP(R,G,B) (((R & 0xf8) << 8) | ((G & 0xfc) << 3) | ((B & 0xF8) >> 3))

extern uint16_t rgbMap[];
extern uint16_t YMap[];
extern uint16_t RYMap[];
extern uint8_t bitMap[];
extern float YVoltsDatasheet[];
extern float RYVoltsDatasheet[];
extern float YVolts[];
extern float RYVolts[];
extern float offset;

void initBitMap();

void swapRGBBytes();
void initColorMaps(float scaleFactor, float offset);
void setColorLevels(float yLevels[16], float ryLevels[16]);

// clang-format on
#ifdef __cplusplus
}
#endif
#endif /* SRC_VDP_H_ */
