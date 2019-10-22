package com.example.myapplication;

import android.bluetooth.BluetoothA2dp;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;

public class MyReceiver extends BroadcastReceiver {
    public MyReceiver() {

    }

    Context context;
    ArrayList<BluetoothDevice> devices = new ArrayList<>();
    ArrayList<String> deviceStr = new ArrayList<>();
    ListView listView;
    private ArrayAdapter adapter;
    audio Audio;


    /*public MyReceiver(Context context, audio Audio) {
        this.context = context;
        this.Audio = Audio;
        LayoutInflater layout = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View view = layout.inflate(R.layout.activity_bt, null);
        listView = view.findViewById(R.id.list);
        Button textView = view.findViewById(R.id.play_btn);
        textView.setText("接收广播");
        adapter = new ArrayAdapter(context, android.R.layout.simple_expandable_list_item_1, deviceStr);
    }*/

    @Override
    public void onReceive(Context context, Intent intent) {
        String name, addr;

        String action = intent.getAction();
        Log.d("MyReceiver", action);
        BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
        switch (action) {
            case Action.BLE_SCAN_FOUND:
                String str = intent.getStringExtra("devicestr");
                Log.d("MyReceiver",str);
                deviceStr.add(str);
                listView.setAdapter(adapter);
                adapter.notifyDataSetChanged();//更新
                break;
            case BluetoothDevice.ACTION_FOUND:
                name = device.getName();
                addr = device.getAddress();
                Log.d("MyReceiver",name + addr);

                if (devices.contains(device))
                    return;
                deviceStr.add(name + ":" + addr);
                devices.add(device);
                listView.setAdapter(adapter);
                adapter.notifyDataSetChanged();//更新

                break;
            case BluetoothDevice.ACTION_PAIRING_REQUEST:
                Log.d("MyReceiver","BluetoothDevice.ACTION_PAIRING_REQUEST");
                /*try {
                    btDevice.setPairingConfirmation(true);
                } catch (Exception e) {
                    e.printStackTrace();
                    Log.d("MyReceiver"," set fail ");
                }*/
                break;
            case BluetoothDevice.ACTION_BOND_STATE_CHANGED:
                switch (device.getBondState()) {
                    case BluetoothDevice.BOND_NONE:
                        Log.d("MyReceiver","BluetoothDevice.BOND_NONE");
                        break;
                    case BluetoothDevice.BOND_BONDING:
                        Log.d("MyReceiver","BluetoothDevice.BOND_BONDING");
                        break;
                    case BluetoothDevice.BOND_BONDED:
                        Log.d("MyReceiver","BluetoothDevice.BOND_BONDED");
                        break;
                }
                break;

            case BluetoothA2dp.ACTION_CONNECTION_STATE_CHANGED:
                switch (intent.getIntExtra(BluetoothA2dp.EXTRA_STATE, -1)) {
                    case BluetoothA2dp.STATE_CONNECTED:
                        Log.d("MyReceiver","BluetoothA2dp.STATE_CONNECTED");
                        break;
                    case BluetoothA2dp.STATE_CONNECTING:
                        Log.d("MyReceiver","BluetoothA2dp.STATE_CONNECTING");
//                           建立蓝牙sco连接
                        Audio.startBluetoothSco();
                        break;
                    case BluetoothA2dp.STATE_DISCONNECTING:
                        Log.d("MyReceiver","BluetoothA2dp.STATE_DISCONNECTING");
                        break;
                    case BluetoothA2dp.STATE_DISCONNECTED:
                        Log.d("MyReceiver","BluetoothA2dp.STATE_DISCONNECTED");
                        break;

                }
                break;

            case BluetoothA2dp.ACTION_PLAYING_STATE_CHANGED:
                switch (intent.getIntExtra(BluetoothA2dp.EXTRA_STATE, -1)) {
                    case BluetoothA2dp.STATE_PLAYING:
                        Log.d("MyReceiver","BluetoothA2dp.STATE_PLAYING");
                        break;
                    case BluetoothA2dp.STATE_NOT_PLAYING:
                        Log.d("MyReceiver","BluetoothA2dp.STATE_NOT_PLAYING");
                        break;
                }
                break;
        }
    }
}
