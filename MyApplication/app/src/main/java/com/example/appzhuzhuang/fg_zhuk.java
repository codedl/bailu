package com.example.appzhuzhuang;

import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.ToggleButton;

import androidx.fragment.app.Fragment;

import java.io.File;
import java.util.Iterator;


public class fg_zhuk extends Fragment implements View.OnClickListener {
    private String tag = "fg_zhuk";
    private static ToggleButton connect;
    private static Button send;
    private ProgressBar progressBar;
    public static socket socket;
    private boolean isconnecting;
    private boolean isupload;
    private TextView txt;
    private TextView zhuk;

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
        txt = view.findViewById(R.id.txt);
        progressBar = view.findViewById(R.id.progress);
        socket = new socket();
        isconnecting = false;
        isupload = false;
        zhuk = getActivity().findViewById(R.id.txt_zhukong);
        return view;
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.connect:
                if (connect.isChecked()) {
                    socket.disconnect();
                    progressBar.setVisibility(View.INVISIBLE);
                    txt.setVisibility(View.INVISIBLE);
                    isconnecting = false;
                } else {
                    isconnecting = true;
                    txt.setVisibility(View.VISIBLE);
                    txt.setText(param.zhuk_ip + ":" + param.zhuk_port);
                    progressBar.setVisibility(View.VISIBLE);
                    progressBar.setMax(100);
                    progressBar.setProgress(0);
                    new asyncProgress().execute((String) null);
                    socket.connect(param.zhuk_ip, Integer.parseInt(param.zhuk_port));
                }
                break;
            case R.id.send:
                if (param.file_record.size() > 0) {//判断是否有需要发送的文件
                    Iterator<String> files = param.file_record.iterator();//迭代文件列表
                    while (files.hasNext()) {
                        String file = files.next();
                        isupload = true;//定时读取上传文件的进度
                        new fileManager().upload(param.zhuk_server, file);//上传文件
                        new asyncProgress().execute((String) null);//异步轮询发送文件的进度
                        progressBar.setVisibility(View.VISIBLE);
                        txt.setVisibility(View.VISIBLE);
                        txt.setText(new File(file).getName());
                        progressBar.setMax((int) new File(file).length());
                        progressBar.setProgress(0);
                        MainActivity.listFiles.remove(new File(file).getName());

                    }
                    param.file_record.clear();//清除已经上传选中的文件
                    MainActivity.adapter.notifyDataSetChanged();
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
                txt.setText(param.zhuk_ip + ":" + param.zhuk_port + "(" + values[0] + "%)");
                if (values[0] == 100) {//连接超时
                    socket.disconnect();
                    isconnecting = false;
                    connect.setText("服务器异常");
                }
                if (values[0] == 200) {//完成连接
                    isconnecting = false;
                    progressBar.setProgress(100);
                    progressBar.setVisibility(View.INVISIBLE);
                    txt.setVisibility(View.INVISIBLE);
                }
            }
            //显示上传文件的进度
            if (isupload) {
                progressBar.setProgress(values[0]);
                if (fileManager.curLength >= fileManager.maxLength) {
                    isupload = false;
                    progressBar.setVisibility(View.INVISIBLE);
                    txt.setVisibility(View.INVISIBLE);
                }
            }
        }
    }

    static void sendAll() {
        File file = new File(param.pathRecord);
        String[] files = file.list();//遍历列表，选中所有待发送文件
        param.file_record.clear();
        for (int i = 0; i < files.length; i++) {
            param.file_record.add(param.pathRecord + files[i]);
            Log.d("fg_zhuk", files[i]);
        }
        send.performClick();//触发send点击事件发送文件
    }

    static void setConnectClick(boolean isConnected) {
        connect.setChecked(isConnected);
        connect.performClick();
    }
}
