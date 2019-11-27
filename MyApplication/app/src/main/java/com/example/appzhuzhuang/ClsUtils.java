package com.example.appzhuzhuang;

import android.bluetooth.BluetoothDevice;
import android.util.Log;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.text.SimpleDateFormat;
import java.util.Date;

public class ClsUtils {
    /**
     * 与设备配对 参考源码：platform/packages/apps/Settings.git
     * /Settings/src/com/android/settings/bluetooth/CachedBluetoothDevice.java
     */
    static public boolean createBond(Class btClass, BluetoothDevice btDevice) throws Exception {
        Method createBondMethod = btClass.getMethod("createBond");
        Boolean returnValue = (Boolean) createBondMethod.invoke(btDevice);
        return returnValue.booleanValue();
    }

    /**
     * 与设备解除配对 参考源码：platform/packages/apps/Settings.git
     * /Settings/src/com/android/settings/bluetooth/CachedBluetoothDevice.java
     */
    static public boolean removeBond(Class<?> btClass, BluetoothDevice btDevice) throws Exception {
        Method removeBondMethod = btClass.getMethod("removeBond");
        Boolean returnValue = (Boolean) removeBondMethod.invoke(btDevice);
        return returnValue.booleanValue();
    }

    static public boolean setPin(Class<? extends BluetoothDevice> btClass, BluetoothDevice btDevice, String str) throws Exception {
        try {
            Method removeBondMethod = btClass.getDeclaredMethod("setPin", new Class[]{byte[].class});
            Boolean returnValue = (Boolean) removeBondMethod.invoke(btDevice, new Object[]{str.getBytes()});
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return true;

    }

    // 取消用户输入
    static public boolean cancelPairingUserInput(Class<?> btClass, BluetoothDevice device) throws Exception {
        Method createBondMethod = btClass.getMethod("cancelPairingUserInput");
        Boolean returnValue = (Boolean) createBondMethod.invoke(device);
        return returnValue.booleanValue();
    }

    // 取消配对
    static public boolean cancelBondProcess(Class<?> btClass,
                                            BluetoothDevice device)

            throws Exception {
        Method createBondMethod = btClass.getMethod("cancelBondProcess");
        Boolean returnValue = (Boolean) createBondMethod.invoke(device);
        return returnValue.booleanValue();
    }

    //确认配对

    static public void setPairingConfirmation(Class<?> btClass, BluetoothDevice device, boolean isConfirm) throws Exception {
        Method setPairingConfirmation = btClass.getDeclaredMethod("setPairingConfirmation", boolean.class);
        setPairingConfirmation.invoke(device, isConfirm);
    }


    /**
     * @param clsShow
     */
    static public void printAllInform(Class clsShow) {
        try {
            // 取得所有方法
            Method[] hideMethod = clsShow.getMethods();
            int i = 0;
            for (; i < hideMethod.length; i++) {
                Log.e("method name", hideMethod[i].getName() + ";and the i is:"
                        + i);
            }
            // 取得所有常量
            Field[] allFields = clsShow.getFields();
            for (i = 0; i < allFields.length; i++) {
                Log.e("Field name", allFields[i].getName());
            }
        } catch (SecurityException e) {
            // throw new RuntimeException(e.getMessage());
            e.printStackTrace();
        } catch (IllegalArgumentException e) {
            // throw new RuntimeException(e.getMessage());
            e.printStackTrace();
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    //对整个数组进行处理
    static String toHexString(byte buf[]) {
        String returnStr = "";
        for (int i = 0; i < buf.length; i++) {
            returnStr += String.format("0x%02x ", buf[i]);
        }
        return returnStr;
    }

    //对数组中从startIndex到endIndex的元素进行处理
    static String toHexString(byte buf[], int startIndex, int endIndex) {
        String returnStr = "";
        for (int i = startIndex; i < endIndex; i++) {
            returnStr += String.format("0x%02x ", buf[i]);
        }
        return returnStr;
    }

    static byte[] toHexBytes(String hexString) {
        String temp = hexString.replace(" ", "");
        temp = temp.replace(",", "");
        int length = temp.length();
        byte[] buf = new byte[length];
        for (int i = 0; i < length; i += 2) {
            buf[i / 2] = (byte) ((Character.digit(temp.charAt(i), 16) << 4) + Character.digit(temp.charAt(i + 1), 16));
        }
        return buf;
    }

    //获取系统时间
    static String currentTime() {
        SimpleDateFormat format = new SimpleDateFormat("yyyyMMddHHmmss");//20191023180330
        Date date = new Date(System.currentTimeMillis());
        return format.format(date);
    }

    //时间保存到byte数组里，占九个字节
    static void currentTime(byte bytes[], int index) {
        SimpleDateFormat format = new SimpleDateFormat("yyyyMMddHHmmss");//20191023180330
        Date date = new Date(System.currentTimeMillis());
        String time = format.format(date);
        toBytes(Integer.parseInt(time.substring(0, 4)), bytes, index);//四个字节的年份
        bytes[index + 4] = Byte.parseByte(time.substring(4, 6));//月份
        bytes[index + 5] = Byte.parseByte(time.substring(6, 8));//日期
        bytes[index + 6] = Byte.parseByte(time.substring(8, 10));//时
        bytes[index + 7] = Byte.parseByte(time.substring(10, 12));//分
        bytes[index + 8] = Byte.parseByte(time.substring(12));//秒

    }


    //将int型数据保存到bytes数组index开始处
    static void toBytes(int number, byte[] bytes, int index) {
        bytes[index] = (byte) (number);
        bytes[index + 1] = (byte) (number >> 8);
        bytes[index + 2] = (byte) (number >> 16);
        bytes[index + 3] = (byte) (number >> 24);

    }

    static void toBytes(float number, byte[] bytes, int index) {
        int temp = Float.floatToIntBits(number);//把数据从float转化成int型
        bytes[index] = (byte) (temp);
        bytes[index + 1] = (byte) (temp >> 8);
        bytes[index + 2] = (byte) (temp >> 16);
        bytes[index + 3] = (byte) (temp >> 24);
    }

    //bytes组成float型数据
    static float floatFromBytes(byte[] bytes, int index) {
        int temp = 0;
        temp = bytes[index] & 0xff;
        temp |= (bytes[index + 1] << 8);
        temp |= (bytes[index + 2] << 16);
        temp |= (bytes[index + 3] << 24);
        return Float.intBitsToFloat(temp);
    }

    static int intFromBytes(byte[] bytes, int index) {
        int temp = 0;
        temp = bytes[index] & 0xff;
        temp |= (bytes[index + 1] << 8);
        temp |= (bytes[index + 2] << 16);
        temp |= (bytes[index + 3] << 24);
        return temp;
    }


}
