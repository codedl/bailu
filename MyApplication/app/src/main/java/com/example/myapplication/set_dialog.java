package com.example.myapplication;

import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;

public class set_dialog implements View.OnClickListener, ListView.OnItemSelectedListener {
    private View view;
    private Button phone;
    private Button data;
    private ListView list;
    private ArrayList<String> btArray = new ArrayList<>();
    private ArrayList<String> bleArray = new ArrayList<>();
    private ArrayAdapter btAdapter;
    private ArrayAdapter bleAdapter;
    private BluetoothLowEnergy blePhone;
    private BluetoothClassic btData;
    private recevier recevier;
    private boolean isBLE;

    public set_dialog(Context context) {
//找到布局
        view = LayoutInflater.from(context).inflate(R.layout.menu_zhuz, null);
        phone = view.findViewById(R.id.phone);
        phone.setOnClickListener(this);
        data = view.findViewById(R.id.data);
        data.setOnClickListener(this);

        list = view.findViewById(R.id.list);
        btAdapter = new ArrayAdapter(context, android.R.layout.simple_list_item_single_choice, btArray);
        bleAdapter = new ArrayAdapter(context, android.R.layout.simple_list_item_single_choice, bleArray);

        blePhone = new BluetoothLowEnergy(context);
        btData = new BluetoothClassic(context);

        IntentFilter filter = new IntentFilter();
        filter.addAction(Action.BLE_SCAN_FOUND);
        filter.addAction(Action.BT_DISCOVERED);
        recevier = new recevier();
        context.registerReceiver(recevier, filter);
    }

    public View getView() {
        return this.view;
    }

    @Override
    public void onClick(View view) {

        switch (view.getId()) {
            case R.id.phone:
                isBLE = true;
                blePhone.startScan();
                btData.cancelDiscovery();
                list.setAdapter(bleAdapter);
                break;
            case R.id.data:
                isBLE = false;
                btData.startDiscovery();
                blePhone.stopScan();
                list.setAdapter(btAdapter);
                break;
        }
    }

    @Override
    public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {

    }

    @Override
    public void onNothingSelected(AdapterView<?> adapterView) {

    }

    private class recevier extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            BluetoothDevice device = intent.getParcelableExtra(Action.DEVICE);
            System.out.println(action);
            String addStr = device.getName() + "\n" + device.getAddress();
            System.out.println(addStr);
            switch (action) {
                case Action.BLE_SCAN_FOUND:
                    if (bleArray.contains(addStr))
                        return;
                    else
                        bleArray.add(addStr);
                    list.setAdapter(bleAdapter);
                    break;
                case Action.BT_DISCOVERED:
                    if (btArray.contains(addStr))
                        return;
                    else
                        btArray.add(addStr);
                    list.setAdapter(btAdapter);
                    break;
            }

        }
    }

}
