package com.example.myapplication;

import android.content.Context;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.ToggleButton;

import androidx.fragment.app.Fragment;

import java.io.File;


public class fg_zhuk extends Fragment implements View.OnClickListener {
    private String tag = "fg_zhuk";
    private ToggleButton connect;
    private Button send;
    private ProgressBar progressBar;
    private static socket socket;
    private boolean isconnecting;
    private boolean isupload;

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
        progressBar = view.findViewById(R.id.progress);
        socket = new socket();
        isconnecting = false;
        isupload = false;
        return view;
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.connect:
                if (connect.isChecked()) {
                    socket.disconnect();
                    progressBar.setVisibility(View.INVISIBLE);
                    isconnecting = false;
                } else {
                    isconnecting = true;
                    progressBar.setVisibility(View.VISIBLE);
                    progressBar.setMax(100);
                    progressBar.setProgress(0);
                    new asyncProgress().execute((String) null);
                    socket.connect(param.zhuk_ip, Integer.parseInt(param.zhuk_port));
                }
                break;
            case R.id.send:
                isupload = true;//定时读取上传文件的进度
                new fileManager().upload(param.zhuk_server, param.file_record);
                if (!param.file_record.isEmpty()) {
                    new asyncProgress().execute((String) null);//创建线程轮询发送文件的长度
                    progressBar.setVisibility(View.VISIBLE);
                    progressBar.setMax((int) new File(param.file_record).length());
                    progressBar.setProgress(0);
                }

                break;
        }
    }

    class asyncProgress extends AsyncTask<String, Integer, String> {

        @Override
        protected String doInBackground(String... strings) {
            int progress = 0;
            try {
                //显示连接socket的进度
                while (isconnecting) {
                    Thread.sleep(100);//连接的时间不超过100*100ms
                    if (socket.isconnect()) {
                        Log.d(tag, "connected");
                        publishProgress(200);//连接完成
                        break;//连接完成后不再轮询
                    }
                    progress++;
                    publishProgress(progress);//每100ms增加1/100的进度
                    socket.connect(param.zhuk_ip, Integer.parseInt(param.zhuk_port));//每100ms连接一次

                }
                while (isupload) {
                    Thread.sleep(10);//每10ms读取一次上传文件的进度
                    publishProgress(fileManager.curLength);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
            return "";
        }

        @Override
        public void onProgressUpdate(Integer... values) {
            //显示连接主控的进度
            if (isconnecting) {
                progressBar.setProgress(values[0]);
                if (values[0] == 100) {//连接超时
                    socket.disconnect();
                    isconnecting = false;
                }
                if (values[0] == 200) {//完成连接
                    isconnecting = false;
                    progressBar.setProgress(100);
                    progressBar.setVisibility(View.INVISIBLE);
                }
            }
            //显示上传文件的进度
            if (isupload) {
                progressBar.setProgress(values[0]);
                if (fileManager.curLength >= fileManager.maxLength) {
                    isupload = false;
                    progressBar.setVisibility(View.INVISIBLE);
                }
            }
        }
    }
}
