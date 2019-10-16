package com.example.myapplication;

import android.bluetooth.BluetoothA2dp;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.ParcelUuid;
import android.view.View;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.UUID;

public class BluetoothClassic {
    Context context;
    private BluetoothAdapter adapter;
    private BluetoothSocket socket;
    private bluetoothReceiver receiver;
    public ArrayList<BluetoothDevice> btDevices = new ArrayList<>();//发现的设备列表
    public ArrayList<String> btDeviceStr = new ArrayList<>();//用来显示到列表的字符串

    public BluetoothClassic(Context context) {
        this.context = context;
        adapter = BluetoothAdapter.getDefaultAdapter();

        IntentFilter intentFilter = new IntentFilter(BluetoothDevice.ACTION_FOUND);//注册广播接收信号
        intentFilter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        intentFilter.addAction(BluetoothDevice.ACTION_PAIRING_REQUEST);
        intentFilter.addAction(BluetoothA2dp.ACTION_CONNECTION_STATE_CHANGED);
        intentFilter.addAction(BluetoothA2dp.ACTION_PLAYING_STATE_CHANGED);
        intentFilter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        receiver = new bluetoothReceiver();
        context.registerReceiver(receiver, intentFilter);//用BroadcastReceiver 来取得结果


    }

    //搜索蓝牙设备
    public void startDiscovery() {
        btDevices.clear();
        btDeviceStr.clear();
        if (adapter.isDiscovering()) {
            adapter.cancelDiscovery();
        }
        disconnect();
        adapter.startDiscovery();
    }

    //取消搜索蓝牙
    public void cancelDiscovery() {
        if (adapter.isDiscovering()) {
            adapter.cancelDiscovery();
        }
    }

    //连接之前需要进行配对
    private void createBond(BluetoothDevice dev) {
        adapter.cancelDiscovery();

        if (dev.getBondState() == BluetoothDevice.BOND_NONE) {
            System.out.println("start bond");
            try {
                Method method = dev.getClass().getMethod("createBond");
                method.setAccessible(true);
                method.invoke(dev);

            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private void disconnect() {
        if (socket != null) {
            try {
                socket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    //连接蓝牙设备
    public void connect(final BluetoothDevice dev) {
        this.createBond(dev);//连接之前需要先进行配对
        try {
            ParcelUuid[] uuids = dev.getUuids();
            UUID uuid = UUID.fromString("00001108-0000-1000-8000-00805f9b34fb");
            if (uuids != null) {
                uuid = uuids[0].getUuid();
                socket = dev.createRfcommSocketToServiceRecord(uuid);//使用从设备获取的uuid
            } else {
                UUID myuuid = UUID.fromString("00001108-0000-1000-8000-00805f9b34fb");//使用自定义的uuid
                socket = dev.createRfcommSocketToServiceRecord(myuuid);
            }
            System.out.println("uuid:" + uuid);
            if (socket != null)
                System.out.println(socket.getRemoteDevice().getName() + " socket succeed");
            Thread.sleep(500);
//            蓝牙连接要放在线程里,connect是阻塞操作
            new Thread() {
                @Override
                public void run() {
                    try {
                        socket.connect();
                        System.out.println("1.connect succeed");
                    } catch (Exception e) {
                        e.printStackTrace();
                        try {
                            UUID myuuid = UUID.fromString("00001108-0000-1000-8000-00805f9b34fb");//使用自定义的uuid
                            socket = dev.createRfcommSocketToServiceRecord(myuuid);
                            socket.connect();
                            System.out.println("2.connect succeed");
                        } catch (Exception f) {
                            f.printStackTrace();
                            try {
                                socket = (BluetoothSocket) dev.getClass().getMethod("createRfcommSocket", new Class[]{int.class}).invoke(dev, 1);//反射创建socke
                                socket.connect();
                                System.out.println("3.connect succeed");
                            } catch (Exception g) {
                                g.printStackTrace();
                            }
                        }
                    }
                }
            }.start();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public class AcceptThread extends Thread {
        private final BluetoothServerSocket mmServerSocket;

        public AcceptThread() {
            BluetoothServerSocket temp = null;
            try {
                temp = adapter.listenUsingRfcommWithServiceRecord("bluetooth", UUID.fromString("f000ccc1-0451-4000-b000-000000000000"));
            } catch (Exception e) {
                e.printStackTrace();
                System.out.println("server failed");
            }
            mmServerSocket = temp;
        }

        @Override
        public void run() {
            BluetoothSocket socket = null;
            if (mmServerSocket == null) {
                System.out.println("server socket null");
            } else {
                while (true) {
                    try {
                        System.out.println("accept start");
                        socket = mmServerSocket.accept();
                        if (socket != null) {
                            try {
                                mmServerSocket.close();
                                System.out.println("close called");
                                InputStream is = null;
                                try {
                                    is = socket.getInputStream();
                                } catch (Exception e) {
                                    e.printStackTrace();
                                }
                                System.out.println("get input succeed");
                                byte buf[] = new byte[1024];
                                int count;
                                String str;
                                while (true) {
                                    try {
                                        count = is.read(buf);
                                        if (count > 0) {
                                            str = new String(buf, 0, count);
                                            System.out.println(str);
                                        }
                                    } catch (Exception e) {
                                        e.printStackTrace();
                                    }
                                }
                            } catch (Exception e) {
                                e.printStackTrace();
                                System.out.println("close failed");
                                break;
                            }
                        } else {
                            System.out.println("socket null ");
                            break;
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                        System.out.println("accept failed");
                        break;
                    }


                }
            }
        }

        public void cancel() {
            try {
                mmServerSocket.close();
            } catch (Exception e) {
                e.printStackTrace();
                System.out.println("close failed");
            }
        }
    }

    //蓝牙搜索接收广播
    private class bluetoothReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String name, addr;

            String action = intent.getAction();
//            System.out.println(action);
            BluetoothDevice btDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

            switch (action) {
                case BluetoothDevice.ACTION_FOUND:
                    name = btDevice.getName();
                    addr = btDevice.getAddress();
                    if (btDevices.contains(btDevice) || name == null || name.isEmpty())
                        return;
                    btDevices.add(btDevice);
                    btDeviceStr.add(name + ":" + addr);
                    System.out.println(name + ":" + addr);
                    Intent myintent = new Intent(Action.BT_DISCOVERED);
                    myintent.putExtra(Action.DEVICE, btDevice);
                    context.sendBroadcast(myintent);
                    break;
                case BluetoothDevice.ACTION_PAIRING_REQUEST:
                    System.out.println("BluetoothDevice.ACTION_PAIRING_REQUEST");
                    /*try {
                        btDevice.setPairingConfirmation(true);
                    } catch (Exception e) {
                        e.printStackTrace();
                        System.out.println(" set fail ");
                    }*/

                    break;
                case BluetoothDevice.ACTION_BOND_STATE_CHANGED:
                    switch (btDevice.getBondState()) {
                        case BluetoothDevice.BOND_NONE:
                            System.out.println("BluetoothDevice.BOND_NONE");
                            break;
                        case BluetoothDevice.BOND_BONDING:
                            System.out.println("BluetoothDevice.BOND_BONDING");
                            break;
                        case BluetoothDevice.BOND_BONDED:
                            BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                            //audio.connect(device);
                            System.out.println("BluetoothDevice.BOND_BONDED");
                            break;
                    }
                    break;

                case BluetoothA2dp.ACTION_CONNECTION_STATE_CHANGED:
                    switch (intent.getIntExtra(BluetoothA2dp.EXTRA_STATE, -1)) {
                        case BluetoothA2dp.STATE_CONNECTED:
                            System.out.println("BluetoothA2dp.STATE_CONNECTED");
                            break;
                        case BluetoothA2dp.STATE_CONNECTING:
                            System.out.println("BluetoothA2dp.STATE_CONNECTING");
//                           建立蓝牙sco连接
//                            audio.startBluetoothSco();
                            break;
                        case BluetoothA2dp.STATE_DISCONNECTING:
                            System.out.println("BluetoothA2dp.STATE_DISCONNECTING");
                            break;
                        case BluetoothA2dp.STATE_DISCONNECTED:
                            System.out.println("BluetoothA2dp.STATE_DISCONNECTED");
                            break;

                    }
                    break;

                case BluetoothA2dp.ACTION_PLAYING_STATE_CHANGED:
                    switch (intent.getIntExtra(BluetoothA2dp.EXTRA_STATE, -1)) {
                        case BluetoothA2dp.STATE_PLAYING:
                            System.out.println("BluetoothA2dp.STATE_PLAYING");
                            break;
                        case BluetoothA2dp.STATE_NOT_PLAYING:
                            System.out.println("BluetoothA2dp.STATE_NOT_PLAYING");
                            break;
                    }
                    break;
            }
        }
    }
}
