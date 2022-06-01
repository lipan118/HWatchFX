package com.wiyixiao.hwatchfx.base;

import android.content.Context;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;

import com.wiyixiao.hwatchfx.MyApplication;

public class BaseActivity extends AppCompatActivity {
    protected MyApplication myApplication;
    private BaseActivity oContext;
    private Context mContext;

    public Context getmContext() {
        return mContext;
    }

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (myApplication == null) {
            // 得到Application对象
            myApplication = (MyApplication) getApplication();
        }
        oContext = this;// 把当前的上下文对象赋值给BaseActivity
        mContext = this.getApplicationContext();
        addActivity();// 调用添加方法
    }

    // 添加Activity方法
    public void addActivity() {
        myApplication.addActivity_(oContext);// 调用myApplication的添加Activity方法
    }

    //销毁当个Activity方法
    public void removeActivity() {
        myApplication.removeActivity_(oContext);// 调用myApplication的销毁单个Activity方法
    }

    //销毁所有Activity方法
    public void removeALLActivity() {
        myApplication.removeALLActivity_();// 调用myApplication的销毁所有Activity方法
    }
}
