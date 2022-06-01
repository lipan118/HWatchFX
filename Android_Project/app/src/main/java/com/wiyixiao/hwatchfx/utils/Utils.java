package com.wiyixiao.hwatchfx.utils;

import static android.content.ContentValues.TAG;

import android.app.Activity;
import android.os.Build;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

public class Utils {

    public static int getByte(byte[] b){
        int n = 0;
        for(int i=0;i<b.length;i++){
            n |= b[i] << i;
        }

        return n;
    }

    public static int bytesToInt(byte[] b){
        String s = new String(b);
        return Integer.parseInt(s);
    }

    //字节转十六进制
    public static String byteToHex(byte b) {
        String hex = Integer.toHexString(b & 0xFF);
        if (hex.length() < 2) {
            hex = "0" + hex;
        }
        return hex;
    }

    //字节数组转字符串
    public static String bytesToHex(byte[] bytes) {
        StringBuffer sbuffer = new StringBuffer();
        for (int i = 0; i < bytes.length; i++) {
            String hex = Integer.toHexString(bytes[i] & 0xFF);
            sbuffer.append("0x");
            if (hex.length() < 2) {
                sbuffer.append(0);
            }
            sbuffer.append(hex);
            sbuffer.append(",");
        }
        return sbuffer.toString();
    }

    public static void initStatusBarColor(Activity activity, int colorId)
    {
        //Android4.4以上系统版本可以修改状态栏颜色，但是只有小米的MIUI、魅族的Flyme
        //和Android6.0以上系统可以把状态栏文字和图标换成深色，其他的系统状态栏文字都是白色的，换成浅色背景的话就看不到了
        Window window = activity.getWindow();
        int sdkInt = Build.VERSION.SDK_INT;
        if (sdkInt >= Build.VERSION_CODES.M) {
            Log.e(TAG, "系统版本大于6.0，可修改状态栏图标为黑色");
            window.getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);
        }else if(sdkInt >= Build.VERSION_CODES.LOLLIPOP)
        {
            Log.e(TAG, "系统版本大于5.0小于6.0，可修改状态栏颜色");
            window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
            window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            window.setStatusBarColor(activity.getResources().getColor(colorId));
        }else{
            Log.e(TAG, "系统版本低于4.4，不支持状态栏修改");
        }

    }

    public static int reverseBits(int n, boolean inverse) {
        int res = 0;
        for (int i = 0; i < 8; i ++) {
            res = (res << 1) + (n & 1);
            n >>= 1;
        }

        if(inverse)
            res = ~res;

        return res;
    }

    public static byte[] int2bytes(int n){
        byte[] temp = new byte[2];
        temp[0] = (byte)(n & 0xff);
        temp[1] = (byte) (n >> 8);
        return temp;
    }
}
