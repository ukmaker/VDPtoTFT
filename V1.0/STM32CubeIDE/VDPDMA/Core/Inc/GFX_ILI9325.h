/*
 * GFX_ILI9325.h
 *
 *  Created on: Jul 26, 2022
 *      Author: Duncan
 */

#ifndef INC_GFX_ILI9325_H_
#define INC_GFX_ILI9325_H_

#include "GFX.h"

// Register names from Peter Barrett's Microtouch code
#define ILI932X_START_OSC 0x00
#define ILI932X_DRIV_OUT_CTRL 0x01
#define ILI932X_DRIV_WAV_CTRL 0x02
#define ILI932X_ENTRY_MOD 0x03
#define ILI932X_RESIZE_CTRL 0x04
#define ILI932X_DISP_CTRL1 0x07
#define ILI932X_DISP_CTRL2 0x08
#define ILI932X_DISP_CTRL3 0x09
#define ILI932X_DISP_CTRL4 0x0A
#define ILI932X_RGB_DISP_IF_CTRL1 0x0C
#define ILI932X_FRM_MARKER_POS 0x0D
#define ILI932X_RGB_DISP_IF_CTRL2 0x0F
#define ILI932X_POW_CTRL1 0x10
#define ILI932X_POW_CTRL2 0x11
#define ILI932X_POW_CTRL3 0x12
#define ILI932X_POW_CTRL4 0x13
#define ILI932X_GRAM_HOR_AD 0x20
#define ILI932X_GRAM_VER_AD 0x21
#define ILI932X_RW_GRAM 0x22
#define ILI932X_POW_CTRL7 0x29
#define ILI932X_FRM_RATE_COL_CTRL 0x2B
#define ILI932X_GAMMA_CTRL1 0x30
#define ILI932X_GAMMA_CTRL2 0x31
#define ILI932X_GAMMA_CTRL3 0x32
#define ILI932X_GAMMA_CTRL4 0x35
#define ILI932X_GAMMA_CTRL5 0x36
#define ILI932X_GAMMA_CTRL6 0x37
#define ILI932X_GAMMA_CTRL7 0x38
#define ILI932X_GAMMA_CTRL8 0x39
#define ILI932X_GAMMA_CTRL9 0x3C
#define ILI932X_GAMMA_CTRL10 0x3D
#define ILI932X_HOR_START_AD 0x50
#define ILI932X_HOR_END_AD 0x51
#define ILI932X_VER_START_AD 0x52
#define ILI932X_VER_END_AD 0x53
#define ILI932X_GATE_SCAN_CTRL1 0x60
#define ILI932X_GATE_SCAN_CTRL2 0x61
#define ILI932X_GATE_SCAN_CTRL3 0x6A
#define ILI932X_PART_IMG1_DISP_POS 0x80
#define ILI932X_PART_IMG1_START_AD 0x81
#define ILI932X_PART_IMG1_END_AD 0x82
#define ILI932X_PART_IMG2_DISP_POS 0x83
#define ILI932X_PART_IMG2_START_AD 0x84
#define ILI932X_PART_IMG2_END_AD 0x85
#define ILI932X_PANEL_IF_CTRL1 0x90
#define ILI932X_PANEL_IF_CTRL2 0x92
#define ILI932X_PANEL_IF_CTRL3 0x93
#define ILI932X_PANEL_IF_CTRL4 0x95
#define ILI932X_PANEL_IF_CTRL5 0x97
#define ILI932X_PANEL_IF_CTRL6 0x98

// Color definitions
#define ILI9325_BLACK 0x0000       ///<   0,   0,   0
#define ILI9325_NAVY 0x000F        ///<   0,   0, 123
#define ILI9325_DARKGREEN 0x03E0   ///<   0, 125,   0
#define ILI9325_DARKCYAN 0x03EF    ///<   0, 125, 123
#define ILI9325_MAROON 0x7800      ///< 123,   0,   0
#define ILI9325_PURPLE 0x780F      ///< 123,   0, 123
#define ILI9325_OLIVE 0x7BE0       ///< 123, 125,   0
#define ILI9325_LIGHTGREY 0xC618   ///< 198, 195, 198
#define ILI9325_DARKGREY 0x7BEF    ///< 123, 125, 123
#define ILI9325_BLUE 0x001F        ///<   0,   0, 255
#define ILI9325_GREEN 0x07E0       ///<   0, 255,   0
#define ILI9325_CYAN 0x07FF        ///<   0, 255, 255
#define ILI9325_RED 0xF800         ///< 255,   0,   0
#define ILI9325_MAGENTA 0xF81F     ///< 255,   0, 255
#define ILI9325_YELLOW 0xFFE0      ///< 255, 255,   0
#define ILI9325_WHITE 0xFFFF       ///< 255, 255, 255
#define ILI9325_ORANGE 0xFD20      ///< 255, 165,   0
#define ILI9325_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define ILI9325_PINK 0xFC18        ///< 255, 130, 198

#define ILI9325_TFTWIDTH 320
#define ILI9325_TFTHEIGHT 240

#define MADCTL_MY 0x80  ///< Bottom to top
#define MADCTL_MX 0x40  ///< Right to left
#define MADCTL_MV 0x20  ///< Reverse Mode
#define MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define MADCTL_MH 0x04  ///< LCD refresh right to left

#define TFTLCD_DELAY 0xFF
// clang-format off
static const uint16_t ILI932x_regValues[] = {
ILI932X_START_OSC, 0x0001, // Start oscillator
		TFTLCD_DELAY, 50, // 50 millisecond delay
		ILI932X_DRIV_OUT_CTRL, 0x0100,
		ILI932X_DRIV_WAV_CTRL, 0x0700,
		ILI932X_ENTRY_MOD, 0x1030,
		ILI932X_RESIZE_CTRL, 0x0000,
		ILI932X_DISP_CTRL2, 0x0202,
		ILI932X_DISP_CTRL3, 0x0000,
		ILI932X_DISP_CTRL4, 0x0000,
		ILI932X_RGB_DISP_IF_CTRL1, 0x01,
		ILI932X_FRM_MARKER_POS, 0x0,
		ILI932X_RGB_DISP_IF_CTRL2, 0x0,
		ILI932X_POW_CTRL1, 0x0000,
		ILI932X_POW_CTRL2, 0x0007,
		ILI932X_POW_CTRL3, 0x0000,
		ILI932X_POW_CTRL4, 0x0000,
		TFTLCD_DELAY, 200,
		ILI932X_POW_CTRL1, 0x1690,
		ILI932X_POW_CTRL2, 0x0227,
		TFTLCD_DELAY, 50,
		ILI932X_POW_CTRL3, 0x001A,
		TFTLCD_DELAY, 50,
		ILI932X_POW_CTRL4, 0x1800,
		ILI932X_POW_CTRL7, 0x002A,
		TFTLCD_DELAY, 50,
		ILI932X_GAMMA_CTRL1, 0x0000,
		ILI932X_GAMMA_CTRL2, 0x0000,
		ILI932X_GAMMA_CTRL3, 0x0000,
		ILI932X_GAMMA_CTRL4, 0x0206,
		ILI932X_GAMMA_CTRL5, 0x0808,
		ILI932X_GAMMA_CTRL6, 0x0007,
		ILI932X_GAMMA_CTRL7, 0x0201,
		ILI932X_GAMMA_CTRL8, 0x0000,
		ILI932X_GAMMA_CTRL9, 0x0000,
		ILI932X_GAMMA_CTRL10, 0x0000,
		ILI932X_GRAM_HOR_AD, 0x0000,
		ILI932X_GRAM_VER_AD, 0x0000,
		ILI932X_HOR_START_AD, 0x0000,
		ILI932X_HOR_END_AD, 0x00EF,
		ILI932X_VER_START_AD, 0X0000,
		ILI932X_VER_END_AD, 0x013F,
		ILI932X_GATE_SCAN_CTRL1, 0xA700, // Driver Output Control (R60h)
		ILI932X_GATE_SCAN_CTRL2, 0x0003, // Driver Output Control (R61h)
		ILI932X_GATE_SCAN_CTRL3, 0x0000, // Driver Output Control (R62h)
		ILI932X_PANEL_IF_CTRL1, 0X0010, // Panel Interface Control 1 (R90h)
		ILI932X_PANEL_IF_CTRL2, 0X0000,
		ILI932X_PANEL_IF_CTRL3, 0X0003,
		ILI932X_PANEL_IF_CTRL4, 0X1100,
		ILI932X_PANEL_IF_CTRL5, 0X0000,
		ILI932X_PANEL_IF_CTRL6, 0X0000,
		ILI932X_DISP_CTRL1, 0x0133, // Main screen turn on
		};
// clang-format on

class GFX_ILI9325: public GFX {
public:

	GFX_ILI9325(GFX_HW &hw) :
			GFX(hw) {
	}

	virtual void init() {

		uint16_t a, d, i;
		i = 0;

		_hw.reset(300);
		HAL_Delay(30);

		while (i < sizeof(ILI932x_regValues) / sizeof(uint16_t)) {
			a = ILI932x_regValues[i++];
			d = ILI932x_regValues[i++];
			if (a == TFTLCD_DELAY)
				HAL_Delay(d * 4);
			else
				_hw.sendCommand16(a, d);
			HAL_Delay(10);
		}

		//uint16_t r = _hw.readcommand16(0);

		_width = ILI9325_TFTWIDTH;
		_height = ILI9325_TFTHEIGHT;
	}

	virtual void cls() {

	}

	virtual void setRotation(uint8_t m) {
		_rotation = m % 4; // can't be higher than 3

		uint16_t cmd = 0;

		switch (_rotation) {
		case 0:
			cmd = 0x1030;
			_width = ILI9325_TFTWIDTH;
			_height = ILI9325_TFTHEIGHT;
			break;
		case 1:
			cmd = 0x1028;
			_width = ILI9325_TFTHEIGHT;
			_height = ILI9325_TFTWIDTH;
			break;
		case 2:
			cmd = 0x1000;
			_width = ILI9325_TFTWIDTH;
			_height = ILI9325_TFTHEIGHT;
			break;
		case 3:
			cmd = 0x1018;
			_width = ILI9325_TFTHEIGHT;
			_height = ILI9325_TFTWIDTH;
			break;
		}

		_hw.sendCommand16(ILI932X_ENTRY_MOD, cmd);
		HAL_Delay(10);
	}

	virtual void setAddressWindow(uint16_t x, uint16_t y, uint16_t width,
			uint16_t height) {

		uint16_t t, l, w, h;

		switch (_rotation) {
		case 0: // (0,0) is top left
			t = y;
			l = x;
			w = width;
			h = height;
			break;
		case 1: // (0,0) is top right - portrait
			t = _height - (x + width);
			l = _width - (y + height);
			w = height;
			h = width;
			break;
		case 2:
			t = _height - (y + height);
			l = _width - (x + width);
			w = width;
			h = height;
			break;
		case 3:
			t = _height - (x + width);
			l = y;
			w = height;
			h = width;
			break;
		default:
			break;

		}

		_hw.sendCommand16(ILI932X_HOR_START_AD, l);
		_hw.sendCommand16(ILI932X_HOR_END_AD, l + w - 1);

		_hw.sendCommand16(ILI932X_VER_START_AD, t);
		_hw.sendCommand16(ILI932X_VER_END_AD, _height - x -1);

		_hw.sendCommand16(ILI932X_GRAM_HOR_AD, l);
		_hw.sendCommand16(ILI932X_GRAM_VER_AD, _height - x);

		_hw.beginCommand();
		_hw.writeCommand16(ILI932X_RW_GRAM);
	}

	virtual void drawPixel(uint16_t x, uint16_t y, uint16_t color) {
		setAddressWindow(x, y, 1, 1);
		_hw.beginCommand();
		_hw.writeCommand16(ILI932X_RW_GRAM);
		pushPixel(color);
		_hw.end();
	}

	virtual inline void pushPixel(uint16_t color) {
		_hw.write16(color);
	}

	virtual void end() {
		_hw.end();
		_hw.readcommand16(ILI932X_GRAM_HOR_AD);
	}

protected:
	uint16_t _width;
	uint16_t _height;
	uint8_t _rotation = 0;
};

#endif /* INC_GFX_ILI9325_H_ */
