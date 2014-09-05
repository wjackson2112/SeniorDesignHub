package edu.uccs.omegasensor;

import java.util.UUID;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;
import android.util.Log;

@SuppressLint("DefaultLocale")
public class DemoDevice {

	private BluetoothDevice mDevice = null;
	private BluetoothService mService = null;
	private BluetoothGatt mGatt = null;
	
	private String mName = "SensorHub???";

	// ------------------------------------------------------------------------
	// UUIDs for the service and it's characteristics
	// ------------------------------------------------------------------------
	private final static UUID UUID_SERVICE = UUID.fromString(
		"7b1e3740-1dc8-11e4-8c21-0800200c9a66");
	private final static UUID UUID_CHAR_RX = UUID.fromString(
		"00003741-0000-1000-8000-00805f9b34fb");
	private final static UUID UUID_CHAR_TX = UUID.fromString(
		"00003742-0000-1000-8000-00805f9b34fb");
	
	// ------------------------------------------------------------------------
	// GATT service and characteristics to control the Particle
	// ------------------------------------------------------------------------
	private BluetoothGattService mGattService = null;
	private BluetoothGattCharacteristic mGattRX = null;
	private BluetoothGattCharacteristic mGattTX = null;

    /// Debug tag.
    private final static String TAG = "DemoDevice";

	public DemoDevice(BluetoothService service, BluetoothDevice device,
			String name) {
		super();
		
		mService = service;
		mDevice = device;
		mName = name;
		
		// Connect to the device so we can probe it for services.
		// connect();
	}
	
	public void connect() {
		Log.d(TAG, "Connecting to GATT service.");
		
		mGatt = mDevice.connectGatt(mService, true, mGattCallback);
	}
	
	public void disconnect() {
		Log.d(TAG, "Disconnecting from GATT service.");
		
		mGatt.close();
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

			for(BluetoothGattService serv : gatt.getServices())
			{
				Log.d(TAG, "Service: " + serv.getUuid().toString());

				for(BluetoothGattCharacteristic c : serv.getCharacteristics())
				{
					Log.d(TAG, " Characteristic: " + c.getUuid().toString());

					for(BluetoothGattDescriptor desc : c.getDescriptors())
					{
						Log.d(TAG, " Descriptor: " + desc.getUuid().toString());
					}
				}
			}
			
			// Find the GATT service for the particle.
			mGattService = gatt.getService(UUID_SERVICE);
			if(mGattService == null) {
				Log.e(TAG, "GATT service is null!");
				
				return;
			}

			// Get the characteristics for the GATT service used to talk
			// with the particle.
			mGattRX = mGattService.getCharacteristic(UUID_CHAR_RX);
			mGattTX = mGattService.getCharacteristic(UUID_CHAR_TX);
			
			// Check for the characteristics.
			if(mGattRX == null || mGattTX == null) {
				Log.e(TAG, "A GATT characteristic is null!");
				
				return;
			}
			
			// Turn the LED on.
			//mGattTX.setValue("1");
			//mGatt.writeCharacteristic(mGattTX);
			
			// Get notifications on new RX values.
			readAndNotify();
		}

		@Override
		public void onCharacteristicRead(BluetoothGatt gatt,
				BluetoothGattCharacteristic characteristic, int status) {
			Log.d(TAG, "Value: " + characteristic.getStringValue(0));
			if(mReadCallback != null)
				mReadCallback.sensorValue(Integer.parseInt(characteristic.getStringValue(0)));
			super.onCharacteristicRead(gatt, characteristic, status);
		}

		@Override
		public void onCharacteristicChanged(BluetoothGatt gatt,
				BluetoothGattCharacteristic characteristic) {
			Log.d(TAG, "Value: " + characteristic.getStringValue(0));
			if(mReadCallback != null)
				mReadCallback.sensorValue(Integer.parseInt(characteristic.getStringValue(0)));
			super.onCharacteristicChanged(gatt, characteristic);
		}
		
		@Override
		public void onCharacteristicWrite(BluetoothGatt gatt,
				BluetoothGattCharacteristic characteristic, int status) {
			super.onCharacteristicWrite(gatt, characteristic, status);

			if(status != BluetoothGatt.GATT_SUCCESS)
				Log.e(TAG, "Write failed!");
			else
				Log.d(TAG, "Wrote: " + characteristic.getStringValue(0));
		}

		@Override
		public void onDescriptorRead(BluetoothGatt gatt,
				BluetoothGattDescriptor descriptor, int status) {
			super.onDescriptorRead(gatt, descriptor, status);
		}
	};
	
	public interface ReadCallback {
		void sensorValue(int value);
	}
	
	private ReadCallback mReadCallback = null;
	public void setReadCallback(ReadCallback callback) {
		mReadCallback = callback;
	}
	
	public String macAddress() {
		return mDevice.getAddress();
	}
	
	public String name() {
		return mName;
	}
	
	public void send(String data) {
		mGattTX.setValue(data);
		mGatt.writeCharacteristic(mGattTX);
	}
	
	protected static final UUID CHARACTERISTIC_UPDATE_NOTIFICATION_DESCRIPTOR_UUID =
		UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");
	
	public void readAndNotify() {
		if(mGatt == null || mGattRX == null) {
			return;
		}

		mGatt.setCharacteristicNotification(mGattRX, true);
		BluetoothGattDescriptor descriptor = mGattRX.getDescriptor(
			CHARACTERISTIC_UPDATE_NOTIFICATION_DESCRIPTOR_UUID);
        descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE); 
        mGatt.writeDescriptor(descriptor);
        
        // This must come after or the notifications above break.
        mGatt.readCharacteristic(mGattRX);
	}

}
