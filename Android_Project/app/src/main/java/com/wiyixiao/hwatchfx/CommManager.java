package com.wiyixiao.hwatchfx;

import com.vise.log.ViseLog;
import com.wiyixiao.hwatchfx.bean.EnvBean;
import com.wiyixiao.hwatchfx.bean.TimeBean;
import com.wiyixiao.hwatchfx.utils.IEEE754;

import java.util.Arrays;

public class CommManager {
    private int frame_len = 38;
    private byte[] frame_head = {(byte) 0xff, (byte) 0xff};
    private byte[] frame_tail = {0x0d, 0x0a};

    private EnvBean envBean = new EnvBean();

    public EnvBean getEnvBean() {
        return envBean;
    }

    public interface CommState{
        byte state_bit = 2;
        byte conn = 0x00;
        byte alarm_get = 0x01;
        byte tim_set = 0x02;
        byte alarm_set = 0x03;
        byte env_upload = 0x10;
        byte alarm_upload = 0x11;
    }

    public byte[] getConnBytes(){
        byte[] tmp = new byte[frame_len];
        int index = 0;
        System.arraycopy(frame_head, 0, tmp, index, frame_head.length);
        index += frame_head.length;
        tmp[index++] = CommState.conn;
        index = frame_len - 2;
        System.arraycopy(frame_tail, 0, tmp, index, frame_tail.length);
        return tmp;
    }

    public byte[] getTimeBytes(TimeBean timeBean){
        byte[] tmp = new byte[frame_len];
        int index = 0;
        System.arraycopy(frame_head, 0, tmp, index, frame_head.length);
        index += frame_head.length;
        tmp[index++] = CommState.tim_set;
        tmp[index++] = getByte((timeBean.getYear()-2000));
        tmp[index++] = getByte(timeBean.getMonth());
        tmp[index++] = getByte(timeBean.getDay());
        tmp[index++] = getByte(timeBean.getHour());
        tmp[index++] = getByte(timeBean.getMin());
        tmp[index++] = getByte(timeBean.getSec());
        tmp[index++] = getByte(timeBean.getWeek());
        index = frame_len - 2;
        System.arraycopy(frame_tail, 0, tmp, index, frame_tail.length);
        return tmp;
    }

    public byte checkData(byte[] data){
        byte state = (byte) 0xff;

        do{
            if(data.length != frame_len) break;

            byte[] head = new byte[2];
            byte[] tail = new byte[2];
            System.arraycopy(data, 0, head, 0, head.length);
            System.arraycopy(data, data.length - 2, tail, 0, tail.length);
            if(!Arrays.equals(head, frame_head) || !Arrays.equals(tail, frame_tail)) break;

            state = data[CommState.state_bit];
            break;

        }while (true);

        return state;
    }

    public void parseEnvData(byte[] data){
        byte[] result = new byte[4];
        int index = CommState.state_bit+1;
        System.arraycopy(data, index, result, 0, result.length);
        envBean.setTemp(IEEE754.hex2floatIeeeApi(result, false));
        index+=result.length;
        System.arraycopy(data, index, result, 0, result.length);
        envBean.setHumi(IEEE754.hex2floatIeeeApi(result, false));
        index+=result.length;
        System.arraycopy(data, index, result, 0, result.length);
        envBean.setPress(IEEE754.hex2floatIeeeApi(result, false) / 100.0f); //hPa

        System.out.println(envBean.getTemp() + " " + envBean.getHumi() + " " + envBean.getPress());
    }

    private byte getByte(int num){
        return (byte) (num & 0xff);
    }
}
