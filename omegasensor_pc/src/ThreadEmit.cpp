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

#include "ThreadEmit.h"

ThreadEmit::ThreadEmit(QObject *p) : QObject(p)
{
	// Nothing to see here.
}

void ThreadEmit::Emit()
{
	emit triggered();
}
