package com.example.appzhuzhuang;

import android.app.AlertDialog;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;

public class set_dialog implements View.OnClickListener, AdapterView.OnItemClickListener, AdapterView.OnItemLongClickListener {
    private String tag = "set_dialog";
    Context context;
    private View view;
    private Button phone;
    private Button data;
    private Button bond;
    private Button btn_data;
    private Button btn_phone;
    private TextView txt_ble;
    private TextView txt_bt;
    private ListView list;
    private ArrayList<String> btArray = new ArrayList<>();
    private ArrayList<BluetoothDevice> btDevices = new ArrayList<>();
    private ArrayList<String> bleArray = new ArrayList<>();
    private ArrayList<BluetoothDevice> bleDevices = new ArrayList<>();
    private ArrayList<String> bondArray = new ArrayList<>();
    private ArrayList<BluetoothDevice> bondDevices = new ArrayList<>();
    private ArrayAdapter btAdapter;
    private ArrayAdapter bleAdapter;
    private ArrayAdapter bondAdapter;
    private recevier recevier;
    //标记按键，用于列表监听事件
    private boolean isBLE;//判断设置的对象为耳机还是数据
    private boolean isBT;
    private boolean isBond;
    private BluetoothDevice bleTmp;
    private BluetoothDevice btTmp;

    public set_dialog(Context context) {
        this.context = context;
//找到布局
        view = LayoutInflater.from(context).inflate(R.layout.menu_zhuz, null);
        phone = view.findViewById(R.id.phone);
        phone.setOnClickListener(this);
        data = view.findViewById(R.id.data);
        data.setOnClickListener(this);
        bond = view.findViewById(R.id.bond);
        bond.setOnClickListener(this);
        txt_bt = view.findViewById(R.id.txt_phone);
        btn_data = view.findViewById(R.id.btn_data);
        btn_data.setOnClickListener(this);
        btn_phone = view.findViewById(R.id.btn_phone);
        btn_phone.setOnClickListener(this);
        if (param.btPhone == null)
            txt_bt.setText("耳机:未设置");
        else
            txt_bt.setText("耳机:" + param.btPhone.getName() + "(" + param.btPhone.getAddress() + ")");
        txt_ble = view.findViewById(R.id.txt_data);
        if (param.bleData == null)
            txt_ble.setText("数据:未设置");
        else
            txt_ble.setText("数据:" + param.bleData.getName() + "(" + param.bleData.getAddress() + ")");

        list = view.findViewById(R.id.list);
        list.setOnItemClickListener(this);
        list.setOnItemLongClickListener(this);

        btAdapter = new ArrayAdapter(context, android.R.layout.simple_list_item_single_choice, btArray);
        bleAdapter = new ArrayAdapter(context, android.R.layout.simple_list_item_single_choice, bleArray);
        bondAdapter = new ArrayAdapter(context, android.R.layout.simple_list_item_single_choice, bondArray);
        btArray.clear();
        btDevices.clear();
        bleArray.clear();
        bleDevices.clear();

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
                isBT = true;
                isBLE = false;
                isBond = false;
                param.btc.startDiscovery();
                param.ble.stopScan();
                list.setAdapter(btAdapter);
                break;
            case R.id.data:
                isBLE = true;
                isBT = false;
                isBond = false;
                param.ble.startScan();
                param.btc.cancelDiscovery();
                list.setAdapter(bleAdapter);
                break;

            case R.id.bond:
                isBond = true;
                isBT = false;
                isBLE = false;
                bondDevices.clear();
                bondArray.clear();
                //遍历连接设备，添加到列表
                if (param.btc.getBondedDevices() != null)
                    for (BluetoothDevice device : param.btc.getBondedDevices()) {
                        bondArray.add(device.getName() + "\n" + device.getAddress());
                        bondDevices.add(device);
                    }
                //检查ble是否有已经连接的设备
                if (param.ble.getConnectedDevices() != null) {
                    bondArray.add(param.ble.connectDevice.getName() + "\n" + param.ble.connectDevice.getAddress());
                    bondDevices.add(param.ble.connectDevice);
                }
                list.setAdapter(bondAdapter);
                break;
            case R.id.btn_data:
                if (param.bleData != null || bleTmp != null) {
                    param.bleData = null;
                    bleTmp = null;
                    txt_ble.setText("数据:未设置");
                }
                break;
            case R.id.btn_phone:
                if (param.btPhone != null || btTmp != null) {
                    param.btPhone = null;
                    btTmp = null;
                    txt_bt.setText("耳机:未设置");
                }
                break;
        }
    }

    protected boolean BUTTON_POSITIVE_Event() {
        if (btTmp == null && bleTmp == null)
            return false;
        //注销资源
        param.ble.stopScan();
        param.btc.cancelDiscovery();
        btArray.clear();
        btDevices.clear();
        bleArray.clear();
        bleDevices.clear();
        context.unregisterReceiver(recevier);
        //判断是否设置了蓝牙连接对象

        if (bleTmp != null)
            param.bleData = bleTmp;
        if (btTmp != null)
            param.btPhone = btTmp;
        return true;
    }

    protected void BUTTON_NEGATIVE_Event() {
        param.ble.stopScan();
        param.btc.cancelDiscovery();
        btArray.clear();
        btDevices.clear();
        bleArray.clear();
        bleDevices.clear();
        context.unregisterReceiver(recevier);
    }

    @Override
    public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
        if (isBLE == true) {
            bleTmp = bleDevices.get(i);
            txt_ble.setText("数据:" + bleTmp.getName() + "(" + bleTmp.getAddress() + ")");
        } else if (isBT == true) {
            btTmp = btDevices.get(i);
            txt_bt.setText("耳机:" + btTmp.getName() + "(" + btTmp.getAddress() + ")");
        }
    }

    //长按删除设备
    @Override
    public boolean onItemLongClick(AdapterView<?> adapterView, View view, final int i, long l) {
        //删除已经连接设备
        if (isBond) {
            final dialog dialog = new dialog(context);
            dialog.create("是否删除", null);
            dialog.setButtonListener(AlertDialog.BUTTON_POSITIVE, new View.OnClickListener() {
                @Override
                public void onClick(View view) {

                    BluetoothClassic.removeBond(bondDevices.get(i));
                    if (param.ble.connectDevice != null &&
                            param.ble.connectDevice.getAddress().equals(bondDevices.get(i).getAddress())) {
                        param.ble.connectDevice = null;
                    }
                    bondDevices.remove(i);
                    bondArray.remove(i);
                    list.setAdapter(bondAdapter);
                    dialog.dismiss();
                }
            });
        }
        return true;
    }

    private class recevier extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            BluetoothDevice device = intent.getParcelableExtra(Action.DEVICE);
            Log.d("set_dialog", action);
            String addStr = device.getName() + "\n" + device.getAddress();
            switch (action) {
                case Action.BLE_SCAN_FOUND:
                    if (bleArray.contains(addStr))
                        return;
                    bleArray.add(addStr);
                    bleDevices.add(device);
                    list.setAdapter(bleAdapter);
                    break;
                case Action.BT_DISCOVERED:
                    if (btArray.contains(addStr))
                        return;
                    btArray.add(addStr);
                    btDevices.add(device);
                    list.setAdapter(btAdapter);
                    break;
            }

        }
    }

}
