package com.example.myapplication;

import android.bluetooth.BluetoothA2dp;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;

public class MyReceiver extends BroadcastReceiver {
    Context context;
    ArrayList<BluetoothDevice> devices = new ArrayList<>();
    ArrayList<String> deviceStr = new ArrayList<>();
    ListView listView;
    private ArrayAdapter adapter;
    audio Audio;


    public MyReceiver(Context context, audio Audio) {
        this.context = context;
        this.Audio = Audio;
        LayoutInflater layout = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View view = layout.inflate(R.layout.activity_bt, null);
        listView = view.findViewById(R.id.list);
        Button textView = view.findViewById(R.id.play_btn);
        textView.setText("接收广播");
        adapter = new ArrayAdapter(context, android.R.layout.simple_expandable_list_item_1, deviceStr);
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        String name, addr;

        String action = intent.getAction();
        System.out.println(action);
        BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

        switch (action) {
            case Action.BLE_SCAN_FOUND:
                String str = intent.getStringExtra("devicestr");
                System.out.println(str);
                deviceStr.add(str);
                listView.setAdapter(adapter);
                adapter.notifyDataSetChanged();//更新
                break;
            case BluetoothDevice.ACTION_FOUND:
                name = device.getName();
                addr = device.getAddress();
                System.out.println(name + addr);
               /* Message msg = new btActivity().bthandle.obtainMessage();
                msg.what = 1;
                new btActivity().bthandle.sendMessage(msg);*/
                if (devices.contains(device))
                    return;
                deviceStr.add(name + ":" + addr);
                devices.add(device);
                listView.setAdapter(adapter);
                adapter.notifyDataSetChanged();//更新

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
                switch (device.getBondState()) {
                    case BluetoothDevice.BOND_NONE:
                        System.out.println("BluetoothDevice.BOND_NONE");
                        break;
                    case BluetoothDevice.BOND_BONDING:
                        System.out.println("BluetoothDevice.BOND_BONDING");
                        break;
                    case BluetoothDevice.BOND_BONDED:
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
                        Audio.startBluetoothSco();
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
