package com.wiyixiao.hwatchfx.blecore;

public interface IBleInterface {

    /**
     * 弹出消息
     * @param msg 消息
     */
    void toastMsg(String msg);

    /**
     * 接收数据回调
     * @param data 字节
     */
    void recvCall(byte[] data);

}
