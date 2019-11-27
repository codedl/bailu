package com.example.myapp;

import android.bluetooth.BluetoothDevice;
import android.util.Log;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

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
//        cancelBondProcess(btClass, device);
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

    static String toHexString(byte buf[]) {
        String returnStr = "";
        for (int i = 0; i < buf.length; i++) {
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

    static byte[] doubleToBytes(double value) {
        long temp = Double.doubleToRawLongBits(value);
        byte buf[] = new byte[8];
        buf[0] = (byte) (temp & 0xff);
        buf[1] = (byte) ((temp >> 8) & 0xff);
        buf[2] = (byte) ((temp >> 16) & 0xff);
        buf[3] = (byte) ((temp >> 24) & 0xff);
        buf[4] = (byte) ((temp >> 32) & 0xff);
        buf[5] = (byte) ((temp >> 40) & 0xff);
        buf[6] = (byte) ((temp >> 48) & 0xff);
        buf[7] = (byte) ((temp >> 56) & 0xff);
        return buf;
    }

    static byte[] doubleArrToBytes(double[] value) {
        byte[] retbuf = new byte[value.length * 8];
        int index = 0;
        byte[] doubuf = new byte[8];
        for (double tmp : value) {
            doubuf = doubleToBytes(tmp);
            for (byte byteTmp : doubuf) {
                retbuf[index++] = byteTmp;
            }
        }
        if (index == value.length)
            return retbuf;
        else {
            System.out.println("doubles to bytes err");
            return retbuf;
        }
    }

    //把value按单位转化成byte
    static byte[] doubleToBytesUnit(double value) {
        byte[] valbuf = new byte[12];
        int index = 0;
        long longvalue = (long) value;

        int tmp = (int) (longvalue % 1000);
        valbuf[index++] = (byte) (tmp & 0xff);
        valbuf[index++] = (byte) ((tmp >> 8) & 0xff);
        valbuf[index++] = (byte) ((tmp >> 16) & 0xff);
        valbuf[index++] = (byte) ((tmp >> 24) & 0xff);

        tmp = (int) ((longvalue % 1000000) / 1000);//10^3
        valbuf[index++] = (byte) (tmp & 0xff);
        valbuf[index++] = (byte) ((tmp >> 8) & 0xff);
        valbuf[index++] = (byte) ((tmp >> 16) & 0xff);
        valbuf[index++] = (byte) ((tmp >> 24) & 0xff);

        tmp = (int) (longvalue / 1000000);//10^6
        valbuf[index++] = (byte) (tmp & 0xff);
        valbuf[index++] = (byte) ((tmp >> 8) & 0xff);
        valbuf[index++] = (byte) ((tmp >> 16) & 0xff);
        valbuf[index++] = (byte) ((tmp >> 24) & 0xff);

        return valbuf;
    }

    /**
     * 计算数组的校验和
     *
     * @param array      数组名
     * @param startIndex 数组的起始位置
     * @param endIndex   数组的结束位置
     * @return 校验和
     */
    static byte arraySum(byte array[], int startIndex, int endIndex) {
        byte ret = 0;
        for (int i = startIndex; i < endIndex; i++) {
            ret += array[i];
        }
        return ret;
    }

}
