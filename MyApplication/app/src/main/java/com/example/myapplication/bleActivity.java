package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.bluetooth.BluetoothAdapter.LeScanCallback;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattServer;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Xml;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

public class bleActivity extends AppCompatActivity {
    private Switch ofSwitch;
    private Switch scanSwitch;
    private Switch connectSwitch;
    private EditText sendEdit;
    private EditText rcvEdit;
    private Button sendBtn;
    private Button rcvBtn;
    private Button led0btn;
    private Button led1btn;
    private TextView devText;
    private ListView devList;
    private ArrayList<BluetoothDevice> devices = new ArrayList<>();
    private ArrayList<String> deviceStr = new ArrayList<>();
    private ArrayAdapter adapter;

    private String bleName = new String("ProjectZero");
    public BluetoothAdapter bleAdapter;
    public BluetoothDevice bleDevice = null;
    public BluetoothGatt bleGatt;
    public BluetoothGattCharacteristic led0;
    public BluetoothGattCharacteristic led1;
    public BluetoothGattCharacteristic btnbgc;

    private handle bleHandle;

    private boolean iscalled = true;//回调完成
    boolean led0on;
    boolean led1on;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ble);
        bleInit();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        bleClose();
    }

    public void bleInit() {
//        打开蓝牙
        Intent btEnable = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        startActivityForResult(btEnable, 1);
        //判断是否支持蓝牙
        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Toast.makeText(this, "不支持蓝牙ble", Toast.LENGTH_SHORT).show();
            finish();
        }
        BluetoothManager bm = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        bleAdapter = bm.getAdapter();
        if (bleAdapter == null) {
            Toast.makeText(this, "无法打开蓝牙适配器", Toast.LENGTH_SHORT).show();
        }
        ofSwitch = (Switch) findViewById(R.id.bleswitch);
        scanSwitch = (Switch) findViewById(R.id.scanswitch);
        sendBtn = (Button) findViewById(R.id.blesend_btn);
        rcvBtn = (Button) findViewById(R.id.blercv_btn);
        sendEdit = (EditText) findViewById(R.id.blesend_edit);
        rcvEdit = (EditText) findViewById(R.id.blercv_edit);
        devText = (TextView) findViewById(R.id.dev_text);
        led0btn = (Button) findViewById(R.id.led0_btn);
        led1btn = (Button) findViewById(R.id.led1_btn);
        devList = (ListView) findViewById(R.id.dev_list);
        adapter = new ArrayAdapter(bleActivity.this, android.R.layout.simple_list_item_1, deviceStr);

        devList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                BluetoothDevice device = null;
                device = devices.get(i);
                Toast.makeText(bleActivity.this, device.getName() + device.getAddress(), Toast.LENGTH_LONG).show();
                if (bleGatt == null) {
                    bleAdapter.getBluetoothLeScanner().stopScan(scanCallBack);
                    bleGatt = device.connectGatt(bleActivity.this, true, GattCallBack);
                    deviceStr.clear();
                    devList.setAdapter(adapter);
                    devText.setText(device.getName() + device.getAddress());
                }
            }
        });
        bleHandle = new handle();

    }

    public void btnEvent(View btn) {
        switch (btn.getId()) {
            case R.id.bleswitch:
                if (ofSwitch.isChecked()) {
                    Toast.makeText(this, "打开ble", Toast.LENGTH_SHORT).show();
                    Intent btEnable = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(btEnable, 1);
                } else {
                    Toast.makeText(this, "关闭ble", Toast.LENGTH_SHORT).show();
                    bleClose();
                }
                break;
            case R.id.scanswitch:
                if (scanSwitch.isChecked()) {
                    Toast.makeText(this, "搜索蓝牙", Toast.LENGTH_SHORT).show();
                    if (bleDevice == null && bleAdapter != null) {
                        bleAdapter.getBluetoothLeScanner().startScan(scanCallBack);
                    } else {
                        Toast.makeText(this, "发现设备", Toast.LENGTH_SHORT).show();
                    }
                } else {
                    devices.clear();
                    deviceStr.clear();
                    devList.setAdapter(adapter);
                    bleAdapter.getBluetoothLeScanner().stopScan(scanCallBack);
                    bleGatt.disconnect();
                    bleDevice = null;
                    Toast.makeText(this, "停止搜索", Toast.LENGTH_SHORT).show();
                }
                break;

            case R.id.blesend_btn:
                if (sendEdit.getText().toString().trim().length() != 0) {
                    String sendStr = sendEdit.getText().toString().trim();
                    try {
                        bleGatt.setCharacteristicNotification(led0, true);
                        led0.setValue(ClsUtils.toHexBytes(sendStr));
                        led0.setWriteType(led0.WRITE_TYPE_NO_RESPONSE);
                        bleGatt.writeCharacteristic(led0);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                } else {
                    Toast.makeText(this, "输入不能为空", Toast.LENGTH_SHORT).show();
                }

                break;
            case R.id.blercv_btn:
                if (bleGatt != null && btnbgc != null) {
                    bleGatt.readCharacteristic(btnbgc);
                }
                break;
            case R.id.led0_btn:
                led0on = led0on ^ true;
                byte val[] = new byte[1];
                if (led0on) {
                    val[0] = 1;
                } else {
                    val[0] = 0;
                }
                if (led0 != null) {
                    bleGatt.setCharacteristicNotification(led0, true);
                    led0.setValue(val);
                    led0.setWriteType(led0.WRITE_TYPE_NO_RESPONSE);
                    bleGatt.writeCharacteristic(led0);
                    iscalled = false;
                }
                break;
            case R.id.led1_btn:
                led1on = led1on ^ true;
                byte ledval[] = new byte[1];
                if (led1on) {
                    ledval[0] = 1;
                } else {
                    ledval[0] = 0;
                }
                if (led1 != null) {
                    bleGatt.setCharacteristicNotification(led1, true);
                    led1.setValue(ledval);
                    led1.setWriteType(led1.WRITE_TYPE_NO_RESPONSE);
                    bleGatt.writeCharacteristic(led1);
                    iscalled = false;
                }
                break;
        }
    }

    public void bleClose() {

        if (bleAdapter == null) {
            return;
        } else {
            if (bleGatt == null) {
                return;
            } else {
                bleGatt.close();
                bleGatt = null;
            }
            bleAdapter.disable();
            bleAdapter = null;
        }

    }

    private BluetoothGattCallback GattCallBack = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            super.onConnectionStateChange(gatt, status, newState);
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                if (bleGatt != null) {
                    System.out.println("connect succeed");
                    Message msg = bleHandle.obtainMessage();
                    msg.what = 1;
                    bleHandle.sendMessage(msg);
                    bleGatt.discoverServices();
                }
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            super.onServicesDiscovered(gatt, status);
            System.out.println("onServicesDiscovered");
            List<BluetoothGattService> gattServices = gatt.getServices();
            for (BluetoothGattService bgs : gattServices) {
                String uuid = bgs.getUuid().toString();
                System.out.println("servers uuid:" + uuid);
                List<BluetoothGattCharacteristic> gattCharacteristics = bgs.getCharacteristics();
                for (BluetoothGattCharacteristic bgc : gattCharacteristics) {
                    uuid = bgc.getUuid().toString();
                    if (uuid.equals("f0001111-0451-4000-b000-000000000000")) {
                        led0 = bgc;//LED
                    }
                    if (uuid.equals("f0001112-0451-4000-b000-000000000000")) {
                        led1 = bgc;//LED
                    }
                    if (uuid.equals("f0001121-0451-4000-b000-000000000000")) {
                        btnbgc = bgc;//按键
                    }
                    System.out.println("character uuid:" + uuid);

                    int charaProp = bgc.getProperties();
                    if ((charaProp & BluetoothGattCharacteristic.PROPERTY_READ) > 0) {
                        System.out.println("可读");
                    }
                    if ((charaProp & BluetoothGattCharacteristic.PROPERTY_WRITE) > 0) {
                        System.out.println("可写");

                    }
                    if ((charaProp & BluetoothGattCharacteristic.PROPERTY_NOTIFY) > 0) {
                        System.out.println("通知");
                    }

                }
            }
            new ReadCharacteristic(led0).start();
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            super.onCharacteristicRead(gatt, characteristic, status);
            byte[] buf = characteristic.getValue();
            String str = "";
            for (int i = 0; i < buf.length; i++) {
                str += String.format("0x%02x;", buf[i]);
            }
            System.out.println("onCharacteristicRead:" + str);

            Message msg = bleHandle.obtainMessage();
            msg.obj = buf;
            msg.what = 2;
            bleHandle.sendMessage(msg);
            iscalled = true;//读取完成
        }

        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            super.onCharacteristicWrite(gatt, characteristic, status);
            byte[] writebuf = characteristic.getValue();
            System.out.println("onCharacteristicWrite:" + ClsUtils.toHexString(writebuf));
            iscalled = true;
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            super.onCharacteristicChanged(gatt, characteristic);
            System.out.println("onCharacteristicChanged");
        }

        @Override
        public void onDescriptorRead(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            super.onDescriptorRead(gatt, descriptor, status);
            System.out.println("onDescriptorRead");

        }

        @Override
        public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            super.onDescriptorWrite(gatt, descriptor, status);
            System.out.println("onDescriptorWrite");

        }
    };
    private ScanCallback scanCallBack = new ScanCallback() {
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
            System.out.println(scanResult.toString());
            for (int i = 0; i < deviceStr.size(); i++) {
                if (scanResult.toString().equals(deviceStr.get(i))) {
                    return;
                }
            }
            devices.add(dev);
            deviceStr.add(scanResult.toString());
            devList.setAdapter(adapter);
            adapter.notifyDataSetChanged();//更新
            if (bleName.equals(name)) {
                bleDevice = result.getDevice();
            }
        }
    };

    private class handle extends Handler {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case 1:
                    bleAdapter.getBluetoothLeScanner().stopScan(scanCallBack);
                    Toast.makeText(bleActivity.this, "连接成功", Toast.LENGTH_LONG).show();
                    break;
                case 2:
                    byte buf[] = (byte[]) msg.obj;
                   /* String rcv = "";
                    for (int i = 0; i < buf.length; i++) {
                        rcv += String.format("0x%02x;", buf[i]);
                    }*/
                    rcvEdit.setText(ClsUtils.toHexString(buf));
                    break;
                case 3:
                    Toast.makeText(bleActivity.this, "读取异常", Toast.LENGTH_SHORT).show();
                    break;
            }
        }
    }

    public class ReadCharacteristic extends Thread {
        BluetoothGattCharacteristic characteristic;

        public ReadCharacteristic(BluetoothGattCharacteristic temp) {
            this.characteristic = temp;
        }

        @Override
        public void run() {
            while (true) {
                if (iscalled == true) {
                    try {
                        sleep(500);
                        if (bleGatt != null && characteristic != null) {
                            bleGatt.readCharacteristic(characteristic);
                            iscalled = false;
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                        Message msg = bleHandle.obtainMessage();
                        msg.what = 3;
                        bleHandle.sendMessage(msg);
                    }
                }
            }
        }
    }

}
