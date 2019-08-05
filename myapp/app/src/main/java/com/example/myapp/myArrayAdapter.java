package com.example.myapp;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import androidx.annotation.NonNull;

public class myArrayAdapter extends ArrayAdapter<String> {

    private Context context;
    private String[] strings;

    public myArrayAdapter(@NonNull Context context, int resource) {
        super(context, resource);
    }

    public myArrayAdapter(Context context, String[] strings) {
        super(context, android.R.layout.simple_list_item_1, strings);
        this.context = context;
        this.strings = strings;
    }

    @Override
    public View getDropDownView(int position, View view, ViewGroup group) {
        if (view == null) {
            LayoutInflater layoutInflater = LayoutInflater.from(this.context);
            view = layoutInflater.inflate(android.R.layout.simple_spinner_dropdown_item, group, false);
        }
        TextView textView = (TextView) view.findViewById(R.id.spin_text);
        textView.setText(strings[position]);
        textView.setTextSize(40f);
        return view;
    }

    @Override
    public View getView(int position, View view, ViewGroup viewGroup) {
        if (view == null) {
            LayoutInflater layoutInflater = LayoutInflater.from(context);
            view = layoutInflater.inflate(android.R.layout.simple_spinner_item, viewGroup, false);
        }
        TextView textView = view.findViewById(R.id.spin_text);
        textView.setText(strings[position]);
        textView.setTextSize(40f);
        return view;
    }
}
