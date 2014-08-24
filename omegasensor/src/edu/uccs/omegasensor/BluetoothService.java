package edu.uccs.omegasensor;

import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.UUID;

import android.annotation.SuppressLint;
import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import edu.uccs.omegasensor.BluetoothScanRecord.ParseErrorException;

/**
 * Control communication with the Omega Sensor module.
 * @author John Eric Martin <jmarti25@uccs.edu>
 * The Android device acts as the Bluetooth Low Energy central and the Omega
 * Sensor module acts as the peripheral. Communication can continue while the
 * application is in the background.
 */
@SuppressLint("DefaultLocale")
public class BluetoothService extends Service {

	// ------------------------------------------------------------------------
	// Bluetooth Low Energy Driver
	// ------------------------------------------------------------------------
	
	/// Manager for the Bluetooth adapter on the phone/device.
	private BluetoothManager mManager = null;
	
	/// BLE adapter that handles all communication.
	private BluetoothAdapter mAdapter = null;
	
	// ------------------------------------------------------------------------
	// Bluetooth Low Energy Peripheral Scan
	// ------------------------------------------------------------------------
	
	/// How long to scan for BLE peripherals (30 seconds).
	private final static long SCAN_PERIOD = 30 * 1000;
	
	/// If a scan for BLE peripherals is running.
	private boolean mScanStarted = false;
	
	/// This handler will stop the scan after the SCAN_PERIOD has elapsed.
	private Handler mScanHandler = null;

	// ------------------------------------------------------------------------
	// Bluetooth Low Energy Service Binder Connection
	// ------------------------------------------------------------------------
	
	/// Binder given to clients.
    private final IBinder mBinder = new BluetoothBinder();
    
    // ------------------------------------------------------------------------
	// Device Preferences
	// ------------------------------------------------------------------------
    
    /// Name of the device preferences file.
    private final static String DEVICE_PREFS_NAME = "Devices";
    
    /// Device preferences object.
    private SharedPreferences mSettings = null;
    
    // ------------------------------------------------------------------------
	// Miscellaneous
	// ------------------------------------------------------------------------
    
    /// List of service UUIDs to scan for.
    private final static UUID[] SERVICE_UUIDS = {
    	UUID.fromString("8aefb031-6c32-486f-825b-e26fa193487d")
    };

    /// Debug tag.
    private final static String TAG = "BluetoothService";
    
    /**
     * Class used for the client Binder. We assume the service is only
     * accessed from the same process (it might break otherwise).
     */
    public class BluetoothBinder extends Binder {
        BluetoothService getService() {
            // Return this instance of BluetoothService so clients can call
        	// public methods.
            return BluetoothService.this;
        }
    }

    /**
     * Fetch the Binder for this service. This Binder should only be used from
     * within the same process.
     * @param intent Intent that was used to bind to this service.
     * @returns Binder for this service.
     */
	@Override
	public IBinder onBind(Intent intent) {
		return mBinder;
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		Log.d(TAG, "Starting service.");

		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public void onCreate() {
		Log.d(TAG, "Created service.");
	
		// Create the list to store all devices.
		mDevices = new HashMap<String, ParticleDevice>();
		
		// Handler to stop a BLE peripheral scan.
		mScanHandler = new Handler();
		
		// Get the BLE manager and adapter.
		mManager = (BluetoothManager)getSystemService(Context.BLUETOOTH_SERVICE);
	    mAdapter = mManager.getAdapter();
	    
	    // Restore the settings.
		mSettings = getSharedPreferences(DEVICE_PREFS_NAME, 0);
		
		// Add the devices we know about.
		for(String deviceMAC : mSettings.getStringSet("deviceList",
				new HashSet<String>())) {
			// Wrap the BLE device in a ParticleDevice.
			ParticleDevice dev = new ParticleDevice(this,
				mAdapter.getRemoteDevice(deviceMAC));
			
			// Add the device to the list.
			mDevices.put(deviceMAC, dev);
		}

		super.onCreate();
	}

	@Override
	public void onDestroy() {
		Log.d(TAG, "Stopping service.");

		super.onDestroy();
	}
	
	/**
	 * Check to see if a BLE peripheral scan is running.
	 * @return true if scanning for BLE peripherals, false otherwise.
	 */
	public final boolean isPerformingScan() {
		return mScanStarted;
	}
	
	/**
	 * Start a scan for BLE peripherals.
	 */
	public void startScan() {
		// If a scan has already started, just return.
		if(mScanStarted)
			return;

		// This will trigger when the scan period has elapsed.
		mScanHandler.postDelayed(new Runnable(){
			@Override
			public void run() {
				// After the scan period has elapsed, stop the scan.
				stopScan();
			}
		}, SCAN_PERIOD);
		
		// Start the scan with a list of services we wish to find.
		// mAdapter.startLeScan(SERVICE_UUIDS, mLeScanCallback);
		
		// Start the scan.
		mAdapter.startLeScan(mScanCallback);
		mScanStarted = true;
		
		Log.d(TAG, "BLE Scan Started");
	}
	
	public void stopScan() {
		// Don't try to stop a scan that isn't started.
		if(!mScanStarted)
			return;

		// Stop the scan.
		mAdapter.stopLeScan(mScanCallback);
		mScanStarted = false;
		
		Log.d(TAG, "BLE Scan Stopped");
	}
	
	// ------------------------------------------------------------------------
	// Bluetooth Low Energy Scan Callback
	// ------------------------------------------------------------------------
	
	private Map<String, ParticleDevice> mDevices = null;
	
	/// Bluetooth Low Energy Scan Callback
	private BluetoothAdapter.LeScanCallback mScanCallback =
			new BluetoothAdapter.LeScanCallback() {
		@Override
		public void onLeScan(BluetoothDevice device, int rssi, byte[] scanRecord) {
			if(!device.getAddress().equalsIgnoreCase("F2:30:5D:58:25:78"))
				return;
			
			// Only process the device if it is not already in the list.
			if(mDevices.containsKey(device.getAddress().toUpperCase()))
				return;
			
			try {
				// Parse the scan record for service UUIDs.
				BluetoothScanRecord record = new BluetoothScanRecord(scanRecord);

				int matches = 0;
				
				// Count the number of matched services.
				for(UUID service : SERVICE_UUIDS)
				{
					/*
					for(UUID possibleMatch : record.serviceUUIDs()) {
						Log.d(TAG, service.toString() + " = " + possibleMatch.toString());
						if(service.compareTo(possibleMatch) == 0) {
							matches++;
							break;
						}
					}
					*/
					Log.d(TAG, "Service count: " + record.serviceUUIDs().size());
					if(record.serviceUUIDs().contains(service)) {
						matches++;
					}
				}
				Log.d(TAG, "Matches: " + matches);
				// If one of the services was not found, don't add the device
				// to the list.
				if(matches < 1)
					return;
			} catch (ParseErrorException e) {
				Log.d(TAG, "Failed to parse scan record. Device ignored.");
				
				return;
			}
			
			// Wrap the BLE device into a ParticleDevice.
			ParticleDevice dev = new ParticleDevice(
				BluetoothService.this, device);
			
			// Add the new particle to the device list.
			mDevices.put(device.getAddress().toUpperCase(), dev);
			
			// Save the device list.
			SharedPreferences.Editor editor = mSettings.edit();
			editor.putStringSet("deviceList", mDevices.keySet());
			editor.commit();
		}		
	};
	
	public Collection<ParticleDevice> deviceList() {
		return mDevices.values();
	}
	
	public void setIgnition(String deviceUUID) {
		Log.d(TAG, "Turning on the ignition.");
	}
	
	public void clearIgnition(String deviceUUID) {
		Log.d(TAG, "Turning off the ignition.");
	}
	
	public void setStarter(String deviceUUID) {
		Log.d(TAG, "Triggering the starter.");
	}
	
	public void clearStarter(String deviceUUID) {
		Log.d(TAG, "Releasing the starter.");
	}
	
	public void updateRssi(String deviceUUID) {
		Log.d(TAG, "Resuming RSSI updates.");
		
		ParticleDevice dev = mDevices.get(deviceUUID);
		if(dev != null) {
			dev.updateRssi();
		}
	}

}
