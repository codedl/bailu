package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import java.net.Socket;

public class socketActivity extends AppCompatActivity {

    handle socketHandle;
    OutputStream os;
    InputStream is;

    EditText ipEdit;
    EditText portEdit;
    EditText sendEdit;
    EditText rcvEdit;
    Switch socketSwitch;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_socket);
        socketInit();

    }

    public void socketInit() {
        ipEdit = (EditText) findViewById(R.id.ip_edit);
        portEdit = (EditText) findViewById(R.id.port_edit);
        sendEdit = (EditText) findViewById(R.id.send_edit);
        rcvEdit = (EditText) findViewById(R.id.rcv_edit);
        socketSwitch = (Switch) findViewById(R.id.switch_switch);

        socketHandle = new handle();
    }

    public class handle extends Handler {
        String handleStr;

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case 1:
                    System.out.println("connect succeed");
                    Toast.makeText(socketActivity.this, "连接成功", Toast.LENGTH_SHORT).show();
                    break;
                case 2:
                    handleStr = (String) msg.obj;
                    rcvEdit.setText(handleStr);
                    String sendstr = null;
                    if ("on".equals(handleStr)) {
                        Toast.makeText(socketActivity.this, "开启驻装" + msg.arg1, Toast.LENGTH_SHORT).show();
                        sendstr = "onok";
                    } else if ("off".equals(handleStr)) {
                        sendstr = "offok";
                        Toast.makeText(socketActivity.this, "关闭驻装" + msg.arg1, Toast.LENGTH_SHORT).show();
                    }
                    sendEdit.setText(sendstr);
                    new send(sendstr).start();

                    break;
            }
        }
    }

    public void socketEvent(View btn) {
        switch (btn.getId()) {
            case R.id.switch_switch:
                if (socketSwitch.isChecked()) {
                    if (ipEdit.getText().toString().trim().length() != 0) {
                        param.ServerHostIP = ipEdit.getText().toString().trim();
                    } else {
                        ipEdit.setText(param.ServerHostIP);
                    }
                    if (portEdit.getText().toString().trim().length() != 0) {
                        String port = portEdit.getText().toString().trim();
                        param.ServerHostPort = Integer.parseInt(port);
                    } else {
                        portEdit.setText(String.valueOf(param.ServerHostPort));
                    }

                    if (param.clientSocket != null) {
                        Toast.makeText(this, "已经连接", Toast.LENGTH_SHORT).show();
                    } else {
                        new client().start();
                        Toast.makeText(this, "正在连接", Toast.LENGTH_LONG).show();
                    }
                } else {
                    if (param.clientSocket != null) {
                        try {
                            param.clientSocket.close();
                            param.clientSocket = null;
                        } catch (Exception e) {
                            Toast.makeText(this, "断开异常", Toast.LENGTH_LONG).show();
                            e.printStackTrace();
                        }
                        Toast.makeText(this, "断开连接", Toast.LENGTH_LONG).show();
                    }
                }
                break;

            case R.id.send_btn:
                if (param.clientSocket != null) {
                    if (sendEdit.getText().toString().trim().length() != 0) {
                        String str = sendEdit.getText().toString().trim();
                        new send(str).start();
                        Toast.makeText(this, "发送成功", Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(this, "输入不能为空", Toast.LENGTH_SHORT).show();
                    }
                } else {
                    Toast.makeText(this, "需要连接服务器", Toast.LENGTH_SHORT).show();
                }
//                startService(new Intent( getBaseContext(), socketService.class));
                break;
            case R.id.rcv_btn:
//                stopService(new Intent( getBaseContext(), socketService.class));
                break;
        }
    }

    public class client extends Thread {
        String str;
        Message msg;
        byte buf[] = new byte[1024];

        @Override
        public void run() {
            try {
                param.clientSocket = new Socket(param.ServerHostIP, param.ServerHostPort);

            } catch (IOException e) {
                e.printStackTrace();
            }
            if (param.clientSocket != null) {
                msg = socketHandle.obtainMessage();
                msg.what = 1;
                socketHandle.sendMessage(msg);
            }
            try {
                is = param.clientSocket.getInputStream();
                os = param.clientSocket.getOutputStream();

            } catch (Exception e) {
                e.printStackTrace();
            }
            int count = 0;
            while (true) {
                try {
                    sleep(100);
                    count = is.read(buf);
                    if (count > 0) {
                        str = new String(buf, 0, count);
                        msg = socketHandle.obtainMessage();
                        msg.obj = str;
                        msg.arg1 = count;
                        msg.what = 2;
                        socketHandle.sendMessage(msg);
                    }

                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public class send extends Thread {
        private String sendsStr;

        public send(String str) {
            this.sendsStr = str;
        }

        @Override
        public void run() {
            if (param.clientSocket != null) {
                try {
                    os.write(sendsStr.getBytes("UTF-8"));
                    os.flush();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

    protected void onDestroy() {
        super.onDestroy();//解除注册
    }
}
