package edu.uccs.omegasensor;

import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

public class MainActivity extends Activity {

	protected BluetoothManager mManager = null;
	protected BluetoothAdapter mAdapter = null;
	
	// ------------------------------------------------------------------------
	// Bluetooth Service Enable Notification
	// ------------------------------------------------------------------------

	/// Dialog saying the application will exit since BLE was not enabled.
	private AlertDialog mExitDialog = null;

	/// If the user has been prompted to enable BLE.
	private boolean mDidShowBTRequest = false;

	/// Request code for the enable BLE intent.
	private final static int REQUEST_ENABLE_BT = 1;
		
	private final static String TAG = "MainActivity";

	protected void checkBluetooth() {
		// If we have not shown a request and Bluetooth still isn't enabled, show
		// another request to enable Bluetooth.
		if(!mDidShowBTRequest && (mAdapter == null || !mAdapter.isEnabled())) {
	    	Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
	    	startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
	    	
	    	// Let us know the request is visible so we don't display it a 2nd time.
	    	mDidShowBTRequest = true;
	    }
	}

	@Override
	protected void onStart() {
		super.onStart();
		
		checkBluetooth();
	}
	
	@Override
	protected void onStop() {
		super.onStart();
		
		if(mExitDialog != null) {
			mExitDialog.dismiss();
			mExitDialog = null;
		}
	}

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);
	
	    mManager = (BluetoothManager)getSystemService(Context.BLUETOOTH_SERVICE);
	    mAdapter = mManager.getAdapter();
	}
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if(requestCode == REQUEST_ENABLE_BT) {
			// The user responded to the Bluetooth enable request. If the app comes to
			// focus again, we should request again if Bluetooth still isn't enabled.
			mDidShowBTRequest = false;
		}

		if(requestCode == REQUEST_ENABLE_BT && resultCode != -1) {
			Log.d(TAG, "User did not enable BLE! (resultCode=" + resultCode + ")");
			
			AlertDialog.Builder builder = new AlertDialog.Builder(this);
			builder.setCancelable(false);
			builder.setTitle("Bluetooth Disabled");
			builder.setIcon(R.drawable.ic_launcher);
			builder.setMessage("You must enable Bluetooth to use this application. " +
				"Since you did not enable Bluetooth, the application will now close.");
			builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					android.os.Process.killProcess(android.os.Process.myPid());
					System.exit(0);
				}
			});

			if(mExitDialog != null) {
				mExitDialog.dismiss();
			}
			
			mExitDialog = builder.create();
			mExitDialog.show();
		}
		
		super.onActivityResult(requestCode, resultCode, data);
	}

}
