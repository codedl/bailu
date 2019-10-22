package com.example.myapplication;

import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.os.Environment;

public class param {
    static String zhuk_ip = "192.168.42.69";//主控ip地址
    static String zhuk_port = "80";//主控端口
    static String zhuk_server = "http://192.168.42.69/web/";//主控文件上传服务器地址
    static BluetoothDevice bleData;
    static BluetoothDevice btPhone;
    static boolean bind;
    static String path = "/storage/emulated/0/zhuzh/";
    static String pathRecord;
    static String pathDisturb;
    static String pathXml;
    static String file_record = "";
    static String file_disturb = "";
    static BluetoothLowEnergy ble;
    static BluetoothClassic btc;

    static void init(Context context) {
        //应用路径
        path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/zhuzh/";
        pathRecord = path + "record/";//录音文件路径
        pathDisturb = path + "disturb/";//干扰音频文件路径
        pathXml = path + "param.xml";
        //蓝牙初始化
        ble = new BluetoothLowEnergy(context);
        btc = new BluetoothClassic(context);
    }
}
