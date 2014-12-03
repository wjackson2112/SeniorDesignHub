package edu.uccs.omegasensor;

import java.util.UUID;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.util.Log;

public class SensorHubAndroid
{
    /// Debug tag.
    private final static String TAG = "SensorHubAndroid";

	private BluetoothGatt mGatt = null;
	private BluetoothDevice mDevice = null;
	private Context mContext = null;

	SensorHubAndroid(Context context, BluetoothDevice device)
	{
		mContext = context;
		mDevice = device;
	}

	public void Connect()
	{
		Log.d(TAG, "Connecting to GATT service.");

		mGatt = mDevice.connectGatt(mContext, true, mGattCallback);
	}

	public void Disconnect()
	{
		Log.d(TAG, "Disconnecting from GATT service.");

		mGatt.close();
		mGatt = null;
	}

	public boolean IsConnected()
	{
		 return mGatt != null;
	}

	public void Write(int service, int characteristic,
		byte[] data)
	{
		BluetoothGattCharacteristic c = CharacteristicByUUID(
			service, characteristic);
		if(c == null)
			return;

		c.setValue(data);

		if(mGatt != null)
		{
			//mGatt.beginReliableWrite();
			mGatt.writeCharacteristic(c);
			//mGatt.executeReliableWrite();
		}
	}

	public void Read(int service, int characteristic)
	{
		BluetoothGattCharacteristic c = CharacteristicByUUID(
			service, characteristic);
		if(c == null)
			return;

		if(mGatt != null)
			mGatt.readCharacteristic(c);
	}

	protected static final UUID CHARACTERISTIC_UPDATE_NOTIFICATION_DESCRIPTOR_UUID =
		UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");

	public void SetNotify(int service, int characteristic)
	{
		BluetoothGattCharacteristic c = CharacteristicByUUID(
			service, characteristic);
		if(c == null)
			return;

		if(mGatt == null)
			return;

		mGatt.setCharacteristicNotification(c, true);
		BluetoothGattDescriptor descriptor = c.getDescriptor(
			CHARACTERISTIC_UPDATE_NOTIFICATION_DESCRIPTOR_UUID);
		descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
		mGatt.writeDescriptor(descriptor);

		// This must come after or the notifications above break.
		mGatt.readCharacteristic(c);
	}

	private BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {
		@Override
		public void onConnectionStateChange(BluetoothGatt gatt, int status,
				int newState) {
			if(status != BluetoothGatt.GATT_SUCCESS) {
				Log.d(TAG, "GATT connection failed!");

				return;
			}

			switch(newState)
			{
				case BluetoothProfile.STATE_CONNECTED:
				{
					Log.d(TAG, "GATT connected.");

					if(!gatt.discoverServices()) {
						Log.d(TAG, "Failed to start service discovery!");
					}

					break;
				}
				case BluetoothProfile.STATE_CONNECTING:
				{
					Log.d(TAG, "GATT connecting.");

					break;
				}
				case BluetoothProfile.STATE_DISCONNECTED:
				{
					Log.d(TAG, "GATT disconnected.");
					//if(mGatt != null)
						//mGatt.cose();

					mGatt = null;

					break;
				}
				case BluetoothProfile.STATE_DISCONNECTING:
				{
					Log.d(TAG, "GATT disconnecting.");

					break;
				}
				default:
				{
					Log.d(TAG, "Unknown GATT connection status: " + status);

					break;
				}
			}
		}

		@Override
		public void onServicesDiscovered(BluetoothGatt gatt, int status) {
			super.onServicesDiscovered(gatt, status);
			if(status != BluetoothGatt.GATT_SUCCESS) {
				Log.e(TAG, "GATT service discovery failed!");

				return;
			}

			JNI.LoadDrivers(mDevice.getAddress());
		}

		@Override
		public void onCharacteristicRead(BluetoothGatt gatt,
				BluetoothGattCharacteristic characteristic, int status) {

			int charID = (int)((characteristic.getUuid().getMostSignificantBits(
				) >> 32) & 0xFFFF);

			JNI.HaveData(mDevice.getAddress(), charID,
				characteristic.getValue());

			/*Log.d(TAG, "Value: " + characteristic.getStringValue(0));
			if(mReadCallback != null)
				mReadCallback.sensorValue(Integer.parseInt(characteristic.getStringValue(0)));*/
			super.onCharacteristicRead(gatt, characteristic, status);
		}

		@Override
		public void onCharacteristicChanged(BluetoothGatt gatt,
				BluetoothGattCharacteristic characteristic) {

			int charID = (int)((characteristic.getUuid().getMostSignificantBits(
				) >> 32) & 0xFFFF);

			JNI.HaveData(mDevice.getAddress(), charID,
				characteristic.getValue());

			/*if(mReadCallback != null)
				mReadCallback.sensorValue(Integer.parseInt(characteristic.getStringValue(0)));*/
			super.onCharacteristicChanged(gatt, characteristic);
		}

		@Override
		public void onCharacteristicWrite(BluetoothGatt gatt,
				BluetoothGattCharacteristic characteristic, int status) {
			super.onCharacteristicWrite(gatt, characteristic, status);

			if(status != BluetoothGatt.GATT_SUCCESS)
				Log.e(TAG, "Write failed!");
			/*else
				Log.d(TAG, "Wrote: " + characteristic.getStringValue(0));*/
		}

		@Override
		public void onDescriptorRead(BluetoothGatt gatt,
				BluetoothGattDescriptor descriptor, int status) {
			super.onDescriptorRead(gatt, descriptor, status);
		}
	};

	private BluetoothGattCharacteristic CharacteristicByUUID(
		int service, int characteristic)
	{
		for(BluetoothGattService serv : mGatt.getServices())
		{
			int serviceID = (int)((serv.getUuid().getMostSignificantBits(
				) >> 32) & 0xFFFF);
			if(serviceID != service)
				continue;

			for(BluetoothGattCharacteristic c : serv.getCharacteristics())
			{
				int charID = (int)((c.getUuid().getMostSignificantBits(
					) >> 32) & 0xFFFF);
				if(charID == characteristic)
					return c;
			}
		}

		return null;
	}
}
