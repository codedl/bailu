package com.example.appzhuzhuang;

import android.content.Context;
import android.util.Log;
import android.util.Xml;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlSerializer;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.lang.reflect.Field;
import java.net.HttpURLConnection;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

public class fileManager {
    private static String tag = "fileManager";
    private String fileUrl;
    private String fileName;
    ArrayList<String> fileList = new ArrayList<>();
    public static int maxLength;//文件的长度
    public static int curLength;//已经发送的文件长度

    public fileManager() {

    }

    //使用mime协议上传文件，post请求方式
    //@url网址
    //@path文件名
    public void upload(String url, String path) {
        //检查输入合法性
        if (url == null || url.isEmpty() || path == null || path.isEmpty() || !new File(path).exists())
            return;
        this.fileUrl = url;
        this.fileName = path;
        maxLength = 0;
        curLength = 0;
        new uploadThread().start();
    }


    public class uploadThread extends Thread {
        @Override
        public void run() {
            synchronized ("") {//同步代码块，保证文件同步上传
                String end = "\r\n";
                String boundary = "******";
                String twoHyphens = "--";

                try {

                    FileInputStream fileInputStream = new FileInputStream(fileName);
                    URL url = new URL(fileUrl);
                    Log.d(tag, fileUrl);
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
                    maxLength = fileInputStream.available();
                    Log.d(tag, "max:" + maxLength);
                    httpURLConnection.setRequestProperty("Content-Type", "multipart/form-data;boundary=" + boundary);
                    Log.d("fileManager", "file:" + fileName + ";length(Bytes):" + maxLength);
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
                        curLength += length;
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
                    //打印上传结果
                    String result = getSubString(b.toString(), "<h1>", ".").replace("\n", "");
                    Log.d("fileManager", result);

                    //将服务器数据保存到文件
                    //覆盖原来文件
               /* FileOutputStream output = new FileOutputStream(Environment.getExternalStorageDirectory().getAbsolutePath() + "/audio/log.txt");
                PrintStream print = new PrintStream(output);
                Log.d("fileManager",b.toString().indexOf("folder"));
                print.println(b.toString());
                dataOutputStream.close();*/
                    //以追加方式写入文件
                    File file = new File(param.path + "upload.txt");
                    if (file.length() > 1024 * 1024) {
                        file.delete();
                        file = new File(param.path + "upload.txt");
                    }
                    Log.d(tag, "upload.txt:" + file.length() / 1024 + "kb");
                    RandomAccessFile raf = new RandomAccessFile(file, "rw");
                    raf.seek(file.length());
                    Date date = new Date(System.currentTimeMillis());
                    SimpleDateFormat simpleDateFormat = new SimpleDateFormat("yyyy.MM.dd-HH:mm:ss");
                    //保存文件上传日志
                    raf.write((boundary + "Time:" + simpleDateFormat.format(date) + end).getBytes());
                    raf.write(("File:" + fileName + end).getBytes());
                    raf.write(("Length:" + maxLength + "bytes" + end).getBytes());
//                raf.write(b.toString().getBytes());
                    raf.write(("Result:" + result + end).getBytes());
                    raf.close();
                    inputStream.close();
                    fg_zhuk.socket.send(procotol.uploadPrepare(0x06));//发送成功回复主机


                } catch (Exception e) {
                    fg_zhuk.socket.send(procotol.uploadPrepare(0x15));
                    e.printStackTrace();
                }
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

    public static String fileInfo(String file) {
        File temp = new File(file);
        String info = "文件名:" + temp.getName() + "\n";
        info += "修改时间:" + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(temp.lastModified()) + "\n";
        info += "路径:" + temp.getAbsolutePath().replace(temp.getName(), "") + "\n";
        double length = (double) temp.length() / 1024;
        if (length > 1024) {
            length = length / 1024;
            info += String.format("大小:%.3f", length) + "MB";
        } else info += String.format("大小:%.3f", length) + "KB";
        return info;
    }

    //获取str中从head开始到end结尾的字符串
    public String getSubString(String str, String head, String end) {
        int headIndex = str.indexOf(head);
        String temp = str.substring(headIndex);
        int endIndex = temp.indexOf(end);
        return temp.substring(temp.indexOf(head), endIndex + end.length());
    }

    //将参数保存到xml文件中
    public static void savexml() {

        try {
            Field[] allFields = new param().getClass().getDeclaredFields();

            File file = new File(param.pathXml);
            FileOutputStream fos = new FileOutputStream(file);
            XmlSerializer serializer = Xml.newSerializer();
            serializer.setOutput(fos, "utf-8");
            serializer.startDocument("utf-8", true);

            serializer.startTag(null, "param");//<param >
            serializer.attribute(null, "time", ClsUtils.currentTime());//<param time="">

            for (int i = 0; i < allFields.length; i++) {
                serializer.startTag(null, allFields[i].getName());//<getName>
                serializer.attribute(null, "type", allFields[i].getType().toString());//<type = getType>
                serializer.text(String.valueOf(allFields[i].get(new param())));//<getName type=getType>get(Object)
                serializer.endTag(null, allFields[i].getName());//<getName type=getType>get(Object)</getName>

            }

            serializer.endTag(null, "param");
            serializer.endDocument();
            fos.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    //从xml文件中读取
    public static void readxml() {
        try {
            param P = new param();
            File file = new File(param.pathXml);
            if (!file.exists())//初次启动应用不用读取参数
                return;
            FileInputStream fis = new FileInputStream(file);
            XmlPullParser parser = Xml.newPullParser();
            parser.setInput(fis, "utf-8");
            int event = parser.getEventType();
            String value;
            Field field;//变量,static java.lang.String com.example.appzhuzhuang.param.zhuk_server
            while (event != XmlPullParser.END_DOCUMENT) {
                String tagName = parser.getName();//xml文件中的变量名,zhuk_server
                switch (event) {
                    case XmlPullParser.START_TAG:

                        if ("param".equals(tagName) || tagName.contains("path")) {//解析到param文件头或path返回
                            break;
                        }
                        value = parser.nextText();//变量值为String，需要转化成相应类型
                        field = P.getClass().getDeclaredField(tagName);
                        //根据变量类型设置变量值
                        if (field.getType().equals(boolean.class))
                            field.setBoolean(P, Boolean.parseBoolean(value));//解析字符串成bool型变量
                        else if (field.getType().equals(String.class))
                            field.set(P, value);
                        else if (field.getType().equals(Integer.class))
                            field.setInt(P, Integer.parseInt(value));
                        else if (field.getType().equals(Float.class))
                            field.setFloat(P, Float.parseFloat(value));

                        break;
                    case XmlPullParser.END_TAG:
                        break;
                }
                event = parser.next();
            }
            fis.close();
            /*Log.i(tag, "param.getDeclaredFields");
            Field[] fileds = new param().getClass().getDeclaredFields();
            for (int i = 0; i < fileds.length; i++) {
                Log.i(tag, fileds[i].getName());
                Log.i(tag, fileds[i].getType().toString());
                Log.i(tag, String.valueOf(fileds[i].get(P)));
            }*/
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    //启动应用时把raw目录下文件拷贝到sd内存
    public static void copyFile(final Context context) {

        new Thread(new Runnable() {
            @Override
            public void run() {
                Field[] fields = R.raw.class.getDeclaredFields();//获取raw目录下所有文件
                String rawName;
                int rawId;
                InputStream is;
                FileOutputStream fos;
                File file;
                for (int i = 0; i < fields.length; i++) {
                    rawName = fields[i].getName();
                    file = new File(param.pathDisturb + rawName);//生成文件
                    if (file.exists())//已经拷贝过
                        continue;
                    Log.i(tag, "rawFile:" + rawName);
                    try {
                        rawId = fields[i].getInt(R.raw.class);
                        Log.i(tag, "rawId:" + rawId);
                        is = context.getResources().openRawResource(rawId);//打开raw文件
                        fos = new FileOutputStream(file);
                        byte[] buf = new byte[is.available()];
                        int length = 0;
                        //将raw目录文件写入sd卡
                        while ((length = is.read(buf)) != -1) {
                            fos.write(buf, 0, length);
                        }
                        fos.flush();
                        fos.close();
                        is.close();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }
        }).start();

    }

    //移动文件file到目录path
    public static void copyFile(final String file, final String path, final boolean move) {
        new Thread() {
            @Override
            public void run() {
                synchronized ("") {
                    try {
                        File source = new File(file);
                        if (!source.exists())
                            return;
                        FileInputStream fis = new FileInputStream(source);
                        String name = source.getName();
                        File output = new File(path + name);
                        Log.d(tag, path + name);
                        FileOutputStream fos = new FileOutputStream(output);
                        int length = -1;
                        byte[] buf = new byte[fis.available()];
                        while ((length = fis.read(buf)) != -1) {
                            fos.write(buf, 0, length);
                        }
                        fos.flush();
                        fos.close();
                        fis.close();
                        if (move)//如果移动文件
                            source.delete();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }
        }.start();

    }

    static void mkdir(String directory) {
        File file = new File(directory);
        if (!file.exists())
            file.mkdir();
    }

}
