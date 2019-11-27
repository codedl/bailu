package com.example.appzhuzhuang;

import android.bluetooth.BluetoothA2dp;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.ParcelUuid;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Set;
import java.util.UUID;

public class BluetoothClassic {
    private String tag = "BluetoothClassic";
    Context context;
    private BluetoothAdapter adapter;
    private BluetoothSocket socket;
    private bluetoothReceiver receiver;
    public ArrayList<BluetoothDevice> btDevices = new ArrayList<>();//发现的设备列表
    public ArrayList<String> btDeviceStr = new ArrayList<>();//用来显示到列表的字符串
    static int progress;//连接的进度


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

    //获取已经绑定的设备
    public Set<BluetoothDevice> getBondedDevices() {
        return BluetoothAdapter.getDefaultAdapter().getBondedDevices();
    }

    //检查设备是否已经连接,只对经典蓝牙协议有效
    public boolean isConnected(BluetoothDevice device) {

        if (getBondedDevices() != null && device != null)
            for (BluetoothDevice dev : getBondedDevices()) {
                if (dev.getAddress().equals(device.getAddress()))
                    return true;
            }
        return false;
    }


    //删掉已连接设备
    static void removeBond(BluetoothDevice device) {
        try {
            Method m = device.getClass().getMethod("removeBond", null);
            m.invoke(device, null);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    //搜索蓝牙设备
    public void startDiscovery() {
        btDevices.clear();
        btDeviceStr.clear();
        if (adapter.isDiscovering()) {
            adapter.cancelDiscovery();
        }
//        disconnect();
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
            Log.d("BluetoothClassic", "start bond");
            try {
                Method method = dev.getClass().getMethod("createBond");
                method.setAccessible(true);
                method.invoke(dev);
                progress = 5;

            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public void disconnect() {
        if (socket != null) {
            try {
                socket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        cancelDiscovery();
    }

    //连接蓝牙设备
    public void connect(final BluetoothDevice dev) {
        this.createBond(dev);//连接之前需要先进行配对
        try {
//            蓝牙连接要放在线程里,connect是阻塞操作
            new Thread() {
                @Override
                public void run() {
                    try {
                        UUID myuuid = UUID.fromString("00001108-0000-1000-8000-00805f9b34fb");//使用自定义的uuid
                        socket = dev.createRfcommSocketToServiceRecord(myuuid);
                        if (socket != null)
                            socket.connect();
                        progress = 90;
                        Log.d("BluetoothClassic", "1.connect succeed");
                    } catch (Exception e) {
                        e.printStackTrace();
                        try {
                            ParcelUuid[] uuids = dev.getUuids();
                            UUID uuid;
                            if (uuids != null) {
                                uuid = uuids[0].getUuid();
                                socket = dev.createRfcommSocketToServiceRecord(uuid);//使用从设备获取的uuid
                            }
                            if (socket != null)
                                socket.connect();
                            progress = 90;
                            Log.d("BluetoothClassic", "2.connect succeed");
                        } catch (Exception f) {
                            f.printStackTrace();
                            try {
                                //反射创建socket
                                socket = (BluetoothSocket) dev.getClass().getMethod("createRfcommSocket", new Class[]{int.class}).invoke(dev, 1);
                                socket.connect();
                                progress = 90;
                                Log.d("BluetoothClassic", "3.connect succeed");
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
                Log.d("BluetoothClassic", "server failed");
            }
            mmServerSocket = temp;
        }

        @Override
        public void run() {
            BluetoothSocket socket = null;
            if (mmServerSocket == null) {
                Log.d("BluetoothClassic", "server socket null");
            } else {
                while (true) {
                    try {
                        Log.d("BluetoothClassic", "accept start");
                        socket = mmServerSocket.accept();
                        if (socket != null) {
                            try {
                                mmServerSocket.close();
                                Log.d("BluetoothClassic", "close called");
                                InputStream is = null;
                                try {
                                    is = socket.getInputStream();
                                } catch (Exception e) {
                                    e.printStackTrace();
                                }
                                Log.d("BluetoothClassic", "get input succeed");
                                byte buf[] = new byte[1024];
                                int count;
                                String str;
                                while (true) {
                                    try {
                                        count = is.read(buf);
                                        if (count > 0) {
                                            str = new String(buf, 0, count);
                                            Log.d("BluetoothClassic", str);
                                        }
                                    } catch (Exception e) {
                                        e.printStackTrace();
                                    }
                                }
                            } catch (Exception e) {
                                e.printStackTrace();
                                Log.d("BluetoothClassic", "close failed");
                                break;
                            }
                        } else {
                            Log.d("BluetoothClassic", "socket null ");
                            break;
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                        Log.d("BluetoothClassic", "accept failed");
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
                Log.d("BluetoothClassic", "close failed");
            }
        }
    }

    //蓝牙搜索接收广播
    private class bluetoothReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String name, addr;

            String action = intent.getAction();
//            Log.d("BluetoothClassic",action);
            BluetoothDevice btDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

            switch (action) {
                case BluetoothDevice.ACTION_FOUND:
                    name = btDevice.getName();
                    addr = btDevice.getAddress();
                    if (btDevices.contains(btDevice) || name == null || name.isEmpty())
                        return;
                    btDevices.add(btDevice);
                    btDeviceStr.add(name + ":" + addr);
                    Log.d("BluetoothClassic", name + ":" + addr);

                    Intent myintent = new Intent(Action.BT_DISCOVERED);
                    myintent.putExtra(Action.DEVICE, btDevice);
                    context.sendBroadcast(myintent);

                    break;
                case BluetoothDevice.ACTION_PAIRING_REQUEST:
                    Log.d("BluetoothClassic", "BluetoothDevice.ACTION_PAIRING_REQUEST");
                    progress = 15;
                    /*try {
                        btDevice.setPairingConfirmation(true);
                    } catch (Exception e) {
                        e.printStackTrace();
                        Log.d("BluetoothClassic"," set fail ");
                    }*/

                    break;
                case BluetoothDevice.ACTION_BOND_STATE_CHANGED:
                    switch (btDevice.getBondState()) {
                        case BluetoothDevice.BOND_NONE:
                            Log.d("BluetoothClassic", "BluetoothDevice.BOND_NONE");
                            break;
                        case BluetoothDevice.BOND_BONDING:
                            Log.d("BluetoothClassic", "BluetoothDevice.BOND_BONDING");
                            progress = 10;
                            break;
                        case BluetoothDevice.BOND_BONDED:
                            BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                            //audio.connect(device);
                            Log.d("BluetoothClassic", "BluetoothDevice.BOND_BONDED");
                            progress = 20;
                            break;
                    }
                    break;

                case BluetoothA2dp.ACTION_CONNECTION_STATE_CHANGED:
                    switch (intent.getIntExtra(BluetoothA2dp.EXTRA_STATE, -1)) {
                        case BluetoothA2dp.STATE_CONNECTED:
                            progress = 100;
                            Log.d("BluetoothClassic", "BluetoothA2dp.STATE_CONNECTED");
                            break;
                        case BluetoothA2dp.STATE_CONNECTING:
                            Log.d("BluetoothClassic", "BluetoothA2dp.STATE_CONNECTING");
//                           建立蓝牙sco连接
//                            audio.startBluetoothSco();
                            break;
                        case BluetoothA2dp.STATE_DISCONNECTING:
                            Log.d("BluetoothClassic", "BluetoothA2dp.STATE_DISCONNECTING");
                            break;
                        case BluetoothA2dp.STATE_DISCONNECTED:
                            Log.d("BluetoothClassic", "BluetoothA2dp.STATE_DISCONNECTED");
                            break;

                    }
                    break;

                case BluetoothA2dp.ACTION_PLAYING_STATE_CHANGED:
                    switch (intent.getIntExtra(BluetoothA2dp.EXTRA_STATE, -1)) {
                        case BluetoothA2dp.STATE_PLAYING:
                            Log.d("BluetoothClassic", "BluetoothA2dp.STATE_PLAYING");
                            break;
                        case BluetoothA2dp.STATE_NOT_PLAYING:
                            Log.d("BluetoothClassic", "BluetoothA2dp.STATE_NOT_PLAYING");
                            progress = 100;
                            break;
                    }
                    break;
            }
        }
    }
}
