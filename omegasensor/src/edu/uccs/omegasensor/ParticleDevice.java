package edu.uccs.omegasensor;

import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;
import android.content.Intent;
import android.util.Log;

@SuppressLint("DefaultLocale")
public class ParticleDevice {

	private BluetoothDevice mDevice = null;
	private BluetoothService mService = null;
	private BluetoothGatt mGatt = null;
	
	private String mName = "Particle out of Range";

	// ------------------------------------------------------------------------
	// UUIDs for the service and it's characteristics
	// ------------------------------------------------------------------------
	private final static UUID UUID_SERVICE = UUID.fromString(
		"4c56c69a-d494-4dee-aefe-7e064803ecee");
	private final static UUID UUID_PASSCODE = UUID.fromString(
		"0000c69b-0000-1000-8000-00805f9b34fb");
	private final static UUID UUID_OPCODE = UUID.fromString(
		"0000c69c-0000-1000-8000-00805f9b34fb");
	private final static UUID UUID_OPERAND = UUID.fromString(
		"0000c69d-0000-1000-8000-00805f9b34fb");
	private final static UUID UUID_RESULT = UUID.fromString(
		"0000c69e-0000-1000-8000-00805f9b34fb");
	private final static UUID UUID_RESULT_DESC = UUID.fromString(
		"00002902-0000-1000-8000-00805f9b34fb");
	
	// ------------------------------------------------------------------------
	// GATT service and characteristics to control the Particle
	// ------------------------------------------------------------------------
	private BluetoothGattService mGattService = null;
	private BluetoothGattCharacteristic mGattPasscode = null;
	private BluetoothGattCharacteristic mGattOpcode = null;
	private BluetoothGattCharacteristic mGattOperand = null;
	private BluetoothGattCharacteristic mGattResult = null;
	private BluetoothGattDescriptor mGattResultDesc = null;
	
	// ------------------------------------------------------------------------
	// Setters
	// ------------------------------------------------------------------------
	
	private final static byte OPCODE_SET_PASSCODE = (byte)0x01;
	private final static byte OPCODE_SET_UUID = (byte)0x10;
	private final static byte OPCODE_SET_MAJOR = (byte)0x11;
	private final static byte OPCODE_SET_MINOR = (byte)0x12;
	private final static byte OPCODE_SET_MEASURED_PWR = (byte)0x13;
	
	// ------------------------------------------------------------------------
	// Getters
	// ------------------------------------------------------------------------
	
	private final static byte OPCODE_GET_FIRMWARE_VER = (byte)0x81;
	private final static byte OPCODE_GET_UUID = (byte)0x90;
	private final static byte OPCODE_GET_MAJOR = (byte)0x91;
	private final static byte OPCODE_GET_MINOR = (byte)0x92;
	private final static byte OPCODE_GET_MEASURED_PWR = (byte)0x93;
	private final static byte OPCODE_GET_NAME = (byte)0x87;
	
	// ------------------------------------------------------------------------
	// Miscellaneous
	// ------------------------------------------------------------------------
	
	private Timer mRssiTimer = null;
	private final static long RSSI_UPDATE_PERIOD = 1000 * 1; // 1sec
	private final static int RSSI_TRIGGER_RANGE = -50;

    /// Debug tag.
    private final static String TAG = "ParticleDevice";

	public ParticleDevice(BluetoothService service, BluetoothDevice device) {
		super();
		
		mService = service;
		mDevice = device;
		
		// This will trigger when the RSSI update period has elapsed.
		updateRssi();
		
		// Connect to the device so we can probe it for services.
		connect();
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
				Log.d(TAG, "GATT service discovery failed!");
				
				return;
			}
			
			// Find the GATT service for the particle.
			mGattService = gatt.getService(UUID_SERVICE);
			
			// Get the characteristics for the GATT service used to talk
			// with the particle.
			if(mGattService != null) {
				mGattPasscode = mGattService.getCharacteristic(UUID_PASSCODE);
				mGattOpcode = mGattService.getCharacteristic(UUID_OPCODE);
				mGattOperand = mGattService.getCharacteristic(UUID_OPERAND);
				mGattResult = mGattService.getCharacteristic(UUID_RESULT);
				mGattResultDesc = mGattResult.getDescriptor(UUID_RESULT_DESC);
			}
			
			// Get the name of the device.
			requestName();
		}

		@Override
		public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status) {
			super.onReadRemoteRssi(gatt, rssi, status);
			
			Log.d(TAG, "RSSI: " + rssi);
			
			// Once the device comes within range, stop updating the RSSI
			// and start the desired intent.
			if(rssi >= RSSI_TRIGGER_RANGE) {
				mRssiTimer.cancel();
				mRssiTimer = null;
				
				Intent intent = new Intent(mService, IgnitionActivity.class);
				intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				intent.putExtra("device", mDevice.getAddress().toUpperCase());
				mService.startActivity(intent);
			}
		}

		@Override
		public void onCharacteristicRead(BluetoothGatt gatt,
				BluetoothGattCharacteristic characteristic, int status) {
			switch(mGattOpcode.getValue()[0])
			{
				case OPCODE_GET_FIRMWARE_VER:
					Log.d(TAG, "Fimware Version: " + characteristic.getStringValue(0));
					break;
				case OPCODE_GET_MAJOR:
					Log.d(TAG, "iBeacon Major: " + (int)characteristic.getValue()[0]);
					Log.d(TAG, "iBeacon Major: " + (int)characteristic.getValue()[1]);
					break;
				case OPCODE_GET_NAME:
					mName = characteristic.getStringValue(0).replace("\0", "");
					Log.d(TAG, "Name: " + mName);
					{
						String hexString = "";

						for(byte b : characteristic.getValue())
							hexString += String.format("%02x", b);
						Log.d(TAG, "Name: " + hexString);
					}
					break;
				default:
					Log.d(TAG, "Unknown Result");
					break;
			}

			super.onCharacteristicRead(gatt, characteristic, status);
		}

		@Override
		public void onCharacteristicWrite(BluetoothGatt gatt,
				BluetoothGattCharacteristic characteristic, int status) {
			mGatt.readCharacteristic(mGattResult);
			super.onCharacteristicWrite(gatt, characteristic, status);
		}

		@Override
		public void onDescriptorRead(BluetoothGatt gatt,
				BluetoothGattDescriptor descriptor, int status) {
			Log.d(TAG, "Desc: " + descriptor.getValue());
			super.onDescriptorRead(gatt, descriptor, status);
		}
	};
	
	public void requestFirmwareVer() {
		byte[] opcode = { OPCODE_GET_FIRMWARE_VER };
		mGattOpcode.setValue(opcode);
		mGatt.writeCharacteristic(mGattOpcode);
	}
	
	public void requestName() {
		byte[] opcode = { OPCODE_GET_NAME };
		mGattOpcode.setValue(opcode);
		mGatt.writeCharacteristic(mGattOpcode);
	}
	
	public void updateRssi() {
		if(mRssiTimer != null) {
			return;
		}

		// Handler to determine RSSI.
		mRssiTimer = new Timer();
		mRssiTimer.scheduleAtFixedRate(new TimerTask() {
			@Override
			public void run() {
				// After the RSSI update period has elapsed, re-read the RSSI.
				if(mGatt != null) {
					mGatt.readRemoteRssi();
				}
			}
		}, 0, RSSI_UPDATE_PERIOD);
	}
	
	public String macAddress() {
		return mDevice.getAddress();
	}
	
	public String name() {
		return mName;
	}

}
