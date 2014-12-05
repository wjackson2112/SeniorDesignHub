/*
 * Omega Wireless Sensor Module
 * Copyright (C) 2014 Team Omega
 * Viva la Resistance
 *
 * Hardware:    Bogdan Crivin <bcrivin@uccs.edu>
 * Firmware:    William Jackson <wjackson@uccs.edu>
 * Software:    John Martin <jmarti25@uccs.edu>
 * Integration: Angela Askins <aaskins@uccs.edu>
 */

#ifndef __I2COperationChain_h__
#define __I2COperationChain_h__

#include <stdint.h>

class QByteArray;

#define CHAIN_PACKET_SIZE (20)

class I2COperationChain
{
public:
	I2COperationChain();
	~I2COperationChain();

	void Clear();

	void SetTransactionID(uint8_t id);
	void SetTransactionIDEnabled(bool enabled);

	uint8_t TransactionID() const;
	bool TransactionIDEnabled() const;

	void AddRead(uint8_t addr, uint8_t sz, bool stopEnabled = true);
	void AddWrite(uint8_t addr, const uint8_t *data, uint8_t sz,
		bool stopEnabled = true);

	void AddRegisterRead(uint8_t addr, uint8_t reg, uint8_t sz);
	void AddRegisterWrite(uint8_t addr, uint8_t reg, uint8_t val);
	void AddRegisterWriteEx(uint8_t addr, uint8_t reg, const uint8_t *data,
		uint8_t sz = 1);

	uint8_t ReadLength() const;
	QByteArray OperationData() const;

private:
	uint8_t mTransactionID;
	bool mTransactionIDEnabled;

	uint8_t mTotalRead;
	uint8_t mChainLength;
	uint8_t mOperationChain[CHAIN_PACKET_SIZE];
};

#endif // __I2COperationChain_h__
