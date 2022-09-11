/*
 * GFX_HW.h
 *
 *  Created on: Jul 26, 2022
 *      Author: Duncan
 */

#ifndef INC_GFX_HW_H_
#define INC_GFX_HW_H_

#include "main.h"

class GFX_HW {
public:

	virtual void reset(uint32_t delay_us)=0;

	virtual void sendCommand(uint8_t commandByte, const uint8_t *dataBytes =
	NULL, uint8_t numDataBytes = 0)=0;
	virtual void sendCommand16(uint16_t commandWord, const uint8_t *dataBytes =
	NULL, uint8_t numDataBytes = 0)=0;
	virtual void sendCommand16(uint16_t commandWord, uint16_t data)=0;
	virtual uint8_t readcommand8(uint8_t commandByte, uint8_t index = 0)=0;
	virtual uint16_t readcommand16(uint8_t commandByte)=0;

	virtual void beginCommand()=0;
	virtual void writeCommand(uint8_t commandByte)=0;
	virtual void writeCommand16(uint16_t commandWord)=0;
	virtual void writeData(const uint8_t *dataBytes = NULL, uint8_t numDataBytes = 0)=0;
	virtual void write(uint8_t data) = 0;
	virtual void write16(uint16_t data) = 0;
	virtual void end()=0;
};

#endif /* INC_GFX_HW_H_ */
