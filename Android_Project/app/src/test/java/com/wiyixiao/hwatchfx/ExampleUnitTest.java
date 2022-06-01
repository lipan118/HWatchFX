package com.wiyixiao.hwatchfx;

import org.junit.Test;

import static org.junit.Assert.*;

import com.wiyixiao.hwatchfx.utils.Utils;

import java.util.Arrays;
import java.util.Calendar;

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
public class ExampleUnitTest {
    @Test
    public void addition_isCorrect() {
        assertEquals(4, 2 + 2);
    }

    @Test
    public void timeGetTest(){
        StringBuilder builder = new StringBuilder();
        Calendar calendar = Calendar.getInstance();
        builder.append(calendar.get(Calendar.YEAR)).append("/");
        builder.append(calendar.get(Calendar.MONTH) + 1).append("/");
        builder.append(calendar.get(Calendar.DAY_OF_MONTH)).append(" ");
        builder.append(calendar.get(Calendar.HOUR_OF_DAY)).append(":");
        builder.append(calendar.get(Calendar.MINUTE)).append(":");
        builder.append(calendar.get(Calendar.SECOND)).append(" ");
        builder.append(calendar.get(Calendar.DAY_OF_WEEK));

        System.out.println(builder);
    }

    @Test
    public void envDataParseTest(){
        byte[] humi = {0x00, 0x00, (byte) 0x97, 0x42}; //75.5
        byte[] temp = {0x00, 0x00, 0x0e, 0x42}; //35.5
        byte[] press = {0x00, 0x60, 0x7b, 0x44}; //1005.5

        byte[] data = new byte[38];
        data[0] = (byte) 0xff;
        data[1] = (byte) 0xff;
        data[2] = 0x10;
        System.arraycopy(temp, 0, data, 3, temp.length);
        System.arraycopy(humi, 0, data, 7, humi.length);
        System.arraycopy(press, 0, data, 11, press.length);
        data[36] = 0x0d;
        data[37] = 0x0a;

        System.out.println(Utils.bytesToHex(data));
        CommManager commManager = new CommManager();
        commManager.parseEnvData(data);
    }
}