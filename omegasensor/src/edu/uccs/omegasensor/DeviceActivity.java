package edu.uccs.omegasensor;

import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ProgressBar;

public class DeviceActivity extends Activity {

	private BluetoothService mService = null;
    private boolean mBound = false;
	
	private final static String TAG = "DeviceActivity";
	private String mDeviceName = null;
	private DemoDevice mDevice = null;
	
	private int mMinValue = 0;
	private int mMaxValue = 1023;
	private int mValue = 0;
	
	private Timer mTimer = null;
	
	/** Defines callbacks for service binding, passed to bindService() */
    private ServiceConnection mConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName className,
                IBinder service) {
            // We've bound to LocalService, cast the IBinder and get LocalService instance
            BluetoothService.BluetoothBinder binder = (BluetoothService.BluetoothBinder) service;
            mService = binder.getService();
            mService.stopScan();
            mBound = true;
            
            Log.d(TAG, "The Bluetooth service has been bound to this activity.");
            
            // Find the device to control.
    		mDeviceName = getIntent().getStringExtra("device");
    		for(DemoDevice dev : mService.deviceList()) {
    			if(dev.name().equals(mDeviceName)) {
    				mDevice = dev;
    				mDevice.connect();
    				break;
    			}
    		}
    		
    		if(mDevice != null) {
    			mDevice.setReadCallback(new DemoDevice.ReadCallback() {
					@Override
					public void sensorValue(int value) {
						if(value < mMinValue)
							value = mMinValue;
						else if(value > mMaxValue)
							value = mMaxValue;
						
						mValue = value;
						mProgressBar.setMax(mMaxValue - mMinValue);
						mProgressBar.setProgress(value - mMinValue);
					}
    			});
    			mDevice.readAndNotify();
    		}
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
        	mBound = false;
        	
        	Log.d(TAG, "The Bluetooth service is no longer bound to this activity.");
        }
    };

    private ProgressBar mProgressBar = null;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	    
	    Log.d(TAG, "Creating DeviceActivity layout...");
	    setContentView(R.layout.layout_device);

	    final Button offButton = (Button)findViewById(R.id.offButton);
	    offButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if(mDevice != null)
					mDevice.send("0");
			}
	    });
	    
	    final Button redButton = (Button)findViewById(R.id.redButton);
	    redButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if(mDevice != null)
					mDevice.send("1");
			}
	    });
	    
	    final Button greenButton = (Button)findViewById(R.id.greenButton);
	    greenButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if(mDevice != null)
					mDevice.send("2");
			}
	    });
	    
	    final Button blueButton = (Button)findViewById(R.id.blueButton);
	    blueButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if(mDevice != null)
					mDevice.send("3");
			}
	    });
	    
	    mProgressBar = (ProgressBar)findViewById(R.id.progressBar);
	    mProgressBar.setMax(1023); // analogRead range
	    mProgressBar.setProgress(mMinValue);
	    
	    final Button minButton = (Button)findViewById(R.id.minButton);
	    minButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				mMinValue = mValue;
				mProgressBar.setMax(mMaxValue - mMinValue);
				mProgressBar.setProgress(mValue - mMinValue);
			}
	    });
	    
	    final Button maxButton = (Button)findViewById(R.id.maxButton);
	    maxButton.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				mMaxValue = mValue;
				mProgressBar.setMax(mMaxValue - mMinValue);
				mProgressBar.setProgress(mValue - mMinValue);
			}
	    });
	    Log.d(TAG, "Created DeviceActivity layout!");
	    
	    mTimer = new Timer();
	    mTimer.scheduleAtFixedRate(new TimerTask() {
			@Override
			public void run() {
				if(mDevice != null)
					mDevice.readAndNotify();
			}
		}, 0, 1000);
	}

	@Override
	protected void onStart() {
		super.onStart();
		Log.d(TAG, "Starting device activity.");
		Intent intent = new Intent(this, BluetoothService.class);
		startService(intent);
		bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
		
		//mDeviceName = getIntent().getExtras().getString("device");
		mDeviceName = getIntent().getStringExtra("device");
		Log.d(TAG, "Controlling device: " + mDeviceName);
	}

	@Override
	protected void onStop() {
		super.onStop();

		// Disconnect the device before the activity goes poof.
		if (mDevice != null) {
			mDevice.disconnect();
		}

		// Unbind from the service
        if (mBound) {
            unbindService(mConnection);
            mBound = false;
        }
	}

}
