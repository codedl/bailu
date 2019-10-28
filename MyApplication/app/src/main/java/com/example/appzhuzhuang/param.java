package com.example.appzhuzhuang;

import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.os.Environment;

import java.util.HashSet;

public class param {
    static String zhuk_ip = "192.168.42.69";//主控ip地址
    static String zhuk_port = "80";//主控端口
    static String zhuk_server = "http://192.168.42.69/web/";//主控文件上传服务器地址
    static BluetoothDevice bleData;
    static BluetoothDevice btPhone;
    static boolean bind;
    static String path = "";//应用保存数据的路径
    static String pathRecord;
    static String pathDisturb;
    static String pathXml;
    static String pathUped;
    static HashSet<String> file_record = new HashSet<>();
    static String file_disturb = "";
    static BluetoothLowEnergy ble;
    static BluetoothClassic btc;
    static int battery;//手机电量
    static int zhuzAddr = 1;//驻装地址
    static int zhukAddr = 0;//主控地址
    static float longitude;//经度
    static float latitude;//纬度
    static uiHandler uiHandler;//处理非ui线程的ui事件

    static void init(Context context) {
        //应用路径
        path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/zhuzh/";
        fileManager.mkdir(path);//检查目录是否存在，不存在就创建目录
        pathRecord = path + "record/";//录音文件路径
        fileManager.mkdir(pathRecord);
        pathDisturb = path + "disturb/";//干扰音频文件路径
        fileManager.mkdir(pathDisturb);
        pathUped = path + "uped/";//已经上传过的文件
        fileManager.mkdir(pathUped);
        pathXml = path + "param.xml";
        //蓝牙初始化
        ble = new BluetoothLowEnergy(context);
        btc = new BluetoothClassic(context);
        uiHandler = new uiHandler();
    }


}
