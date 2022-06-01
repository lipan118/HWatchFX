package com.wiyixiao.hwatchfx.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.wiyixiao.hwatchfx.R;
import com.wiyixiao.hwatchfx.bean.AlarmBean;

import java.util.ArrayList;

public class AlarmListAdapter extends BaseAdapter {

    private Context mContext;
    private ArrayList<AlarmBean> list;

    public AlarmListAdapter(Context mContext, ArrayList<AlarmBean> list) {
        this.mContext = mContext;
        this.list = list;
    }

    @Override
    public int getCount() {
        return list.size();
    }

    @Override
    public Object getItem(int i) {
        return list.get(i);
    }

    @Override
    public long getItemId(int i) {
        return i;
    }

    @Override
    public View getView(int i, View view, ViewGroup viewGroup) {
        if (view==null){
            view= LayoutInflater.from(mContext).inflate(R.layout.item_alarm_list,null);
        }

        StringBuilder builder = new StringBuilder();
        AlarmBean bean = list.get(i);
        TextView alarmTv = view.findViewById(R.id.alarm_item_tv);

        builder.append(bean.getId()).append(" ");
        builder.append(bean.getWeek()).append(" ");
        builder.append(bean.getHour()).append(":");
        builder.append(bean.getMin());

        alarmTv.setText(builder);

        return view;
    }
}
