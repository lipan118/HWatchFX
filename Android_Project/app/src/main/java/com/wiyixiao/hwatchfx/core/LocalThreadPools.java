package com.wiyixiao.hwatchfx.core;

import android.content.Context;

import java.util.concurrent.ExecutorService;

public class LocalThreadPools {
    private Context mContext;
    private ExecutorService executorService = new PriorityExecutor(8, false);
    private static LocalThreadPools instance;

    private LocalThreadPools() {

    }

    public static LocalThreadPools getInstance(){
        if(instance == null){
            instance = new LocalThreadPools();
        }

        return instance;
    }

    public void init(Context context){
        this.mContext = context;
        System.out.println("**********LocalThreadPools Init**********");
    }

    public ExecutorService getExecutorService(){
        return executorService;
    }

    public void close(){
        executorService.shutdown();
        instance = null;
        System.out.println("**********LocalThreadPools Exit**********");
    }
}
