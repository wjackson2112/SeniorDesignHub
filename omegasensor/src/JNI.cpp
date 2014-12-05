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

#include "SensorHubService.h"
#include "SensorHubAndroidService.h"
#include "SensorHubAndroid.h"

#include <QDebug>

#include <jni.h>

/**
 * Convert a Java string to a QString.
 * @param env Java environment the string belongs to.
 * @param str The Java string to convert.
 * @return A QString copy of the Java string.
 */
QString jstringToQString(JNIEnv *env, jstring str)
{
    // Get the raw string.
    const char *rawString = env->GetStringUTFChars(str, 0);

    // Convert the raw string to a QString.
    QString qstr(rawString);

    // Release the raw string.
    env->ReleaseStringUTFChars(str, rawString);

    return qstr;
}

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_edu_uccs_omegasensor_JNI_AddHub(JNIEnv *env,
	jobject obj, jobject hub, jstring hwName, jstring mac, jint rssi)
{
	Q_UNUSED(obj);

	SensorHubAndroidService *srv = dynamic_cast<SensorHubAndroidService*>(
		SensorHubService::GetSingletonPtr());
	srv->AddHub(
		QAndroidJniObject(hub),
		jstringToQString(env, hwName),
		jstringToQString(env, mac),
		rssi
	);
}

JNIEXPORT void JNICALL Java_edu_uccs_omegasensor_JNI_UpdateHub(
    JNIEnv *env, jobject obj, jstring mac, jint rssi)
{
	Q_UNUSED(obj);

	SensorHubAndroidService *srv = dynamic_cast<SensorHubAndroidService*>(
		SensorHubService::GetSingletonPtr());
	srv->UpdateHub(
		jstringToQString(env, mac),
		rssi
	);
}

JNIEXPORT void JNICALL Java_edu_uccs_omegasensor_JNI_LoadDrivers(
	JNIEnv *env, jobject obj, jstring mac)
{
	Q_UNUSED(obj);

	SensorHubAndroidService *srv = dynamic_cast<SensorHubAndroidService*>(
		SensorHubService::GetSingletonPtr());
	srv->LoadDrivers(
		jstringToQString(env, mac)
	);
}

JNIEXPORT void JNICALL Java_edu_uccs_omegasensor_JNI_HaveData(
	JNIEnv *env, jobject obj, jstring mac, int characteristic, jbyteArray data)
{
	Q_UNUSED(obj);

	QByteArray qdata;
	jsize sz = env->GetArrayLength(data);
	if(sz > 0)
	{
		jbyte *d = env->GetByteArrayElements(data, 0);
		qdata = QByteArray((char*)d, sz);
		env->ReleaseByteArrayElements(data, d, JNI_ABORT);
	}

	SensorHubAndroidService *srv = dynamic_cast<SensorHubAndroidService*>(
		SensorHubService::GetSingletonPtr());
	srv->HaveData(
		jstringToQString(env, mac),
		characteristic,
		qdata
	);
}

#ifdef __cplusplus
}
#endif
