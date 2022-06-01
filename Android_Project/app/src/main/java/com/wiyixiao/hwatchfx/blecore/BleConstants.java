package com.wiyixiao.hwatchfx.blecore;

public class BleConstants {

    /**
     * 修改mtu大小开关
     */
    public static final boolean changeMtuEnable = true;

    /**
     * 状态码
     */
    public static final int CODE_BLE_RECV = 0x01;
    public static final int CODE_GPS_REQUEST = 0x02;
    public static final int CODE_BLE_SEND = 0x03;

    /**
     * 发送最大长度
     * 244
     */
    public static final int SEND_LEN_MAX = 244;

    /**
     * 发送最小长度
     * 20
     */
    public static final int SEND_LEN_MIN = 20;

}
