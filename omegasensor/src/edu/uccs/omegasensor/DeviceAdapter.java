package edu.uccs.omegasensor;

import java.util.List;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public class DeviceAdapter extends ArrayAdapter<DemoDevice> {

	public DeviceAdapter(Context context, List<DemoDevice> objects) {
		super(context, R.layout.layout_devicelist, objects);
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		DemoDevice dev = getItem(position);
		
		if(convertView == null) {
			convertView = LayoutInflater.from(getContext()).inflate(
				R.layout.layout_devicelist, parent, false);
		}
		
		TextView name = (TextView)convertView.findViewById(R.id.deviceList_Name);
		TextView address = (TextView)convertView.findViewById(R.id.deviceList_Address);
		
		name.setText(dev.name());
		address.setText(dev.macAddress());
		
		return convertView;
	}

}
