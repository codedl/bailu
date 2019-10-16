package com.example.myapplication;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

public class BluetoothLowEnergy {

    //特征值，跟蓝牙设备通信
    BluetoothGattCharacteristic red;
    BluetoothGattCharacteristic green;
    BluetoothGattCharacteristic btnbgc;
    BluetoothGattCharacteristic xdt;
    private BluetoothGatt bleGatt;//服务包含多个特征
    private BluetoothAdapter bleAdapter;
    private Context context;
    public ArrayList<BluetoothDevice> bleDevices = new ArrayList<>();//扫描的蓝牙设备
    public ArrayList<String> bleDeviceStr = new ArrayList<>();//显示到列表的字符串

    public BluetoothLowEnergy(Context context) {
        this.context = context;
        bleAdapter = BluetoothAdapter.getDefaultAdapter();
    }

    //扫描蓝牙设备
    public void startScan() {
        bleDevices.clear();
        bleDeviceStr.clear();
        disconnect();
        bleAdapter.getBluetoothLeScanner().startScan(this.scanCallBack);
    }

    public void stopScan() {
        bleAdapter.getBluetoothLeScanner().stopScan(this.scanCallBack);
    }

    //蓝牙扫描的回调函数，返回蓝牙扫描的结果
    public ScanCallback scanCallBack = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            super.onScanResult(callbackType, result);
            BluetoothDevice dev = result.getDevice();
            String name = dev.getName();
            String addr = dev.getAddress();
            StringBuilder scanResult = new StringBuilder();
            scanResult.append(name);
            scanResult.append(":");
            scanResult.append(addr);

            if (name == null || name.isEmpty() || bleDevices.contains(dev))
                return;
            System.out.println(scanResult.toString());
            bleDevices.add(dev);
            bleDeviceStr.add(scanResult.toString());
            Intent intent = new Intent(Action.BLE_SCAN_FOUND);
            intent.putExtra(Action.DEVICE, dev);
            context.sendBroadcast(intent);
        }
    };

    //连接蓝牙
    //@bluetoothDevice 扫描到的蓝牙设备
    public void bleConnect(BluetoothDevice bluetoothDevice) {
        bleAdapter.getBluetoothLeScanner().stopScan(scanCallBack);
        bleGatt = bluetoothDevice.connectGatt(context, true, gattCallBack);
//        bleDevices.clear();
//        bleDeviceStr.clear();
    }

    private void disconnect() {
        bleAdapter.getBluetoothLeScanner().stopScan(scanCallBack);
        if (bleGatt != null)
            bleGatt.disconnect();
    }

    //蓝牙连接的回调函数
    public BluetoothGattCallback gattCallBack = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            super.onConnectionStateChange(gatt, status, newState);
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                if (gatt != null) {
                    bleGatt = gatt;
                    String addr = gatt.getDevice().getAddress();
                    System.out.println(addr);
                    gatt.discoverServices();
                }
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            super.onServicesDiscovered(gatt, status);
            Log.d("print", "onServicesDiscovered");
            List<BluetoothGattService> gattServices = gatt.getServices();
            for (BluetoothGattService bgs : gattServices) {
                String uuid = bgs.getUuid().toString();
                Log.d("print", "servers uuid:" + uuid);
                List<BluetoothGattCharacteristic> gattCharacteristics = bgs.getCharacteristics();
                for (BluetoothGattCharacteristic bgc : gattCharacteristics) {
                    uuid = bgc.getUuid().toString();
                    if (uuid.equals("f0001111-0451-4000-b000-000000000000")) {
                        red = bgc;//LED红
                    }
                    if (uuid.equals("f0001112-0451-4000-b000-000000000000")) {
                        green = bgc;//LED绿
                    }
                    if (uuid.equals("f0001121-0451-4000-b000-000000000000")) {
                        btnbgc = bgc;//按键
                    }
                    if (uuid.equals("0000fee1-0000-1000-8000-00805f9b34fb")) {
                        xdt = bgc;
                    }
                    Log.d("print", "character uuid:" + uuid);

                    int charaProp = bgc.getProperties();
                    if ((charaProp & BluetoothGattCharacteristic.PROPERTY_READ) > 0) {
                        Log.d("print", "可读");
                    }
                    if ((charaProp & BluetoothGattCharacteristic.PROPERTY_WRITE) > 0) {
                        Log.d("print", "可写");

                    }
                    if ((charaProp & BluetoothGattCharacteristic.PROPERTY_NOTIFY) > 0) {
                        Log.d("print", "通知");
                    }

                }
            }
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            super.onCharacteristicRead(gatt, characteristic, status);
            byte[] buf = characteristic.getValue();
            String str = "";
            for (int i = 0; i < buf.length; i++) {
                str += String.format("0x%02x;", buf[i]);
            }
            Log.d("print", "onCharacteristicRead:" + str);
        }

        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            super.onCharacteristicWrite(gatt, characteristic, status);
            byte[] writebuf = characteristic.getValue();
            Log.d("print", "onCharacteristicWrite:" + ClsUtils.toHexString(writebuf));
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            super.onCharacteristicChanged(gatt, characteristic);
            Log.d("print", "onCharacteristicChanged");
        }

        @Override
        public void onDescriptorRead(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            super.onDescriptorRead(gatt, descriptor, status);
            Log.d("print", "onDescriptorRead");

        }

        @Override
        public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            super.onDescriptorWrite(gatt, descriptor, status);
            Log.d("print", "onDescriptorWrite");
        }
    };

    //蓝牙写操作
    //@characteristic特征值，连接蓝牙时获取
    //需要写的数据
    public void bleWrite(BluetoothGattCharacteristic characteristic, byte val[]) {
        if (bleGatt != null && characteristic != null && val != null) {
//        blegatt.setCharacteristicNotification(characteristic, true);
            characteristic.setValue(val);
//        characteristic.setWriteType(characteristic.WRITE_TYPE_NO_RESPONSE);
            bleGatt.writeCharacteristic(characteristic);
        }
    }
}
