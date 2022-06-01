package com.wiyixiao.hwatchfx.dialog;

import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;

import com.vise.baseble.model.BluetoothLeDevice;
import com.wiyixiao.hwatchfx.R;
import com.wiyixiao.hwatchfx.adapter.DeviceAdapter;
import com.wiyixiao.hwatchfx.blecore.IDeviceSearchListener;

import java.util.ArrayList;

public class DeviceDialog extends AlertDialog {

    private ListView deviceLv;

    private DeviceAdapter deviceAdapter;
    private ArrayList<BluetoothLeDevice> devicesList = new ArrayList<>();
    private IDeviceSearchListener deviceSearchListener;

    private AdapterView.OnItemClickListener onItemClickListener = new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
            if(deviceSearchListener != null){
                deviceSearchListener.deviceSelected(devicesList.get(i));
                dismiss();
            }
        }
    };

    public void setDeviceSearchListener(IDeviceSearchListener deviceSearchListener) {
        this.deviceSearchListener = deviceSearchListener;
    }

    public DeviceDialog(@NonNull Context context) {
        super(context);
        deviceAdapter = new DeviceAdapter(context, devicesList);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.dialog_device_popupview);

        deviceLv = findViewById(R.id.device_lv);
        deviceLv.setAdapter(deviceAdapter);
        deviceLv.setOnItemClickListener(onItemClickListener);
    }

    @Override
    public void dismiss() {
        super.dismiss();
        if(devicesList.size() > 0) {
            devicesList.clear();
            deviceAdapter.notifyDataSetChanged();
        }
    }

    public void addDevice(BluetoothLeDevice device){
        boolean ishave = false;
        if(!devicesList.contains(device)){

            for (BluetoothLeDevice de: devicesList
            ) {
                if(de.getAddress().equals(device.getAddress())){
                    ishave = true;
                    break;
                }
            }

            if(!ishave){
                devicesList.add(device);
                deviceAdapter.notifyDataSetChanged();
            }
        }
    }
}
