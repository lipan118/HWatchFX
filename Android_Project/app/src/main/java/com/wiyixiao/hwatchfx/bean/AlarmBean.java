package com.wiyixiao.hwatchfx.bean;

import java.io.Serializable;

public class AlarmBean implements Serializable {
    private int id;
    private int week;
    private int hour;
    private int min;

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getWeek() {
        return week;
    }

    public void setWeek(int week) {
        this.week = week;
    }

    public int getHour() {
        return hour;
    }

    public void setHour(int hour) {
        this.hour = hour;
    }

    public int getMin() {
        return min;
    }

    public void setMin(int min) {
        this.min = min;
    }


}
