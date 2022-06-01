package com.wiyixiao.hwatchfx.bean;

import java.io.Serializable;

public class TimeBean implements Serializable {
    private int year;
    private int month;
    private int day;
    private int hour;
    private int min;
    private int sec;
    private int week;
    private String weekStr[] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};

    public int getYear() {
        return year;
    }

    public void setYear(int year) {
        this.year = year;
    }

    public int getMonth() {
        return month;
    }

    public void setMonth(int month) {
        this.month = month;
    }

    public int getDay() {
        return day;
    }

    public void setDay(int day) {
        this.day = day;
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

    public int getSec() {
        return sec;
    }

    public void setSec(int sec) {
        this.sec = sec;
    }

    public int getWeek() {
        return week;
    }

    public void setWeek(int week) {
        this.week = week;
    }

    public String getWeekStr(){
        return weekStr[week];
    }
}
