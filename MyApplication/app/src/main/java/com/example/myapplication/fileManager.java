package com.example.myapplication;

import android.content.Context;
import android.os.Environment;

import java.io.DataOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.PrintStream;
import java.net.ConnectException;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.Date;

public class fileManager {
    String fileUrl;
    String fileName;
    ArrayList<String> fileList = new ArrayList<>();

    public fileManager() {

    }

     public void upload(String url,String name){
         this.fileUrl = url;
         this.fileName = name;
         new uploadThread().start();
    }

    public class  uploadThread extends Thread{
        @Override
        public void run(){
            String end = "\r\n";
            String twoHyphens = "--";
            String boundary = "******";
            try {
                FileOutputStream output = new FileOutputStream(fileName);
                PrintStream print = new PrintStream(output);
                Date date = new Date(System.currentTimeMillis());//获取当前时间
                print.println(date);

                URL url = new URL(fileUrl);
                HttpURLConnection httpURLConnection = (HttpURLConnection) url.openConnection();
                httpURLConnection.setChunkedStreamingMode(128 * 1024);
                httpURLConnection.setDoInput(true);
                httpURLConnection.setDoOutput(true);
                httpURLConnection.setUseCaches(false);
                httpURLConnection.setRequestMethod("POST");
                httpURLConnection.setRequestProperty("Connection", "Keep-Alive");
                httpURLConnection.setRequestProperty("CharSet", "UTF-8");
                httpURLConnection.setRequestProperty("Content-Type", "multipart/form-data;boundary=" + boundary);

                DataOutputStream dataOutputStream = new DataOutputStream(httpURLConnection.getOutputStream());
                dataOutputStream.writeBytes(twoHyphens + boundary + end);
                dataOutputStream.writeBytes("Content-Disposition:form-data;" + "name=\"file1\";filename=\"" + fileName + "\"" + end);
                dataOutputStream.writeBytes(end);

                FileInputStream fileInputStream = new FileInputStream(fileName);
                int bufferSize = 1024;
                byte[] buffer = new byte[bufferSize];
                int length = -1;
                while ((length = fileInputStream.read(buffer)) != -1) {
                    dataOutputStream.write(buffer, 0, length);
                }
                dataOutputStream.writeBytes(end);
                dataOutputStream.writeBytes(twoHyphens + boundary + twoHyphens + end);
                fileInputStream.close();
                dataOutputStream.flush();
                InputStream inputStream = httpURLConnection.getInputStream();
                int ch;
                StringBuilder b = new StringBuilder();
                while((ch = inputStream.read()) != -1){
                    b.append((char) ch);
                }
                //保存日志
                output = new FileOutputStream(Environment.getExternalStorageDirectory().getAbsolutePath() + "/log.txt");
                print = new PrintStream(output);
                print.println(b.toString());
                dataOutputStream.close();
                inputStream.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
}
