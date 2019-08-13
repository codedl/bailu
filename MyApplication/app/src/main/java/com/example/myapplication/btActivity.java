package com.example.myapplication;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.location.LocationManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import android.view.View;
import android.media.AudioManager;
import android.provider.Settings;
import android.bluetooth.BluetoothServerSocket;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Set;
import java.util.UUID;

public class btActivity extends AppCompatActivity {
    public BluetoothAdapter BA;
    public BluetoothSocket btSocket;
    public BluetoothDevice btDevice;
    bluetoothReceiver btReceiver;
    String pin = "1234";

    private Switch blOfSwitch;
    private Switch btSchwitch;
    private ArrayAdapter adapter;
    private ListView listView;
    private EditText destNameEdit;

    public ArrayList<BluetoothDevice> devices = new ArrayList<>();
    public ArrayList<String> deviceName = new ArrayList<>();
    private String destName = new String("HWP");
    private String destAddr = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bt);
        btInit();
    }

    public void btInit() {
        blOfSwitch = (Switch) findViewById(R.id.btof_switch);
        btSchwitch = (Switch) findViewById(R.id.btSearch_switch);
        destNameEdit = (EditText) findViewById(R.id.btDestName_edit);
        listView = (ListView) findViewById(R.id.list);
        adapter = new ArrayAdapter(btActivity.this, android.R.layout.simple_expandable_list_item_1, deviceName);

        BA = BluetoothAdapter.getDefaultAdapter();

        IntentFilter intentFilter = new IntentFilter(BluetoothDevice.ACTION_FOUND);//注册广播接收信号
        intentFilter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        intentFilter.addAction(BluetoothDevice.ACTION_PAIRING_REQUEST);
        btReceiver = new bluetoothReceiver();
        registerReceiver(btReceiver, intentFilter);//用BroadcastReceiver 来取得结果

        new AcceptThread().start();

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                BluetoothDevice device = null;
                device = devices.get(i);
                Toast.makeText(btActivity.this, devices.get(i).getName() + devices.get(i).getAddress(), Toast.LENGTH_LONG).show();
                new ConnectThread(device).start();
            }
        });
    }

    public class bluetoothReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String name, addr;

            String action = intent.getAction();
            System.out.println(action);
            btDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

            switch (action) {
                case BluetoothDevice.ACTION_FOUND:
                    name = btDevice.getName();
                    addr = btDevice.getAddress();

                    for (int i = 0; i < devices.size(); i++) {
                        if (devices.get(i).getName().equals(name)) {
                            return;
                        }
                    }
                    deviceName.add(name + ":" + addr);
                    devices.add(btDevice);
                    listView.setAdapter(adapter);
                    adapter.notifyDataSetChanged();//更新

                   /* if (destName.equals(name)) {
                        BA.cancelDiscovery();
                        destAddr = btDevice.getAddress();//获取设备连接地址
                    }*/
                    break;
                case BluetoothDevice.ACTION_PAIRING_REQUEST:
                    System.out.println("start set pin ");

                    /*try {
                        abortBroadcast();
                        if (ClsUtils.setPin(device.getClass(), device, pin)) {
                            System.out.println(" pin true");
                        }
                        //ClsUtils.setPairingConfirmation(device.getClass(), device, true);//有问题会进入异常

                    } catch (Exception e) {
                        e.printStackTrace();
                        System.out.println(" set fail ");
                    }
                    System.out.println(" set over ");*/

                    break;
                case BluetoothDevice.ACTION_BOND_STATE_CHANGED:
                    switch (btDevice.getBondState()) {
                        case BluetoothDevice.BOND_NONE:
                            System.out.println("bond canceled");
                            Toast.makeText(btActivity.this, "取消配对", Toast.LENGTH_SHORT).show();

                            break;
                        case BluetoothDevice.BOND_BONDING:
                            System.out.println("bond ing...");
                            Toast.makeText(btActivity.this, "正在配对", Toast.LENGTH_SHORT).show();

                            break;
                        case BluetoothDevice.BOND_BONDED:
                            System.out.println("bond succeed");
                            Toast.makeText(btActivity.this, "配对成功", Toast.LENGTH_SHORT).show();

                            break;
                    }
                    break;
            }
        }
    }

    ;

    public void switchEvent(View btSwitch) {
        switch (btSwitch.getId()) {
            case R.id.btof_switch:
                if (blOfSwitch.isChecked()) {
                    if (!BA.isEnabled()) {
                        Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                        startActivityForResult(turnOn, 1);
                        Toast.makeText(btActivity.this, "蓝牙打开", Toast.LENGTH_SHORT).show();

                    } else {
                        Toast.makeText(btActivity.this, "蓝牙已经打开", Toast.LENGTH_SHORT).show();
                    }

                } else {
                    BA.disable();
                    Toast.makeText(btActivity.this, "蓝牙关闭", Toast.LENGTH_SHORT).show();
                }
                break;
            case R.id.btSearch_switch:
                if (btSchwitch.isChecked()) {
                    if (destNameEdit.getText().toString().trim().length() != 0) {
                        destName = destNameEdit.getText().toString().trim();
                    }
                    if (BA.isDiscovering()) {
                        BA.cancelDiscovery();
                    }
                    BA.startDiscovery();
                    Toast.makeText(btActivity.this, "搜索蓝牙\n" + destName, Toast.LENGTH_SHORT).show();

                } else {
                    if (BA.isDiscovering()) {
                        BA.cancelDiscovery();
                    }
                    devices.clear();
                    deviceName.clear();
                    listView.setAdapter(adapter);
                    Toast.makeText(btActivity.this, "搜索蓝牙结束", Toast.LENGTH_SHORT).show();

                }
        }
    }

    public void btEvent(View btn) {
        switch (btn.getId()) {
            case R.id.btVisible_btn:
                if (BA.getScanMode() != BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE) {
                    Intent btdisIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
                    btdisIntent.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 300);
                    startActivity(btdisIntent);
                }
                break;

            case R.id.btConnect_btn:

                if (btDevice != null) {
                    Toast.makeText(btActivity.this, destName + "\n" + destAddr, Toast.LENGTH_SHORT).show();
//                    new ConnectThread().start();
                } else {
                }
                Toast.makeText(btActivity.this, "需要搜索蓝牙", Toast.LENGTH_SHORT).show();
                if (btSocket != null) {
                    if (btSocket.isConnected()) {
                        System.out.println("connect succeed");
                        Toast.makeText(btActivity.this, "连接成功", Toast.LENGTH_SHORT).show();
                    } else {
                        System.out.println("connect failed ");
                        Toast.makeText(btActivity.this, "连接失败", Toast.LENGTH_SHORT).show();
                    }
                }
                break;
            case R.id.btPaired_btn:
                if (BA.isDiscovering()) {
                    BA.cancelDiscovery();
                }
                if (btDevice == null) {
                    Toast.makeText(this, "设备为空", Toast.LENGTH_SHORT);
                }
                if (btDevice.getBondState() == BluetoothDevice.BOND_NONE) {

                    try {
                        boolean creatbool = ClsUtils.createBond(btDevice.getClass(), btDevice);
                        System.out.println("createbond  :" + creatbool);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }

                } else {
                    Toast.makeText(this, "已经配对", Toast.LENGTH_SHORT);
                }
                break;
            case R.id.send_btn:

                break;
            case R.id.rcv_btn:

                break;
        }
    }


    protected void onDestroy() {
        super.onDestroy();//解除注册
        unregisterReceiver(btReceiver);
    }


    public class ConnectThread extends Thread {
        BluetoothDevice dev;

        public ConnectThread(BluetoothDevice tmp) {
            this.dev = tmp;
            try {
                System.out.println("try socket link");
                btSocket = dev.createRfcommSocketToServiceRecord(UUID.fromString("f000ccc1-0451-4000-b000-000000000000"));
            } catch (Exception e) {
                e.printStackTrace();
                System.out.println("client socket failed");
            }
        }

        @Override
        public void run() {
            BA.cancelDiscovery();
            byte[] buf = new byte[10];
            try {
                btSocket.connect();
                System.out.println("client connect succeed");
                OutputStream os = null;
                try {
                    os = btSocket.getOutputStream();
                } catch (Exception e) {
                    e.printStackTrace();
                }
                System.out.println("get output succeed");
                try {
                    os.write(new String("hello").getBytes("UTF-8"));
                } catch (Exception e) {
                    e.printStackTrace();
                    System.out.println("write failed");
                }

            } catch (Exception e) {
                System.out.println("client connect failed");
                try {
                    btSocket.close();
                } catch (Exception f) {
                    f.printStackTrace();
                    System.out.println("client close failed");
                }
                return;
            }
        }

        public void cancel() {
            try {
                btSocket.close();
            } catch (Exception e) {
                e.printStackTrace();
                System.out.println("client close failed");
            }
        }
    }

    public class AcceptThread extends Thread {
        private final BluetoothServerSocket mmServerSocket;

        public AcceptThread() {
            BluetoothServerSocket temp = null;
            try {
                temp = BA.listenUsingRfcommWithServiceRecord("bluetooth", UUID.fromString("f000ccc1-0451-4000-b000-000000000000"));
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
