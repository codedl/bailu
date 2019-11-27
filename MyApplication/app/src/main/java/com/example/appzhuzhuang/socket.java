package com.example.appzhuzhuang;

import android.os.Message;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.SocketException;

public class socket {
    private String tag = "socket";
    private Socket socket;
    private OutputStream output;
    private InputStream input;
    private byte buf[] = new byte[1024];
    private int count = 0;
    private boolean isOnline;//手动判断是否连接成功

    public socket() {

    }

    public synchronized void connect(final String ip, final int port) {
        new Thread() {
            @Override
            public void run() {
                try {
                    isOnline = false;
                    socket = new Socket(ip, port);
                    if (socket != null) {
                        input = socket.getInputStream();
                        output = socket.getOutputStream();
                        if (input != null && output != null) {
                            isOnline = true;//连接完成
                            new Thread() {//创建一个线程定时轮询连接状态
                                @Override
                                public synchronized void run() {
                                    try {
                                        while (isOnline) {
                                            sleep(3000);
                                            output.write("T".getBytes());//定时写数据判断连接是否断开
                                            output.flush();
                                        }
                                    } catch (Exception e) {
                                        //连接断开后重新执行连接
                                        if (isOnline) {
                                            Message msg = param.uiHandler.obtainMessage();
                                            msg.what = 0x01;
                                            param.uiHandler.sendMessage(msg);
                                        }
                                        e.printStackTrace();
                                    }
                                }
                            }.start();
                        }
                    }

                    while (socket.isConnected() && !socket.isClosed()) {
                        sleep(100);
                        try {
                            if ((count = input.read(buf)) > 0) {
                                Log.d(tag, "Receive: " + ClsUtils.toHexString(buf, 0, count));
                                procotol.rcvProcess(buf, count);
                            }
                        } catch (Exception e) {//read异常认为连接断开
                            if (isOnline) {//如果不是手动断开连接需要自动重连
                                Message msg = param.uiHandler.obtainMessage();
                                msg.what = 0x01;
                                param.uiHandler.sendMessage(msg);
                            }
                            e.printStackTrace();
                        }

                    }

                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();
    }

    public void disconnect() {
        isOnline = false;
        if (socket != null) {
            try {
                socket.close();
                if (input != null)
                    input.close();
                if (output != null)
                    output.close();

            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public boolean isconnect() {
        if (socket != null)
            return socket.isConnected() && isOnline;

        return false;
    }

    public void send(final String str) {
        new Thread() {
            @Override
            public synchronized void run() {
                if (isconnect()) {
                    try {
                        output.write(str.getBytes("UTF-8"));
                        output.flush();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }.start();

    }

    public void send(final byte buf[]) {
        new Thread() {
            @Override
            public synchronized void run() {
                if (socket != null && socket.isConnected() && output != null) {
                    try {
                        output.write(buf);
                        output.flush();
                        Log.d(tag, "Send: " + ClsUtils.toHexString(buf));
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }.start();

    }

}
