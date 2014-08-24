package edu.uccs.omegasensor;

import java.util.ArrayList;
import java.util.List;

import android.app.ListActivity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;

public class DeviceListActivity extends ListActivity {

	private BluetoothService mService = null;
    private boolean mBound = false;
	
	private final static String TAG = "DeviceListActivity";
	private List<ParticleDevice> mDevices = null;
	
	/** Defines callbacks for service binding, passed to bindService() */
    private ServiceConnection mConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName className,
                IBinder service) {
            // We've bound to LocalService, cast the IBinder and get LocalService instance
            BluetoothService.BluetoothBinder binder = (BluetoothService.BluetoothBinder) service;
            mService = binder.getService();
            mBound = true;
            
            // Add all the devices to the list.
            mDevices.addAll(mService.deviceList());
            
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
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
	
		mDevices = new ArrayList<ParticleDevice>();
		DeviceAdapter adapter = new DeviceAdapter(this, mDevices);
		setListAdapter(adapter);
	}
	
	@Override
	protected void onStart() {
		super.onStart();
		
		Intent intent = new Intent(this, BluetoothService.class);
		startService(intent);
		bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
	}

	@Override
	protected void onStop() {
		super.onStop();
		
		// Unbind from the service
        if (mBound) {
            unbindService(mConnection);
            mBound = false;
        }
	}

}
