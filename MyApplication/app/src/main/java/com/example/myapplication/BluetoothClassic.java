package com.example.myapplication;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.os.ParcelUuid;
import java.io.InputStream;
import java.lang.reflect.Method;
import java.util.UUID;

public class BluetoothClassic {
    Context context;
    BluetoothAdapter adapter;
    BluetoothSocket socket;

    public BluetoothClassic(Context context) {
        this.context = context;
        adapter = BluetoothAdapter.getDefaultAdapter();
    }

    public void startDiscovery() {
        if (adapter.isDiscovering()) {
            adapter.cancelDiscovery();
        }
        adapter.startDiscovery();
    }

    public void cancelDiscovery() {
        if (adapter.isDiscovering()) {
            adapter.cancelDiscovery();
        }
    }

    public void createBond(BluetoothDevice dev) {
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

    public void connect(final BluetoothDevice dev) {
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
}
