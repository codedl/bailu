package com.example.myapp;

public class param {
    static String freqval = new String("10.000000");
    static String frequnit = new String("MHz");
    static String freqstr = null;
    static String powerval = new String("1.000");
    static String powerunit = new String("W");
    static String powerstr = null;
    static String bleName = "蓝牙名";

    static double power = 1;//功率,默认1W
    static double freq = 10;//频率，默认10MHz
    static byte mode = 0;//工作模式
    static byte freqway = 0;//频率方式
    static byte freqrange = 0;//跳频范围
    static byte freqspeed = 0;//跳频速度
    static byte demodway = 0;//调制方式
    static byte demodsoutce = 0;//调制源
    static String msgStr;//报文字符串
    static byte msgBytes[];//报文byte
    static String msgBits = "";//报文比特序列
    static byte packageIndex;//每个包的序列号

}
