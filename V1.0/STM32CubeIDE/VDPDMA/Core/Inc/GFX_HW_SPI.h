/*
 * GFX_HW_SPI.h
 *
 *  Created on: Jul 26, 2022
 *      Author: Duncan
 */

#ifndef INC_GFX_HW_SPI_H_
#define INC_GFX_HW_SPI_H_

#include "GFX_HW.h"

class GFX_HW_SPI: public GFX_HW {
public:

	GFX_HW_SPI(SPI_HandleTypeDef *spi, GPIO_TypeDef *csport, uint16_t cspin,
			GPIO_TypeDef *dcport, uint16_t dcpin, GPIO_TypeDef *rsport, uint16_t rspin) {
		_spi = spi;
		_csport = csport, _cspin = cspin;
		_dcport = dcport, _dcpin = dcpin;
		_rsport = rsport, _rspin = rspin;
	}

	virtual void reset(uint32_t delay_us) {

		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(_csport, _cspin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(_rsport, _rspin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(_rsport, _rspin, GPIO_PIN_RESET);
		MY_Delay(delay_us);
		HAL_GPIO_WritePin(_rsport, _rspin, GPIO_PIN_SET);
	}



	virtual void sendCommand(uint8_t commandByte, const uint8_t *dataBytes =
	NULL, uint8_t numDataBytes = 0) {
		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(_csport, _cspin, GPIO_PIN_RESET);
		MY_Delay(1);

		HAL_SPI_Transmit(_spi, &commandByte, 1, HAL_MAX_DELAY);
		while (SPI1->SR & SPI_SR_BSY) {
			; // wait for SPI to finish
		}

		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_SET);
		MY_Delay(1);

		if (numDataBytes > 0) {
			HAL_SPI_Transmit(_spi, (uint8_t*)dataBytes, numDataBytes, HAL_MAX_DELAY);
		}
		while (SPI1->SR & SPI_SR_BSY) {
			; // wait for SPI to finish
		}

		HAL_GPIO_WritePin(_csport, _cspin, GPIO_PIN_SET);

	}

	virtual void sendCommand16(uint16_t commandWord, const uint8_t *dataBytes =
	NULL, uint8_t numDataBytes = 0) {
		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(_csport, _cspin, GPIO_PIN_RESET);
		MY_Delay(1);
		write16(commandWord);
		while (SPI1->SR & SPI_SR_BSY) {
			; // wait for SPI to finish
		}


		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_SET);
		MY_Delay(1);

		if (numDataBytes > 0) {
			HAL_SPI_Transmit(_spi, (uint8_t*)dataBytes, numDataBytes, HAL_MAX_DELAY);
		}
		while (SPI1->SR & SPI_SR_BSY) {
			; // wait for SPI to finish
		}

		HAL_GPIO_WritePin(_csport, _cspin, GPIO_PIN_SET);

	}

	virtual void sendCommand16(uint16_t commandWord, uint16_t data) {
		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(_csport, _cspin, GPIO_PIN_RESET);
		MY_Delay(1);
		write16(commandWord);
		while (SPI1->SR & SPI_SR_BSY) {
			; // wait for SPI to finish
		}


		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_SET);
		MY_Delay(1);

		HAL_SPI_Transmit(_spi, (uint8_t*)data, 2, HAL_MAX_DELAY);

		while (SPI1->SR & SPI_SR_BSY) {
			; // wait for SPI to finish
		}

		HAL_GPIO_WritePin(_csport, _cspin, GPIO_PIN_SET);

	}

	virtual uint8_t readcommand8(uint8_t commandByte, uint8_t index = 0) {
		uint8_t result = 0;

		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(_csport, _cspin, GPIO_PIN_RESET);
		MY_Delay(1);

		HAL_SPI_Transmit(_spi, &commandByte, 1, HAL_MAX_DELAY);
		while (SPI1->SR & SPI_SR_BSY) {
			; // wait for SPI to finish
		}

		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_SET);
		do {
			HAL_SPI_Receive(_spi, &result, 1, HAL_MAX_DELAY);
		} while (index--);
		while (SPI1->SR & SPI_SR_BSY) {
			; // wait for SPI to finish
		}

		HAL_GPIO_WritePin(_csport, _cspin, GPIO_PIN_SET);

		return result;
	}

	virtual uint16_t readcommand16(uint8_t commandByte) {
		uint16_t result = 0;
		uint8_t bite;

		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(_csport, _cspin, GPIO_PIN_RESET);
		MY_Delay(1);

		HAL_SPI_Transmit(_spi, &commandByte, 1, HAL_MAX_DELAY);
		while (SPI1->SR & SPI_SR_BSY) {
			; // wait for SPI to finish
		}

		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_SET);
		HAL_SPI_Receive(_spi, &bite, 1, HAL_MAX_DELAY);
		result = bite << 8;
		HAL_SPI_Receive(_spi, &bite, 1, HAL_MAX_DELAY);
		result |= bite;
		while (SPI1->SR & SPI_SR_BSY) {
			; // wait for SPI to finish
		}

		HAL_GPIO_WritePin(_csport, _cspin, GPIO_PIN_SET);

		return result;
	}

	virtual void beginCommand() {
		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(_csport, _cspin, GPIO_PIN_SET);
		MY_Delay(1);

		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(_csport, _cspin, GPIO_PIN_RESET);
		MY_Delay(1);
	}

	virtual void writeCommand(uint8_t commandByte) {
		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_RESET);
		MY_Delay(1);
		HAL_SPI_Transmit(_spi, &commandByte, 1, HAL_MAX_DELAY);
		while (SPI1->SR & SPI_SR_BSY) {
			; // wait for SPI to finish
		}

		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_SET);

	}

	virtual void writeCommand16(uint16_t commandWord) {
		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_RESET);
		MY_Delay(1);

		write16(commandWord);
		while (SPI1->SR & SPI_SR_BSY) {
			; // wait for SPI to finish
		}


		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_SET);

	}

	virtual void writeData(const uint8_t *dataBytes =
	NULL, uint8_t numDataBytes = 0) {

		uint8_t *data = (uint8_t*)dataBytes;
		if (numDataBytes > 0) {
			HAL_SPI_Transmit(_spi, data, numDataBytes, HAL_MAX_DELAY);
		}
	}

	virtual void write(uint8_t data) {
		writeData(&data, 1);
	}

	virtual void write16(uint16_t data) {
		uint8_t d = (data & 0xff00) >> 8;
		writeData(&d, 1);
		d = data & 0xff;
		writeData(&d, 1);
	}

	virtual void end() {
		while (SPI1->SR & SPI_SR_BSY) {
			; // wait for SPI to finish
		}
		HAL_GPIO_WritePin(_csport, _cspin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(_dcport, _dcpin, GPIO_PIN_SET);
		MY_Delay(1);
	}

	virtual void MY_Delay(uint32_t d) {
		for(uint32_t i=0; i< 192*d; i++) {
			__NOP();
		}
	}

protected:
	SPI_HandleTypeDef *_spi;
	GPIO_TypeDef *_csport;
	GPIO_TypeDef *_dcport;
	GPIO_TypeDef *_rsport;
	uint16_t _cspin;
	uint16_t _dcpin;
	uint16_t _rspin;
};

#endif /* INC_GFX_HW_SPI_H_ */
