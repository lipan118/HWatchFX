package com.wiyixiao.hwatchfx;

import android.app.Activity;
import android.content.Context;
import android.util.Log;

import com.vise.log.ViseLog;
import com.vise.log.inner.LogcatTree;
import com.wiyixiao.hwatchfx.base.ApplicationBase;
import com.wiyixiao.hwatchfx.blecore.BleManager;
import com.wiyixiao.hwatchfx.core.LocalThreadPools;

import java.util.ArrayList;
import java.util.List;

public class MyApplication extends ApplicationBase {

    private List<Activity> oList;//用于存放所有启动的Activity的集合

    @Override
    public void onCreate() {
        super.onCreate();

        //初始化日志打印
        ViseLog.getLogConfig()
                .configAllowLog(true)//是否输出日志
                .configShowBorders(true)//是否排版显示
                .configTagPrefix("ViseLog")//设置标签前缀
                .configFormatTag("%d{HH:mm:ss:SSS} %t %c{-5}")//个性化设置标签，默认显示包名
                .configLevel(Log.VERBOSE);//设置日志最小输出级别，默认Log.VERBOSE
        ViseLog.plant(new LogcatTree());//添加打印日志信息到Logcat的树

        //初始化BLE蓝牙实例
        BleManager.getInstance().init(this);

        //初始化线程池
        LocalThreadPools.getInstance().init(this);
    }

    /**
     * 添加Activity
     */
    public void addActivity_(Activity activity) {

        if(oList == null){
            oList = new ArrayList<Activity>();
        }

        // 判断当前集合中不存在该Activity
        if (!oList.contains(activity)) {
            oList.add(activity);//把当前Activity添加到集合中
        }
    }

    /**
     * 销毁单个Activity
     */
    public void removeActivity_(Activity activity) {
        //判断当前集合中存在该Activity
        if (oList.contains(activity)) {
            oList.remove(activity);//从集合中移除
            activity.finish();//销毁当前Activity
        }
    }

    /**
     * 销毁所有的Activity
     */
    public void removeALLActivity_() {
        //通过循环，把集合中的所有Activity销毁
        for (Activity activity : oList) {
            activity.finish();
        }

        oList.clear();
        oList = null;
    }
}
