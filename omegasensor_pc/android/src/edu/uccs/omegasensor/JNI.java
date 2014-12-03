package edu.uccs.omegasensor;

public class JNI
{
	public static native void AddHub(SensorHubAndroid hub,
		String hwName, String mac, int rssi);
	public static native void UpdateHub(String mac, int rssi);
	public static native void LoadDrivers(String mac);
	public static native void HaveData(String mac,
		int characteristic, byte[] data);
}
