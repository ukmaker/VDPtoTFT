/*
 * GFX.h
 *
 *  Created on: Jul 26, 2022
 *      Author: Duncan
 */

#ifndef INC_GFX_H_
#define INC_GFX_H_


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "GFX_HW.h"
#include "math.h"
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

class GFX {
public:

	GFX(GFX_HW &hw) : _hw(hw) {
	}

	virtual void init() =0;
	virtual void cls() =0;
	virtual void setRotation(uint8_t r) =0;
	virtual void setAddressWindow(uint16_t top, uint16_t left, uint16_t width, uint16_t height) =0;

	virtual void drawPixel(uint16_t x, uint16_t y, uint16_t color) =0;
	virtual void pushPixel(uint16_t color) =0;
	virtual void end()=0;

	/**************************************************************************/
	/*!
	   @brief    Write a line.  Bresenham's algorithm - thx wikpedia
	    @param    x0  Start point x coordinate
	    @param    y0  Start point y coordinate
	    @param    x1  End point x coordinate
	    @param    y1  End point y coordinate
	    @param    color 16-bit 5-6-5 Color to draw with
	*/
	/**************************************************************************/
	void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {

	  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	  if (steep) {
	    _swap_int16_t(x0, y0);
	    _swap_int16_t(x1, y1);
	  }

	  if (x0 > x1) {
	    _swap_int16_t(x0, x1);
	    _swap_int16_t(y0, y1);
	  }

	  int16_t dx, dy;
	  dx = x1 - x0;
	  dy = abs(y1 - y0);

	  int16_t err = dx / 2;
	  int16_t ystep;

	  if (y0 < y1) {
	    ystep = 1;
	  } else {
	    ystep = -1;
	  }

	  for (; x0 <= x1; x0++) {
	    if (steep) {
	    	drawPixel(y0, x0, color);
	    } else {
	    	drawPixel(x0, y0, color);
	    }
	    err -= dy;
	    if (err < 0) {
	      y0 += ystep;
	      err += dx;
	    }
	  }
	}

protected:
	GFX_HW &_hw;
};


#ifdef __cplusplus
}
#endif

#endif /* INC_GFX_H_ */
