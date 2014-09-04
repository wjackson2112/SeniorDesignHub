package edu.uccs.omegasensor;

import java.util.Arrays;
import java.util.UUID;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.ParcelUuid;
import android.os.Parcelable;
import android.util.Log;

import com.google.zxing.integration.android.IntentIntegrator;
import com.google.zxing.integration.android.IntentResult;

import edu.uccs.omegasensor.BluetoothScanRecord.ParseErrorException;

public class ScanActivity extends MainActivity {

	//private String mAuthCode = null;

	private final static UUID[] SERVICE_UUIDS = { UUID.fromString("7b1e3740-1dc8-11e4-8c21-0800200c9a66") };

	private final static long SCAN_PERIOD = 30 * 1000; // 10 seconds.
	private final static String TAG = "ScanActivity";
	
	private Handler mHandler = null;
	private boolean mScanStarted = false;
	
	private boolean mDeviceFound = false;
	private Context mContext = this;

	private static boolean hasService(String desiredService, byte[] scanRecord) {
		final int serviceOffset = 9;
		
		try {
			byte[] service = Arrays.copyOfRange(scanRecord,
				serviceOffset, serviceOffset + 16);
			
			String hexService = "";

			for(byte b : service)
				hexService += String.format("%02x", b);

			//Log.d(TAG, desiredService.replaceAll("-", "") + " =? " + hexService);
			
			return desiredService.replaceAll("-", "").equalsIgnoreCase(hexService);
		} catch(Exception e) {
			return false;
		}
	}
	
	private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			BluetoothDevice deviceExtra = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
			Parcelable[] uuidExtra = intent.getParcelableArrayExtra(BluetoothDevice.EXTRA_UUID);
			
			if(deviceExtra.getUuids() != null) {
				for(ParcelUuid uuid : deviceExtra.getUuids()) {
					Log.d(TAG, "Service UUID: " + uuid);
				}
			} else {
				Log.d(TAG, "Failed to discover any services.");
			}
			
			if(uuidExtra != null) {
				for(Parcelable uuid : uuidExtra) {
					ParcelUuid u = (ParcelUuid)uuid;
					Log.d(TAG, "Service UUID: " + u.toString());
				}
			}
		}		
	};

	private BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {

		@Override
		public void onCharacteristicChanged(BluetoothGatt gatt,
				BluetoothGattCharacteristic characteristic) {
			Log.d(TAG, "Characteristic changed: " + characteristic.getUuid());
			Log.d(TAG, "Value: " + characteristic.getValue());
		}

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
						Log.d(TAG, "GATT connecting.");
						break;
					case BluetoothProfile.STATE_DISCONNECTED:
					{
						Log.d(TAG, "GATT disconnected.");
						if(!gatt.discoverServices()) {
							Log.d(TAG, "Failed to start service discovery!");
						}
						break;
					}
					case BluetoothProfile.STATE_DISCONNECTING:
						Log.d(TAG, "GATT disconnecting.");
						break;
					default:
						Log.d(TAG, "Unknown GATT connection status: " + status);
						break;
				}
		}

		@Override
		public void onServicesDiscovered(BluetoothGatt gatt, int status) {
			if(status != BluetoothGatt.GATT_SUCCESS) {
				Log.d(TAG, "Service discovery failed!");
				return;
			}
			
			for(BluetoothGattService service : gatt.getServices()) {
				Log.d(TAG, "Service: " + service.getUuid());
				
				for(BluetoothGattCharacteristic characteristic : service.getCharacteristics()) {
					Log.d(TAG, " Characteristic: " + characteristic.getUuid());
					
					// 3742
					// 3741
					if(characteristic.getUuid().toString().equalsIgnoreCase("0000c69b-0000-1000-8000-00805f9b34fb")) {
						Log.d(TAG, "Write passcode...");
						characteristic.setValue("");
						gatt.writeCharacteristic(characteristic);
					}
					
					if(characteristic.getUuid().toString().equalsIgnoreCase("0000c69c-0000-1000-8000-00805f9b34fb")) {
						Log.d(TAG, "Write opcode...");
						byte[] val = { 0 };
						characteristic.setValue(val);
						gatt.writeCharacteristic(characteristic);
					}
					
					if(characteristic.getUuid().toString().equalsIgnoreCase("0000c69d-0000-1000-8000-00805f9b34fb")) {
						Log.d(TAG, "Write operand...");
						byte[] val = { 0 };
						characteristic.setValue(val);
						gatt.writeCharacteristic(characteristic);
					}
					
					if(characteristic.getUuid().toString().equalsIgnoreCase("0000c69e-0000-1000-8000-00805f9b34fb")) {
						Log.d(TAG, "Write operand...");
						gatt.readCharacteristic(characteristic);
					}
					
					for(BluetoothGattDescriptor desc : characteristic.getDescriptors()) {
						Log.d(TAG, "  Descriptor: " + desc.getUuid() + " | Value: " + desc.getValue());
					}
				}
			}
			
			//Log.d(TAG, "Closing the GATT connection.");
			//gatt.close();
		}
	};

	private BluetoothAdapter.LeScanCallback mLeScanCallback =
			new BluetoothAdapter.LeScanCallback() {		
		@Override
		public void onLeScan(final BluetoothDevice device, final int rssi, final byte[] scanRecord) {
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					if(!device.getName().matches("^SensorHub[0-9]{3}$"))
						return;
					Log.d(TAG, "Found sensor hub: " + device.getName());
					if(!hasService(SERVICE_UUIDS[0].toString(), scanRecord))
						return;
					Log.d(TAG, "Found service on: " + device.getName());
					if(!device.getAddress().equalsIgnoreCase("F2:30:5D:58:25:78"))
						return;
					
					// Connect w/ GATT to probe the services.
					if(!mDeviceFound) {
						mDeviceFound = true;
						Log.d(TAG, "Trying to connect to GATT.");
						device.connectGatt(mContext, true, mGattCallback);
					}
					
					// Stop the scan.
					/*
					if(mScanStarted) {
						mAdapter.stopLeScan(mLeScanCallback);
						mScanStarted = false;
						Log.d(TAG, "BLE Scan Stopped");
					}
					*/
					
					/*try {
						BluetoothEIR eir = new BluetoothEIR(scanRecord);
					} catch (ParseErrorException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}*/
					
					/*
					String hexDump = "";
					
					for(byte b : scanRecord)
						hexDump += String.format("%02x", b);
					
					Log.d(TAG, "Scan Record Length: " + scanRecord.length);
					Log.d(TAG, hexDump);
					*/
					
					// Do stuff.
					Log.d(TAG, "Device found: " + device.toString() + " (" + rssi + "dBm)");
					if(device.getUuids() != null) {
						for(ParcelUuid uuid : device.getUuids()) {
							Log.d(TAG, "Service UUID: " + uuid);
						}
					} else {
						//if(!device.fetchUuidsWithSdp()) {
							//Log.d(TAG, "Failed to initiate service discovery.");
						//}
					}
					
					if(device.getName() != null) {
						Log.d(TAG, "Device name: " + device.getName());
					}
					
					//Log.d(TAG, "Device type: " + device.getType());
				}
			});
		}
	};

	@Override
	protected void onStop() {
		super.onStop();
		
		// Unbind from the service
        if (mBound) {
            unbindService(mConnection);
            mBound = false;
        }
	}

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    
	    setContentView(R.layout.layout_scan);
	
	    IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_UUID);
	    registerReceiver(mReceiver, filter);
	    
	    //ListView scanList = (ListView)findViewById(R.id.scan_list);
		//scanList.setAdapter(adapter);
		
	    mHandler = new Handler();
	    
	    /*
	    if(mAuthCode == null) {
		    IntentIntegrator integrator = new IntentIntegrator(this);
		    integrator.addExtra("PROMPT_MESSAGE", "Please scan the device authorization code.");
			integrator.initiateScan(IntentIntegrator.QR_CODE_TYPES);
	    }
	    */
	}

	@Override
	protected void onStart() {
		super.onStart();

		Intent intent = new Intent(this, BluetoothService.class);
		startService(intent);
		bindService(intent, mConnection, Context.BIND_AUTO_CREATE);

		/*
		if(!mScanStarted) {
			mHandler.postDelayed(new Runnable(){
				@Override
				public void run() {
					if(mScanStarted) {
						mAdapter.stopLeScan(mLeScanCallback);
						mScanStarted = false;
						Log.d(TAG, "BLE Scan Stopped");
					}
				}
			}, SCAN_PERIOD);
			//mAdapter.startLeScan(SERVICE_UUIDS, mLeScanCallback);
			mAdapter.startLeScan(mLeScanCallback);
			mScanStarted = true;
			Log.d(TAG, "BLE Scan Started");
		}
		*/
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		IntentResult scanResult = IntentIntegrator.parseActivityResult(requestCode, resultCode, data);
		if(scanResult != null) {
			Log.d(TAG, "Scan result: " + scanResult.getContents());
			//mAuthCode = scanResult.getContents();
		}
		
		super.onActivityResult(requestCode, resultCode, data);
	}
	
	BluetoothService mService;
    boolean mBound = false;
    
	/** Defines callbacks for service binding, passed to bindService() */
    private ServiceConnection mConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName className,
                IBinder service) {
            // We've bound to LocalService, cast the IBinder and get LocalService instance
            BluetoothService.BluetoothBinder binder = (BluetoothService.BluetoothBinder) service;
            mService = binder.getService();
            mBound = true;
            
            // Start the scan.
            mService.startScan();
            
            Log.d(TAG, "The Bluetooth service has been bound to this activity.");
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
        	mBound = false;
        	
        	Log.d(TAG, "The Bluetooth service is no longer bound to this activity.");
        }
    };

}
