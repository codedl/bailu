package com.example.myapplication;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.bluetooth.BluetoothA2dp;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.location.LocationManager;
import android.media.MediaRecorder;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.ParcelUuid;
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

import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Set;
import java.util.UUID;

public class btActivity extends AppCompatActivity {
    public BluetoothAdapter BA;
    public BluetoothA2dp bluetoothA2dp;
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
    private MediaRecorder mediaRecorder;
    private AudioManager audioManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bt);
        btInit();
    }

    public void btInit() {
        audioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        blOfSwitch = (Switch) findViewById(R.id.btof_switch);
        btSchwitch = (Switch) findViewById(R.id.btSearch_switch);
        destNameEdit = (EditText) findViewById(R.id.btDestName_edit);
        listView = (ListView) findViewById(R.id.list);
        adapter = new ArrayAdapter(btActivity.this, android.R.layout.simple_expandable_list_item_1, deviceName);

        BA = BluetoothAdapter.getDefaultAdapter();
        BA.getProfileProxy(this, new BluetoothProfile.ServiceListener() {
            @Override
            public void onServiceConnected(int i, BluetoothProfile bluetoothProfile) {
                if (i == BluetoothA2dp.A2DP) {
                    bluetoothA2dp = (BluetoothA2dp) bluetoothProfile;
                    System.out.println("a2dp get succeed");
                }
            }

            @Override
            public void onServiceDisconnected(int i) {
                bluetoothA2dp = null;
                System.out.println("a2dp put");
            }
        }, BluetoothA2dp.A2DP);
        IntentFilter intentFilter = new IntentFilter(BluetoothDevice.ACTION_FOUND);//注册广播接收信号
        intentFilter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        intentFilter.addAction(BluetoothDevice.ACTION_PAIRING_REQUEST);
        intentFilter.addAction(BluetoothA2dp.ACTION_CONNECTION_STATE_CHANGED);
        intentFilter.addAction(BluetoothA2dp.ACTION_PLAYING_STATE_CHANGED);
        intentFilter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        intentFilter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        btReceiver = new bluetoothReceiver();
        registerReceiver(btReceiver, intentFilter);//用BroadcastReceiver 来取得结果

        //new AcceptThread().start();

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                btDevice = devices.get(i);
                System.out.println(btDevice.getName() + btDevice.getAddress());
                new ConnectThread(btDevice).start();
            }
        });
    }

    public void connect() {
        if (btDevice == null) {
            return;
        }
        if (bluetoothA2dp == null) {
            return;
        }
        try {
            Method method = bluetoothA2dp.getClass().getMethod("connect", BluetoothDevice.class);
            method.setAccessible(true);
            method.invoke(bluetoothA2dp, btDevice);

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("method exception");
        }
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
                    System.out.println(name + addr);
                    if (devices.contains(btDevice))
                        return;
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
                            System.out.println("BluetoothDevice.BOND_NONE");
                            break;
                        case BluetoothDevice.BOND_BONDING:
                            System.out.println("BluetoothDevice.BOND_BONDING");
                            break;
                        case BluetoothDevice.BOND_BONDED:
                            connect();
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
        String file = Environment.getExternalStorageDirectory().getAbsolutePath();
        file += "/record.3gp";
        System.out.println(file);
        switch (btn.getId()) {
            case R.id.start_btn:
                mediaRecorder = new MediaRecorder();
                mediaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
                mediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
                mediaRecorder.setOutputFile(file);
                mediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);

                if (audioManager.isBluetoothScoAvailableOffCall()) {
                    System.out.println("support sco");
                }
                try {
                    mediaRecorder.prepare();
                } catch (Exception e) {
                    e.printStackTrace();
                }
                audioManager.stopBluetoothSco();
                audioManager.startBluetoothSco();
                registerReceiver(new BroadcastReceiver() {
                    @Override
                    public void onReceive(Context context, Intent intent) {
                        int state = intent.getIntExtra(AudioManager.EXTRA_SCO_AUDIO_STATE, -1);
                        if (AudioManager.SCO_AUDIO_STATE_CONNECTED == state) {
                            audioManager.setBluetoothScoOn(true);
                            audioManager.setMode(AudioManager.STREAM_MUSIC);
                            mediaRecorder.start();
                            unregisterReceiver(this);
                            System.out.println("start record");
                        } else {
                            try {
                                Thread.sleep(1000);
                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                            audioManager.startBluetoothSco();
                            System.out.println("start record again");
                        }
                    }
                }, new IntentFilter(AudioManager.ACTION_SCO_AUDIO_STATE_UPDATED));
                break;

            case R.id.stop_btn:
                mediaRecorder.stop();
                mediaRecorder.release();
                mediaRecorder = null;
                if (audioManager.isBluetoothScoOn()) {
                    audioManager.setBluetoothScoOn(false);
                    audioManager.stopBluetoothSco();
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
        BluetoothSocket socket;

        public ConnectThread(BluetoothDevice tmp) {
            this.dev = tmp;
        }

        @Override
        public void run() {
            BA.cancelDiscovery();

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
                sleep(500);
//连接蓝牙放在子线程中
                new Thread() {
                    @Override
                    public void run() {
                        try {
                            socket.connect();
                            System.out.println("connect1 succeed");

                        } catch (IOException e) {
                            try {
                                UUID myuuid = UUID.fromString("00001108-0000-1000-8000-00805f9b34fb");//使用自定义的uuid
                                socket = dev.createRfcommSocketToServiceRecord(myuuid);
                                socket.connect();
                                System.out.println("connect2 succeed");

                            } catch (Exception f) {
                                try {
                                    socket = (BluetoothSocket) dev.getClass().getMethod("createRfcommSocket", new Class[]{int.class}).invoke(dev, 1);//反射创建socke
                                    socket.connect();
                                    System.out.println("connect3 succeed");
                                } catch (Exception g) {
                                    g.printStackTrace();
                                }
                                f.printStackTrace();
                            }
                        }
                        try {
                            InputStream inputStream = socket.getInputStream();
                            DataInputStream in = new DataInputStream(inputStream);
                            int count;
                            byte[] buf = new byte[1024];

                            try {
                                while (true) {
                                    System.out.println("reading");
                                    System.out.println(in.readInt());
                                    System.out.println("data read:");

                                    switch (in.readInt()) {
                                    }
                                }

                            } catch (Exception e) {
                                e.printStackTrace();
                                System.out.println("get in exception");
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                        }

                    }
                }.start();
            } catch (Exception e) {
                e.printStackTrace();
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
