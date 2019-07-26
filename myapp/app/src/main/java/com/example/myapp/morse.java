package com.example.myapp;

public class morse {
//    字母表
    static String alphTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
//   摩尔斯码
    static String bitsTable[] = {
            "10111",
            "111010101",
            "11101011101",
            "1110101",
            "1",
            "101011101",
            "111011101",
            "1010101",
            "101",
            "1011101110111",
            "1110111",
            "101110101",
            "1110111",
            "11101",
            "11101110111",
            "10111011101",
            "1110111010111",
            "1011101",
            "10101",
            "111",
            "1010111",
            "101010111",
            "101110111",
            "11101010111",
            "1110101110111",
            "11101110101",
            "1110111011101110111",
            "10111011101110111",
            "101011101110111",
            "1010101110111",
            "10101010111",
            "101010101",
            "11101010101",
            "1110111010101",
            "111011101110101",
            "11101110111011101"
    };
//二进制字符串转化成byte数组
    static byte[] bitsToBytes(String binaryStr) {
        String subStr = "";
        int index = 0;
        String tmpStr = binaryStr;
        if (tmpStr.length() % 8 != 0) {
            for (int i = 0; i < (8 - tmpStr.length() % 8); i++) {
                tmpStr += "0";
            }
        }
        byte[] bytes = new byte[tmpStr.length() / 8];
        for (index = 0; index < bytes.length; index++) {
            bytes[index] = (byte) Integer.parseInt(tmpStr.substring(index * 8, index * 8 + 8), 2);
            System.out.println(bytes[index]);
        }
        return bytes;
    }
//字符串转化成二进制字符串
    static String stringToBits(String string) {
        String destStr = string.replaceAll("[^0-9a-zA-Z]", "").trim();
        String retStr = "";
        System.out.println(destStr);
        char temp;
        int index;
        for (int i = 0; i < destStr.length(); i++) {
            temp = destStr.charAt(i);
            index = alphTable.indexOf(temp);
            retStr += bitsTable[index];
            retStr += "000";
            if ((i + 1) % 4 == 0 && (i + 1) != destStr.length()) {
                retStr += "00";
            }
        }
        retStr += "00";
        return retStr;
    }
}
