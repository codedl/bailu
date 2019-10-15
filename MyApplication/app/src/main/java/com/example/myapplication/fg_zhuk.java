package com.example.myapplication;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import androidx.fragment.app.Fragment;


public class fg_zhuk extends Fragment implements View.OnClickListener {
    private Button connect;
    private Button send;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup group, Bundle bundle) {
        //inflate:加载布局文件
        //@int resource:需要加载的布局
        //@ViewGroup root:布局所在的根布局
        //boolean attachToRoot:是否将root作为根对象返回
        View view = inflater.inflate(R.layout.zhuk, group, false);
        connect = view.findViewById(R.id.connect);
        send = view.findViewById(R.id.send);
        connect.setOnClickListener(this);
        send.setOnClickListener(this);
        return view;
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.connect:
                System.out.println("connect");
                break;
            case R.id.send:
                System.out.println("send");
                break;
        }
    }
}
