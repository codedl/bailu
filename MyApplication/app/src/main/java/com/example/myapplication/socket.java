package com.example.myapplication;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

public class socket {
    private Socket socket;
    private OutputStream output;
    private InputStream input;
    private byte buf[] = new byte[1024];
    private int count = 0;
    public socket() {

    }

    public void connect(final String ip, final int port) {
        new Thread() {
            @Override
            public void run() {
                try {
                    socket = new Socket(ip, port);
                    if (socket != null) {
                        input = socket.getInputStream();
                        output = socket.getOutputStream();
                    }

                    while (socket.isConnected()){
                        sleep(100);
                        count = input.read(buf);
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();
    }

    public void send(final String str) {
        new Thread() {
            @Override
            public void run() {
                if (socket.isConnected() && output != null){
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
            public void run() {
                if (socket.isConnected() && output != null){
                    try {
                        output.write(buf);
                        output.flush();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }.start();

    }

}
