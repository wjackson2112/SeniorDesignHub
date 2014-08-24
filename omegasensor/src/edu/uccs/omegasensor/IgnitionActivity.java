package edu.uccs.omegasensor;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.ImageButton;

public class IgnitionActivity extends Activity {

	private BluetoothService mService = null;
    private boolean mBound = false;
	
	private final static String TAG = "IgnitionActivity";
	private String mDeviceUUID = null;
	
	/** Defines callbacks for service binding, passed to bindService() */
    private ServiceConnection mConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName className,
                IBinder service) {
            // We've bound to LocalService, cast the IBinder and get LocalService instance
            BluetoothService.BluetoothBinder binder = (BluetoothService.BluetoothBinder) service;
            mService = binder.getService();
            mBound = true;
            
            // Turn on the ignition.
            mService.setIgnition(mDeviceUUID);
            
            Log.d(TAG, "The Bluetooth service has been bound to this activity.");
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
        	mBound = false;
        	
        	Log.d(TAG, "The Bluetooth service is no longer bound to this activity.");
        }
    };

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    
	    setContentView(R.layout.layout_ignition);
	    
	    final ImageButton button = (ImageButton)findViewById(R.id.ignitionButton);
        button.setOnTouchListener(new OnTouchListener() {
        	@Override
        	public boolean onTouch(View v, MotionEvent event) {
        		if(event.getAction() == MotionEvent.ACTION_DOWN) {
        			mService.setStarter(mDeviceUUID);
        		} else if (event.getAction() == MotionEvent.ACTION_UP) {
        			mService.clearStarter(mDeviceUUID);
        		}
        		
        		return true;
        	}
        });
	}

	@Override
	protected void onStart() {
		super.onStart();
		
		Intent intent = new Intent(this, BluetoothService.class);
		startService(intent);
		bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
		
		mDeviceUUID = getIntent().getStringExtra("device");
	}

	@Override
	protected void onStop() {
		super.onStop();
		
		// Make sure we release the starter.
		// mService.clearStarter(mDeviceUUID);

		// Turn off the ignition.
		// mService.clearIgnition(mDeviceUUID);
		
		// Resume updating the RSSI on this device.
		// mService.updateRssi(mDeviceUUID);
		
		// Unbind from the service
        if (mBound) {
            unbindService(mConnection);
            mBound = false;
        }
	}

}
