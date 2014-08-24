package edu.uccs.omegasensor;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.UUID;

import android.util.Log;

public class BluetoothScanRecord {

	private List<UUID> mServices = new ArrayList<UUID>();
	
	/// Miscellaneous flags.
	private final static int EIR_TYPE_FLAGS = 0x01;
	
	/// Incomplete list of 16-bit service class UUIDs.
	private final static int EIR_TYPE_INCOMPLETE_LIST_UUID16 = 0x02;
	
	/// Complete list of 16-bit service class UUIDs.
	private final static int EIR_TYPE_COMPLETE_LIST_UUID16 = 0x03;
	
	/// Manufacturer Specific Data.
	private final static int EIR_TYPE_MANUFACTURER_DATA = 0xFF;

	public class ParseErrorException extends Exception {
		public ParseErrorException(String message) {
			super(message);
		}
		
		private static final long serialVersionUID = 3420683814578781248L;
	};

	public BluetoothScanRecord(final byte[] scanRecord) throws ParseErrorException {
		int offset = 0;
		
		while(offset < scanRecord.length)
			offset = parseRecord(offset, scanRecord);
	}
	
	private int parseRecord(int offset, final byte[] scanRecord) throws ParseErrorException {
		int recordLength = scanRecord[offset] & 0xFF;
		if(recordLength < 1) {
			// throw new ParseErrorException("Invalid EIR record length.");
			return offset + 1;
		}

		int recordID = scanRecord[offset + 1] & 0xFF;
		Log.d("BluetoothEIR", "Record ID: " + String.format("%02x", recordID));
		
		switch(recordID) {
			case EIR_TYPE_INCOMPLETE_LIST_UUID16:
			{
				String uuid = byteToHexString(Arrays.copyOfRange(
						scanRecord, offset, offset + 16));
				
				Log.d("BluetoothEIR", "Service UUID: " + uuid);
				
				mServices.add(UUID.fromString(uuid));
				
				break;
			}
			case EIR_TYPE_MANUFACTURER_DATA:
			{
				String uuid = byteToHexString(Arrays.copyOfRange(
						scanRecord, offset + 6, offset + 22)).replaceAll(                                            
								"(\\w{8})(\\w{4})(\\w{4})(\\w{4})(\\w{12})",
								"$1-$2-$3-$4-$5");
				
				Log.d("BluetoothEIR", "Service UUID: " + uuid);
				
				mServices.add(UUID.fromString(uuid));
				
				break;
			}
			default:
				break;
		}

		return offset + 1 + recordLength;
	}
	
	private String byteToHexString(final byte[] data) {
		String hexString = "";

		for(byte b : data)
			hexString += String.format("%02x", b);
		
		return hexString;
	}
	
	public final List<UUID> serviceUUIDs() {
		return mServices;
	}

}
