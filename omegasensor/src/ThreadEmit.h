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

#ifndef __ThreadEmit_h__
#define __ThreadEmit_h__

#include <QtCore/QObject>

class ThreadEmit : public QObject
{
	Q_OBJECT

public:
	ThreadEmit(QObject *parent = 0);

	void Emit();

signals:
	void triggered();
};

#endif // __ThreadEmit_h__

