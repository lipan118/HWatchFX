package com.wiyixiao.hwatchfx.utils;

import android.annotation.SuppressLint;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Author:Think
 * Time:2021/2/3 16:58
 * Description:This is IEEE754
 */
public class IEEE754 {
    /**
     * @Desc: IEEE754标准（四字节转浮点数）调用JDK接口转换
     */
    public static float hex2floatIeeeApi(byte[] bytes, boolean reverse){
        float result;
        if(reverse){
            result = ByteBuffer.wrap(bytes).order(ByteOrder.BIG_ENDIAN).getFloat();
        }

        result = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN).getFloat();
        return result;
    }

    /**
     * @Desc: IEEE754标准（浮点数转四字节）调用JDK接口转换
     */
    public static byte[] float2hexIeeeApi(float f){
        int intBits = Float.floatToRawIntBits(f);
        System.out.println(intBits);
        return intToByteArray(intBits);
    }

    public static byte[] intToByteArray(int i) {
        byte[] result = new byte[4];
        // 由高位到低位
        result[0] = (byte) ((i >> 24) & 0xFF);
        result[1] = (byte) ((i >> 16) & 0xFF);
        result[2] = (byte) ((i >> 8) & 0xFF);
        result[3] = (byte) (i & 0xFF);
        return result;
    }

}
