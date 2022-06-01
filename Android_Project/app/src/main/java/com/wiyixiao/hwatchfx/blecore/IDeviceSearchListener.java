package com.wiyixiao.hwatchfx.blecore;

import com.vise.baseble.model.BluetoothLeDevice;

public interface IDeviceSearchListener {

    /**
     * 设备选择回调
     * @param device 设备
     */
    void deviceSelected(BluetoothLeDevice device);

    /**
     * 取消扫描
     */
    void deviceScanCancel();

}
