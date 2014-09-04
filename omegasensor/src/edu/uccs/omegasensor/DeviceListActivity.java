package edu.uccs.omegasensor;

import java.util.ArrayList;
import java.util.List;

import android.app.ListActivity;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.view.View;
import android.widget.ListView;

public class DeviceListActivity extends ListActivity {

	// Intent actions.
	public static final String UPDATE_DEVICES = "edu.uccs.omegasensor.UPDATE_DEVICES";
	
	private BluetoothService mService = null;
    private boolean mBound = false;
	
	private final static String TAG = "DeviceListActivity";
	private List<DemoDevice> mDevices = null;
	
	private DeviceAdapter mAdapter = null;
	
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
            mAdapter.notifyDataSetChanged();
            
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
	
		mDevices = new ArrayList<DemoDevice>();
		mAdapter = new DeviceAdapter(this, mDevices);
		setListAdapter(mAdapter);
		
		LocalBroadcastManager manager = LocalBroadcastManager.getInstance(this);
		IntentFilter intentFilter = new IntentFilter();
		intentFilter.addAction(UPDATE_DEVICES);
		manager.registerReceiver(mIntentReceiver, intentFilter);
	}
	
	@Override
	protected void onListItemClick(ListView l, View v, int position, long id) {
		super.onListItemClick(l, v, position, id);
		DemoDevice dev = mDevices.get(position);
		Log.d(TAG, "Clicked on device: " + dev.name());
		Intent intent = new Intent(this, DeviceActivity.class);
		intent.putExtra("device", dev.name());
		startActivity(intent);
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

	private BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			if(intent.getAction().equals(UPDATE_DEVICES)) {
				Log.d(TAG, "Updating device list..");
				mDevices.clear();
				mDevices.addAll(mService.deviceList());
				mAdapter.notifyDataSetChanged();
				Log.d(TAG, "Devices in list: " + mDevices.size());
			}
		}
	};

}
