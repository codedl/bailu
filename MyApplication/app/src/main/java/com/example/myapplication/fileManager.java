package com.example.myapplication;

import android.os.Environment;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.net.HttpURLConnection;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

public class fileManager {
    String fileUrl;
    String fileName;
    ArrayList<String> fileList = new ArrayList<>();

    public fileManager() {

    }

    //使用mime协议上传文件，post请求方式
    //@url网址
    //@name文件名
    public void upload(String url, String name) {
        this.fileUrl = url;
        this.fileName = name;
        new uploadThread().start();
    }

    public class uploadThread extends Thread {
        @Override
        public void run() {

            String end = "\r\n";
            String boundary = "******";
            String twoHyphens = "--";

            try {

                FileInputStream fileInputStream = new FileInputStream(fileName);

                URL url = new URL(fileUrl);
                HttpURLConnection httpURLConnection = (HttpURLConnection) url.openConnection();
//                httpURLConnection.setChunkedStreamingMode(1024 * 1024);
                //写进http端口的数据大小 = 文件长度 +  writeBytes的内容, 可以用length计算
                httpURLConnection.setFixedLengthStreamingMode(82 + fileInputStream.available() + fileName.length());
                httpURLConnection.setDoInput(true);
                httpURLConnection.setDoOutput(true);
                httpURLConnection.setUseCaches(false);
                httpURLConnection.setRequestMethod("POST");
                httpURLConnection.setRequestProperty("Connection", "Keep-Alive");
                httpURLConnection.setRequestProperty("CharSet", "UTF-8");
                //用表单上传文件,定义了数据分割线为boundary
                //内容用end分离
                httpURLConnection.setRequestProperty("Content-Type", "multipart/form-data;boundary=" + boundary);
                System.out.println("file:" + fileName + ";length:" + fileInputStream.available());
                DataOutputStream dataOutputStream = new DataOutputStream(httpURLConnection.getOutputStream());
                dataOutputStream.writeBytes(twoHyphens + boundary + end);
                //文件数据用end隔开
                dataOutputStream.writeBytes("Content-Disposition:form-data;" + "name=\"file1\";filename=\"" + fileName + "\"" + end);
                dataOutputStream.writeBytes(end);

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
                while ((ch = inputStream.read()) != -1) {
                    b.append((char) ch);
                }
                System.out.println(getSubString(b.toString(), "<meta", "</title>"));

                //将服务器数据保存到文件
                //覆盖原来文件
               /* FileOutputStream output = new FileOutputStream(Environment.getExternalStorageDirectory().getAbsolutePath() + "/audio/log.txt");
                PrintStream print = new PrintStream(output);
                System.out.println(b.toString().indexOf("folder"));
                print.println(b.toString());
                dataOutputStream.close();*/
                //以追加方式写入文件
                File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/audio/log.txt");
                RandomAccessFile raf = new RandomAccessFile(file, "rw");
                raf.seek(file.length());
                Date date = new Date(System.currentTimeMillis());
                SimpleDateFormat simpleDateFormat = new SimpleDateFormat("yyyy-MM-dd--HH:mm:ss");
                raf.write(("time="+simpleDateFormat.format(date)+end).getBytes());
                raf.write(("file=" + fileName + end).getBytes());
                raf.write(b.toString().getBytes());
                inputStream.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    //文件重命名
    public static void rename(String oldPath, String newPath) {
        File oldFile = new File(oldPath);
        File newFile = new File(newPath);
        //执行重命名
        oldFile.renameTo(newFile);
    }

    public static String fileInfo(File file) {
        String info = "文件名:" + file.getName() + "\n";
        info += "修改时间:" + new SimpleDateFormat("yyyy.MM.dd HH:mm:ss").format(file.lastModified()) + "\n";
        info += "路径:" + file.getAbsolutePath().replace(file.getName(), "") + "\n";
        info += String.format("大小:%.3f", (double) file.length() / 1024) + "KB";
        return info;
    }

    //获取str中从head开始到end结尾的字符串
    public String getSubString(String str, String head, String end) {
        int headIndex = str.indexOf(head);
        String temp = str.substring(headIndex);
        int endIndex = temp.indexOf(end);
        return temp.substring(temp.indexOf(head), endIndex + end.length());
    }


}
