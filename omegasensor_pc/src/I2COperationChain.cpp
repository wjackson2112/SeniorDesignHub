#include "I2COperationChain.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QByteArray>

#include <iostream>

const uint8_t I2C_READ  = 1;
const uint8_t I2C_WRITE = 0;

I2COperationChain::I2COperationChain() : mTransactionID(0),
	mTransactionIDEnabled(false), mTotalRead(0), mChainLength(0)
{
	// Zero out the operation chain buffer.
	memset(mOperationChain, 0, sizeof(mOperationChain));
}

I2COperationChain::~I2COperationChain()
{
	// Nothing to see here.
}

void I2COperationChain::Clear()
{
	mTransactionID = 0;
	mTransactionIDEnabled = false;

	mTotalRead = 0;
	mChainLength = 0;

	// Zero out the operation chain buffer.
	memset(mOperationChain, 0, sizeof(mOperationChain));
}

void I2COperationChain::SetTransactionID(uint8_t id)
{
	mTransactionID = id;
}

void I2COperationChain::SetTransactionIDEnabled(bool enabled)
{
	mTransactionIDEnabled = enabled;
}

uint8_t I2COperationChain::TransactionID() const
{
	return mTransactionID;
}

bool I2COperationChain::TransactionIDEnabled() const
{
	return mTransactionIDEnabled;
}

void I2COperationChain::AddRead(uint8_t addr, uint8_t sz, bool stopEnabled)
{
	Q_ASSERT(sz <= 0x1F);
	Q_ASSERT((mTotalRead + sz + mTransactionIDEnabled)
		<= CHAIN_PACKET_SIZE);
	Q_ASSERT((mChainLength + 2 + mTransactionIDEnabled)
		<= CHAIN_PACKET_SIZE);

	mOperationChain[mChainLength++] = (addr << 1) | I2C_READ;
	mOperationChain[mChainLength++] = (sz & 0x1F) |
		(stopEnabled ? 0x80 : 0x00);
	mTotalRead += sz;
}

void I2COperationChain::AddWrite(uint8_t addr, const uint8_t *data,
	uint8_t sz, bool stopEnabled)
{
	Q_ASSERT(sz <= 0x1F);
	Q_ASSERT((mChainLength + 2 + sz + mTransactionIDEnabled)
		<= CHAIN_PACKET_SIZE);

	mOperationChain[mChainLength++] = (addr << 1) | I2C_WRITE;
	mOperationChain[mChainLength++] = (sz & 0x1F) |
		(stopEnabled ? 0x80 : 0x00);
	memcpy(&mOperationChain[mChainLength], data, sz);
	mChainLength += sz;
}

void I2COperationChain::AddRegisterRead(uint8_t addr, uint8_t reg,
	uint8_t sz)
{
	AddWrite(addr, &reg, 1, false);
	AddRead(addr, sz, true);
}

void I2COperationChain::AddRegisterWrite(uint8_t addr, uint8_t reg,
	uint8_t val)
{
	AddRegisterWriteEx(addr, reg, &val, 1);
}

void I2COperationChain::AddRegisterWriteEx(uint8_t addr, uint8_t reg,
	const uint8_t *data, uint8_t sz)
{
	uint8_t buffer[sz + 1];
	buffer[0] = reg;
	memcpy(&buffer[1], data, sz);
	AddWrite(addr, buffer, sz + 1);
}

uint8_t I2COperationChain::ReadLength() const
{
	if(mTransactionIDEnabled)
		return mTotalRead + 1;
	else
		return mTotalRead;
}

QByteArray I2COperationChain::OperationData() const
{
	if(mTransactionIDEnabled)
	{
		Q_ASSERT(mChainLength < CHAIN_PACKET_SIZE);

		uint8_t buffer[CHAIN_PACKET_SIZE];
		buffer[0] = mTransactionID;
		memcpy(&buffer[1], mOperationChain, mChainLength);

		QStringList hexDump;

		for(int i = 0; i < (mChainLength + 1); i++)
		{
			hexDump.append(QString("0x%1").arg(buffer[i], 2, 16, QLatin1Char('0')));
		}

		//std::cout << "OP: " << hexDump.join(" ").toUtf8().constData() << std::endl;

		return QByteArray((char*)buffer, mChainLength + 1);
	}
	else
	{
		Q_ASSERT(mChainLength <= CHAIN_PACKET_SIZE);

		return QByteArray((char*)mOperationChain, mChainLength);
	}
}
