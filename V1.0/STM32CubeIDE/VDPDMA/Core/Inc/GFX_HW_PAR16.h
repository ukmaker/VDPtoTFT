/*
 * GFX_HW_SPI.h
 *
 *  Created on: Jul 26, 2022
 *      Author: Duncan
 */

#ifndef INC_GFX_HW_PAR16_H_
#define INC_GFX_HW_PAR16_H_

#include "GFX_HW.h"

class GFX_HW_PAR16: public GFX_HW {
public:

	GFX_HW_PAR16(
			GPIO_TypeDef *dataPort,
			GPIO_TypeDef *csPort, uint16_t csPin,
			GPIO_TypeDef *dcPort, uint16_t dcPin,
			GPIO_TypeDef *wrPort, uint16_t wrPin,
			GPIO_TypeDef *rdPort, uint16_t rdPin,
			GPIO_TypeDef *rsPort, uint16_t rsPin) {

		_dataPort = dataPort;
		_csPort = csPort; _csPin = csPin;
		_dcPort = dcPort; _dcPin = dcPin;
		_wrPort = wrPort; _wrPin = wrPin;
		_rdPort = rdPort; _rdPin = rdPin;
		_rsPort = rsPort; _rsPin = rsPin;
		high(_dcPort, _dcPin);
		high(_csPort, _csPin);
		high(_wrPort, _wrPin);
		high(_rdPort, _rdPin);
		high(_rsPort, _rsPin);
	}

	virtual void reset(uint32_t delay_us) {

		high(_dcPort, _dcPin);
		high(_csPort, _csPin);
		high(_wrPort, _wrPin);
		high(_rdPort, _rdPin);
		high(_rsPort, _rsPin);

		low(_rsPort, _rsPin);
		MY_Delay1();
		high(_rsPort, _rsPin);
	}



	virtual void sendCommand(uint8_t commandByte, const uint8_t *dataBytes =
	NULL, uint8_t numDataBytes = 0) {
		MY_Delay();
		low(_csPort, _csPin);
		MY_Delay();
		low(_dcPort, _dcPin);
		MY_Delay();
		low(_wrPort, _wrPin);

		_dataPort->ODR = commandByte & 0x00ff;
		MY_Delay();

		high(_wrPort, _wrPin);
		MY_Delay();
		high(_dcPort, _dcPin);

		writeData(dataBytes, numDataBytes);

		high(_csPort, _csPin);
	}

	virtual void sendCommand16(uint16_t commandWord, const uint8_t *dataBytes = NULL, uint8_t numDataBytes = 0) {
		MY_Delay();
		low(_csPort, _csPin);
		MY_Delay();
		low(_dcPort, _dcPin);

		_dataPort->ODR = commandWord;
		low(_wrPort, _wrPin);
		MY_Delay();
		high(_wrPort, _wrPin);

		MY_Delay();
		high(_dcPort, _dcPin);

		writeData(dataBytes, numDataBytes);

		high(_csPort, _csPin);
	}

	virtual void sendCommand16(uint16_t commandWord, uint16_t data) {

		MY_Delay1();
		low(_dcPort, _dcPin);
		MY_Delay1();
		low(_csPort, _csPin);
		MY_Delay1();

		_dataPort->ODR = commandWord;
		low(_wrPort, _wrPin);
		MY_Delay1();
		high(_wrPort, _wrPin);

		MY_Delay1();
		high(_dcPort, _dcPin);
		MY_Delay1();

		_dataPort->ODR = data;
		low(_wrPort, _wrPin);
		MY_Delay1();
		high(_wrPort, _wrPin);

		high(_csPort, _csPin);
		high(_dcPort, _dcPin);

		MY_Delay();
	}

	virtual uint8_t readcommand8(uint8_t commandByte, uint8_t index = 0) {

		uint8_t result = 0;
		low(_dcPort, _dcPin);
		MY_Delay();
		low(_csPort, _csPin);
		low(_wrPort, _wrPin);

		_dataPort->ODR = commandByte & 0x00ff;
		MY_Delay();
		high(_wrPort, _wrPin);
		MY_Delay();
		high(_dcPort, _dcPin);

		_dataPort->MODER = 0x00;

		while(index > 0) {

			MY_Delay();
			low(_rdPort, _rdPin);
			MY_Delay();

			result = _dataPort->IDR & 0x00ff;
			index--;

			high(_rdPort, _rdPin);
		}

		high(_csPort, _csPin);
		MY_Delay();
		_dataPort->MODER = 0x5555;

		return result;
	}

	virtual uint16_t readcommand16(uint8_t commandByte) {

		uint16_t result = 0;
		low(_dcPort, _dcPin);
		MY_Delay();
		low(_csPort, _csPin);
		low(_wrPort, _wrPin);

		_dataPort->ODR = commandByte & 0x00ff;
		MY_Delay();
		high(_wrPort, _wrPin);
		MY_Delay();
		high(_dcPort, _dcPin);
		MY_Delay();

		_dataPort->MODER = 0x00;

		MY_Delay();
		low(_rdPort, _rdPin);
		MY_Delay();

		result = _dataPort->IDR;

		high(_rdPort, _rdPin);
		high(_csPort, _csPin);
		MY_Delay();
		_dataPort->MODER = 0x55555555;

		return result;
	}

	virtual void beginCommand() {
		MY_Delay();
		low(_dcPort, _dcPin);
		MY_Delay();
		low(_csPort, _csPin);
		MY_Delay();
	}

	virtual void writeCommand(uint8_t commandByte) {
		low(_dcPort, _dcPin);
		MY_Delay();
		write(commandByte);
		high(_dcPort, _dcPin);
		MY_Delay();
	}

	virtual void writeCommand16(uint16_t commandWord) {
		low(_wrPort, _wrPin);
		_dataPort->ODR = commandWord;
		MY_Delay();
		high(_wrPort, _wrPin);

		MY_Delay();
		high(_dcPort, _dcPin);
		MY_Delay();
	}

	virtual void writeData(const uint8_t *dataBytes = NULL, uint8_t numDataBytes = 0) {

		while(numDataBytes > 0) {
			MY_Delay();

			low(_wrPort, _wrPin);

			_dataPort->ODR = *dataBytes++ & 0x00ff;
			numDataBytes--;

			MY_Delay();
			high(_wrPort, _wrPin);
		}
	}

	virtual void write(uint8_t data) {
		writeData(&data, 1);
	}

	virtual void write16(uint16_t data) {
		_dataPort->ODR = data;
		low(_wrPort, _wrPin);
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		high(_wrPort, _wrPin);
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
		__NOP();
	}

	virtual void end() {
		high(_csPort, _csPin);
		high(_dcPort, _dcPin);
	}

	virtual inline void low(GPIO_TypeDef *port, uint16_t pin) {
		port->BRR = (uint32_t)pin;
	}

	virtual inline void high(GPIO_TypeDef *port, uint16_t pin) {
		port->BSRR = (uint32_t)pin;
	}

	virtual void MY_Delay() {
		__NOP();
	}

	virtual void MY_Delay1() {
		__NOP();
	}

protected:
	GPIO_TypeDef *_dataPort;
	GPIO_TypeDef *_csPort;
	GPIO_TypeDef *_dcPort;
	GPIO_TypeDef *_wrPort;
	GPIO_TypeDef *_rdPort;
	GPIO_TypeDef *_rsPort;
	uint16_t _csPin;
	uint16_t _dcPin;
	uint16_t _wrPin;
	uint16_t _rdPin;
	uint16_t _rsPin;
};

#endif /* INC_GFX_HW_SPI_H_ */
