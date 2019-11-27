package com.example.appzhuzhuang;

import android.os.Message;
import android.util.Log;

//手机与外界通信协议
public class procotol {
    private static String tag = "procotol";

    //接收数据解析
    static void rcvProcess(byte bytes[], int count) {
        Message msg = param.uiHandler.obtainMessage();//将非ui线程的数据发送给ui线程
        //数据是否正确
        if (bytes[0] == (byte) 0x90 && bytes[count - 1] == (byte) 0xa0) {
            param.zhukAddr = bytes[1];//主控中心地址
            if (param.zhuzAddr == bytes[2])
                Log.d(tag, "zhuzAddr:" + bytes[2]);
            switch (bytes[3]) {
                case 0x20://查询状态信息
                    fg_zhuk.socket.send(statusPrepare());
                    break;
                case 0x22://遥控关机
                    powerPrepare();
                    break;
                case 0x70://上传音频文件
                    msg.what = 0x70;
                    fg_zhuk.socket.send(procotol.uploadPrepare(0x07));//通知主机已经接收到指令，开始上传文件
                    break;
                case 0x71://查询电台信息
                    stationPrepare();
                    break;
                case 0x72://开启干扰
                    msg.what = 0x72;
                    break;
                case 0x73://关闭干扰
                    msg.what = 0x73;
                    break;
            }
        }
        param.uiHandler.sendMessage(msg);

    }

    //准备状态信息
    private static byte[] statusPrepare() {
        byte bytes[] = new byte[30];
        bytes[0] = (byte) 0x90;//起始符
        bytes[1] = (byte) param.zhukAddr;//主控地址
        bytes[2] = (byte) param.zhuzAddr;//驻装地址
        bytes[3] = (byte) 0x20;//状态信息功能码
        ClsUtils.toBytes(21, bytes, 4);//数据长度
        ClsUtils.currentTime(bytes, 8);//时间
        ClsUtils.toBytes(param.longitude, bytes, 17);//经度
        ClsUtils.toBytes(param.latitude, bytes, 21);//纬度
        ClsUtils.toBytes((float) param.battery, bytes, 25);//电量
        bytes[29] = (byte) 0xa0;
        return bytes;
    }

    public static void powerPrepare() {
        byte[] bytes = new byte[9];
        bytes[0] = (byte) 0x90;//起始符
        bytes[1] = (byte) param.zhukAddr;//主控地址
        bytes[2] = (byte) param.zhuzAddr;//驻装地址
        bytes[3] = (byte) 0x22;//功能码
        ClsUtils.toBytes(0x15, bytes, 4);//失败返回
        bytes[8] = (byte) 0xa0;
    }

    //回复主机上传文件的状态
    //@upload 0x06表示上传成功；0x07表示正在上传;0x15表示上传失败
    public static byte[] uploadPrepare(int upload) {
        byte[] bytes = new byte[9];
        bytes[0] = (byte) 0x90;//起始符
        bytes[1] = (byte) param.zhukAddr;//主控地址
        bytes[2] = (byte) param.zhuzAddr;//驻装地址
        bytes[3] = (byte) 0x70;//功能码
        ClsUtils.toBytes(upload, bytes, 4);//成功返回
        bytes[8] = (byte) 0xa0;
        return bytes;
    }


    public static void stationPrepare() {

    }

    //返回干扰数据
    //@isOn 干扰是否开启
    //@isSucceed 干扰操作是否成功
    public static byte[] disturbPrepare(boolean isOn, boolean isSucceed) {
        byte[] bytes = new byte[9];
        bytes[0] = (byte) 0x90;//起始符
        bytes[1] = (byte) param.zhukAddr;//主控地址
        bytes[2] = (byte) param.zhuzAddr;//驻装地址
        if (isOn)
            bytes[3] = (byte) 0x72;//功能码
        else
            bytes[3] = (byte) 0x73;//功能码
        if (isSucceed)
            ClsUtils.toBytes(0x06, bytes, 4);//成功返回
        else
            ClsUtils.toBytes(0x15, bytes, 4);//失败返回
        bytes[8] = (byte) 0xa0;
        return bytes;
    }
}
