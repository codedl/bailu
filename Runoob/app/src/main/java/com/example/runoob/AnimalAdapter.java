package com.example.runoob;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import java.util.LinkedList;

public class AnimalAdapter extends BaseAdapter {
    private LinkedList<animal> mData;
    private Context context;
    private viewHolder holder = null;

    public AnimalAdapter(LinkedList<animal> data, Context context) {
        this.mData = data;
        this.context = context;
    }

    @Override
    public int getCount() {
        return mData.size();
    }

    @Override
    public Object getItem(int i) {
        return null;
    }

    @Override
    public long getItemId(int i) {
        return i;
    }

    //创建listview的列表项
    @Override
    public View getView(int i, View view, ViewGroup viewGroup) {
        if (view == null) {
            view = LayoutInflater.from(context).inflate(R.layout.item_list_animal, viewGroup, false);
            holder = new viewHolder();
            holder.txt_name = view.findViewById(R.id.txt_name);
            holder.txt_speak = view.findViewById(R.id.txt_speak);
            view.setTag(holder);
        } else {
            view.getTag();
        }

        holder.txt_name.setText(mData.get(i).getName());
        holder.txt_speak.setText(mData.get(i).getSpeak());
        return view;
    }

    static class viewHolder {
        TextView txt_name;
        TextView txt_speak;
    }

    public void add(animal data) {
        if (this.mData == null) {
            this.mData = new LinkedList<animal>();
        }
        mData.add(data);
        notifyDataSetChanged();
    }

    public void add(int postion, animal data) {
        if (this.mData == null) {
            mData = new LinkedList<animal>();
        }
        mData.add(postion, data);
        for (int i = 0; i < mData.size(); i++)
            System.out.println(mData.get(i).getName() + mData.get(i).getSpeak());
        notifyDataSetChanged();
    }
}
