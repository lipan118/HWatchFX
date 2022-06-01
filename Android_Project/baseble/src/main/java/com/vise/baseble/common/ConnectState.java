package com.vise.baseble.common;

/**
 * @Description: 状态描述
 * @author: <a href="http://www.xiaoyaoyou1212.com">DAWI</a>
 * @date: 16/8/20 17:06.
 */
public enum ConnectState {
    CONNECT_INIT(-1),           //连接初始化
    CONNECT_PROCESS(0x00),      //连接中
    CONNECT_SUCCESS(0x01),      //连接成功
    CONNECT_FAILURE(0x02),      //连接失败
    CONNECT_TIMEOUT(0x03),      //连接超时
    CONNECT_DISCONNECT(0x04);   //连接断开

    private int code;

    ConnectState(int code) {
        this.code = code;
    }

    public int getCode() {
        return code;
    }
}
