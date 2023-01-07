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
	virtual void setAddressWindow(uint16_t x, uint16_t y, uint16_t width, uint16_t height) =0;

	virtual void drawPixel(uint16_t x, uint16_t y, uint16_t color) =0;
	virtual void pushPixel(uint16_t color) =0;
	virtual void end()=0;

	void drawFilledRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
		setAddressWindow(x,y,width,height);
		for(uint32_t pixel = 0; pixel < width * height; pixel++) {
			pushPixel(color);
		}
		end();
	}

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


	/**************************************************************************/
	/*!
	   @brief   Draw a triangle with no fill color
	    @param    x0  Vertex #0 x coordinate
	    @param    y0  Vertex #0 y coordinate
	    @param    x1  Vertex #1 x coordinate
	    @param    y1  Vertex #1 y coordinate
	    @param    x2  Vertex #2 x coordinate
	    @param    y2  Vertex #2 y coordinate
	    @param    color 16-bit 5-6-5 Color to draw with
	*/
	/**************************************************************************/
	void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
	                                int16_t x2, int16_t y2, uint16_t color) {
		writeLine(x0, y0, x1, y1, color);
		writeLine(x1, y1, x2, y2, color);
		writeLine(x2, y2, x0, y0, color);
	}

	/**************************************************************************/
	/*!
	   @brief     Draw a triangle with color-fill
	    @param    x0  Vertex #0 x coordinate
	    @param    y0  Vertex #0 y coordinate
	    @param    x1  Vertex #1 x coordinate
	    @param    y1  Vertex #1 y coordinate
	    @param    x2  Vertex #2 x coordinate
	    @param    y2  Vertex #2 y coordinate
	    @param    color 16-bit 5-6-5 Color to fill/draw with
	*/
	/**************************************************************************/
	void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
	                                int16_t x2, int16_t y2, uint16_t color) {

	  int16_t a, b, y, last;

	  // Sort coordinates by Y order (y2 >= y1 >= y0)
	  if (y0 > y1) {
	    _swap_int16_t(y0, y1);
	    _swap_int16_t(x0, x1);
	  }
	  if (y1 > y2) {
	    _swap_int16_t(y2, y1);
	    _swap_int16_t(x2, x1);
	  }
	  if (y0 > y1) {
	    _swap_int16_t(y0, y1);
	    _swap_int16_t(x0, x1);
	  }

	  if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
	    a = b = x0;
	    if (x1 < a)
	      a = x1;
	    else if (x1 > b)
	      b = x1;
	    if (x2 < a)
	      a = x2;
	    else if (x2 > b)
	      b = x2;
	    writeFastHLine(a, y0, b - a + 1, color);
	    return;
	  }

	  int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
	          dx12 = x2 - x1, dy12 = y2 - y1;
	  int32_t sa = 0, sb = 0;

	  // For upper part of triangle, find scanline crossings for segments
	  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	  // is included here (and second loop will be skipped, avoiding a /0
	  // error there), otherwise scanline y1 is skipped here and handled
	  // in the second loop...which also avoids a /0 error here if y0=y1
	  // (flat-topped triangle).
	  if (y1 == y2)
	    last = y1; // Include y1 scanline
	  else
	    last = y1 - 1; // Skip it

	  for (y = y0; y <= last; y++) {
	    a = x0 + sa / dy01;
	    b = x0 + sb / dy02;
	    sa += dx01;
	    sb += dx02;
	    /* longhand:
	    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
	    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
	    */
	    if (a > b)
	      _swap_int16_t(a, b);
	    writeFastHLine(a, y, b - a + 1, color);
	  }

	  // For lower part of triangle, find scanline crossings for segments
	  // 0-2 and 1-2.  This loop is skipped if y1=y2.
	  sa = (int32_t)dx12 * (y - y1);
	  sb = (int32_t)dx02 * (y - y0);
	  for (; y <= y2; y++) {
	    a = x1 + sa / dy12;
	    b = x0 + sb / dy02;
	    sa += dx12;
	    sb += dx02;
	    /* longhand:
	    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
	    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
	    */
	    if (a > b)
	      _swap_int16_t(a, b);
	    writeFastHLine(a, y, b - a + 1, color);
	  }
	}

	void writeFastHLine(uint16_t x, uint16_t y, uint16_t length, uint16_t color) {
		// use setAddrWindow and pushPixel to do this as fast as possible
		setAddressWindow(x, y, length,1);
		for(uint16_t pixel = 0; pixel < length; pixel++) {
			pushPixel(color);
		}
		end();
	}

	void writeFastVLine(uint16_t x, uint16_t y, uint16_t height, uint16_t color) {
		// use setAddrWindow and pushPixel to do this as fast as possible
		setAddressWindow(x, y, 1, height);
		for(uint16_t pixel = 0; pixel < height; pixel++) {
			pushPixel(color);
		}
		end();
	}

protected:
	GFX_HW &_hw;
};


#ifdef __cplusplus
}
#endif

#endif /* INC_GFX_H_ */
