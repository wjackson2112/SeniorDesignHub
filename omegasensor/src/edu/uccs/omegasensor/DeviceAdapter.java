package edu.uccs.omegasensor;

import java.util.List;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public class DeviceAdapter extends ArrayAdapter<ParticleDevice> {

	public DeviceAdapter(Context context, List<ParticleDevice> objects) {
		super(context, R.layout.layout_devicelist, objects);
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		ParticleDevice dev = getItem(position);
		
		if(convertView == null) {
			convertView = LayoutInflater.from(getContext()).inflate(R.layout.layout_devicelist, null);
		}
		
		TextView name = (TextView)convertView.findViewById(R.id.deviceList_Name);
		TextView address = (TextView)convertView.findViewById(R.id.deviceList_Address);
		
		name.setText(dev.name());
		address.setText(dev.macAddress());
		
		return convertView;
	}

}
