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
