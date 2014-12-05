package edu.uccs.omegasensor;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.util.Log;

import java.util.HashMap;
import java.util.Map;
import java.lang.String;

public class SensorHubAndroidService
{
    /// Debug tag.
	private final static String TAG = "SensorHubAndroidService";

    /// Manager for the Bluetooth adapter on the phone/device.
    private BluetoothManager mManager = null;

    /// BLE adapter that handles all communication.
    private BluetoothAdapter mAdapter = null;

    /// Context for this service.
    private Context mContext = null;

    /// Regular expression for the hardware name of a sensor hub.
    private final static String OMEGA_NAME_REGEX = "^Hub[0-9]$";

	/// List of sensor hub devices.
	private Map<String, SensorHubAndroid> mSensorHubs =
		new HashMap<String, SensorHubAndroid>();

	public SensorHubAndroidService(Context context)
    {
        // Get the BLE manager and adapter.
        mContext = context;
        mManager = (BluetoothManager)context.getSystemService(
            Context.BLUETOOTH_SERVICE);
        mAdapter = mManager.getAdapter();

        Log.d(TAG, "Created service.");
    }

    public void StartScan()
    {
		mAdapter.startLeScan(mScanCallback);
    }

    public void StopScan()
    {
        mAdapter.stopLeScan(mScanCallback);
    }

    public void Reload()
    {
        // Do nothing.
    }

    /// Bluetooth Low Energy Scan Callback
    private BluetoothAdapter.LeScanCallback mScanCallback =
        new BluetoothAdapter.LeScanCallback()
    {
        @Override
        public void onLeScan(BluetoothDevice device,
            int rssi, byte[] scanRecord)
        {
			// Get the name of the device.
			String name = device.getName();

			// Make sure the device has a name.
			if(name == null)
				return;

            // Filter the devices by name.
			if(!name.matches(OMEGA_NAME_REGEX))
                return;

            // TODO: Add the service matching code for Android (which is not
            // used right now).

            // Get the hardware address of the hub for lookup.
            String mac = device.getAddress();

			//Log.d(TAG, "Found device: " + name + " / " + mac);

            // Either update or add the hub.
			if(mSensorHubs.containsKey(mac))
            {
                // Get the hub by it's hardware address.
				SensorHubAndroid hub = mSensorHubs.get(mac);

                if(hub != null) // Should never be null.
                {
                    // Notify that the hub was updated.
					JNI.UpdateHub(mac, rssi);
                }
            }
            else
            {
                // Create the sensor hub object.
				SensorHubAndroid hub = new SensorHubAndroid(
					mContext, device);

                // Store the hub.
				mSensorHubs.put(mac, hub);

				// Send the hub to the C++ code.
				JNI.AddHub(hub, name, mac, rssi);
			}

            // Print out some debug info about the sensor hub.
            /*
            std::cout << "Name: " << peripheral->name().toUtf8().constData()
                << " | MAC: " << peripheral->address().toString().toUtf8().constData()
                << " | RSSI: " << rssi << " dBm" << std::endl;
            std::cout << "  Services: "
                << uuids.join(", ").toUtf8().constData() << std::endl;
            */
        }
    };
}
