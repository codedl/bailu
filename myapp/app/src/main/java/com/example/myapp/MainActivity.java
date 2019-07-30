package com.example.myapp;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.location.LocationManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.Adapter;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.AutoCompleteTextView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ListView;

import com.example.myapp.R;

import java.sql.Array;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;

public class MainActivity extends AppCompatActivity {
    private Spinner freqway_intwaySpin;
    protected ArrayList<String> freqway_intwayStr = new ArrayList<>();
    private ArrayAdapter freqway_intwayAdapt;
    private Spinner comModeSpin;
    protected ArrayList<String> comModeStr = new ArrayList<>();
    private ArrayAdapter comModeAdapt;
    private Spinner freqrange_intbandSpin;
    protected ArrayList<String> freqrange_intbandStr = new ArrayList<>();
    private ArrayAdapter freqrange_intbandAdapt;
    private Spinner freqspeedSpin;
    protected ArrayList<String> freqspeedStr = new ArrayList<>();
    private ArrayAdapter freqspeedAdapt;
    private Spinner demodwaySpin;
    protected ArrayList<String> demodwayStr = new ArrayList<>();
    private ArrayAdapter demodwayAdapt;
    private Spinner msg_noiseSpin;
    protected ArrayList<String> msg_noiseStr = new ArrayList<>();
    private ArrayAdapter msg_noiseAdapt;

    private TextView freqrange_intbandText;
    private TextView msgcontent_Text;
    private TextView freqspeedText;
    private EditText freqEdit;
    private EditText msgEdit;
    private EditText powerEdit;
    private Button freqlistBtn;
    private Button connectBtn;
    private TextView freqway_intwayText;
    private TextView msg_noiseText;
    private Switch emitSwitch;
    private ProgressBar bar;
    msgHandle handle;
    private double freqListDArray[];
    private int freqListIArray[];

    BluetoothAdapter bleAdapt;
    BluetoothGatt bleGatt;
    BluetoothDevice bleDevice;
    BluetoothGattCharacteristic red;
    BluetoothGattCharacteristic green;
    BluetoothGattCharacteristic bleCharacteristic;
    byte[] rcvBuf;
    byte[] sendBuf;//发送数据，不包括等幅报文
    int sendIndex;
    boolean isRead;
    boolean isWrite = false;//判断是否完成发送
    boolean isfreqListSend;
    boolean ismsgSend;
    byte ledon;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        checkPermissions();
        init();
    }

    void init() {
        layoutInit();
        comModeInit();
        bleInit();
    }

    /**
     * 打开蓝牙，获取蓝牙适配器
     */
    void bleInit() {

        Intent intent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        startActivityForResult(intent, 1);

        BluetoothManager bm = (BluetoothManager) getSystemService(BLUETOOTH_SERVICE);
        bleAdapt = bm.getAdapter();

    }

    /**
     * 通信模式初始化
     */
    void comModeInit() {
        isfreqListSend = false;
        ismsgSend = false;

        freqway_intwayText.setText("频率方式:");
        freqrange_intbandText.setText("跳频范围:");
        freqspeedText.setText("跳频速率:");
        msg_noiseText.setText("报        文:");
        msg_noiseText.setVisibility(View.VISIBLE);

        freqrange_intbandText.setTextColor(Color.parseColor("#8A000000"));
        freqrange_intbandSpin.setEnabled(false);
        freqrange_intbandStr.clear();
        freqrange_intbandStr.add("200KHz");
        freqrange_intbandSpin.setAdapter(freqrange_intbandAdapt);
        freqrange_intbandSpin.setSelection(0, true);
        freqrange_intbandSpin.setOnItemSelectedListener(comFreqRangeSpinListener);

        freqspeedText.setTextColor(Color.parseColor("#8A000000"));
        freqspeedSpin.setEnabled(false);
        freqspeedStr.clear();
        freqspeedStr.add("20跳/秒");
        freqspeedSpin.setAdapter(freqspeedAdapt);

        freqway_intwaySpin.setOnItemSelectedListener(null);
        freqway_intwayStr.clear();
        freqway_intwayStr.add("定频");
        freqway_intwayStr.add("跳频");
        freqway_intwaySpin.setAdapter(freqway_intwayAdapt);
        freqway_intwaySpin.setSelection(0, true);
        freqway_intwaySpin.setOnItemSelectedListener(comFreqwaySpinListener);

        msg_noiseStr.clear();
        msg_noiseStr.add("黄山黄山我是泰山");
        msg_noiseStr.add("长江长江我是黄河");
        msg_noiseStr.add("老虎老虎我是老鹰");
        msg_noiseStr.add("01 01  我  是 02");
        msg_noiseStr.add("黄河黄河我是长江");
        msg_noiseSpin.setVisibility(View.VISIBLE);
        msg_noiseSpin.setAdapter(msg_noiseAdapt);
        msgEdit.setVisibility(View.INVISIBLE);

        TextView demodway = findViewById(R.id.demodway_text);
        demodway.setTextColor(Color.parseColor("#4CAF50"));
        TextView demodsou = findViewById(R.id.demodsource_text);
        demodsou.setTextColor(Color.parseColor("#4CAF50"));
        demodwaySpin.setOnItemSelectedListener(comDemodWaySpinListener);
        demodwaySpin.setEnabled(true);
    }

    /**
     * 界面初始化
     */
    void layoutInit() {
        handle = new msgHandle();
        bar = findViewById(R.id.bar);
        msgcontent_Text = findViewById(R.id.msgcontent_text);
        connectBtn = findViewById(R.id.connect_btn);
        msg_noiseText = findViewById(R.id.noise_msg_text);
        freqway_intwayText = findViewById(R.id.intway_freqway_text);
        freqrange_intbandText = findViewById(R.id.intband_freqrange_text);
        freqspeedText = findViewById(R.id.freqspeed_text);
        freqEdit = findViewById(R.id.freq_edit);
        powerEdit = findViewById(R.id.power_edit);
        freqlistBtn = findViewById(R.id.speed_btn);
        msg_noiseSpin = findViewById(R.id.noise_msgcontent_spin);
        msg_noiseAdapt = new ArrayAdapter(this, android.R.layout.simple_list_item_1, msg_noiseStr);
        demodwaySpin = findViewById(R.id.demodway_spin);
        demodwayAdapt = new ArrayAdapter(this, android.R.layout.simple_list_item_1, demodwayStr);
        freqway_intwaySpin = findViewById(R.id.intway_freqway_spin);
        freqway_intwayAdapt = new ArrayAdapter(this, android.R.layout.simple_list_item_1, freqway_intwayStr);
        comModeSpin = findViewById(R.id.mode_spin);
        comModeAdapt = new ArrayAdapter(this, android.R.layout.simple_list_item_1, comModeStr);
        freqspeedSpin = findViewById(R.id.freqspeed_spin);
        freqspeedAdapt = new ArrayAdapter(this, android.R.layout.simple_list_item_1, freqspeedStr);
        freqrange_intbandSpin = findViewById(R.id.intband_freqrange_spin);
        freqrange_intbandAdapt = new ArrayAdapter(this, android.R.layout.simple_list_item_1, freqrange_intbandStr);
        msgEdit = findViewById(R.id.noise_msgcontent_edit);
        emitSwitch = findViewById(R.id.emitstat_switch);
        comModeStr.add("通信模式");
        comModeStr.add("干扰模式");
        comModeSpin.setAdapter(comModeAdapt);
        comModeSpin.setSelection(0, true);
        comModeSpin.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                switch (i) {
                    case 0:
                        comModeInit();
                        break;
                    case 1:
                        intModeInit();
                        break;
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        });

        demodwayStr.add("AM");
        demodwayStr.add("FM");
        demodwayStr.add("USB");
        demodwayStr.add("LSB");
        demodwayStr.add("DSB");
        demodwayStr.add("CW");
        demodwayStr.add("FSK");
        demodwayStr.add("多音FSK");
        demodwayStr.add("LINK数据链");
        demodwaySpin.setAdapter(demodwayAdapt);
        demodwaySpin.setSelection(0, true);
        demodwaySpin.setOnItemSelectedListener(comDemodWaySpinListener);

    }

    //干扰模式初始化
    void intModeInit() {
        freqway_intwayText.setText("干扰样式");
        freqway_intwaySpin.setOnItemSelectedListener(null);
        freqway_intwayStr.clear();
        freqway_intwayStr.add("瞄准式");
        freqway_intwayStr.add("扫频式");
        freqway_intwaySpin.setAdapter(freqway_intwayAdapt);
        freqway_intwaySpin.setSelection(0, true);
        freqway_intwaySpin.setOnItemSelectedListener(intWaySpinListener);

        freqrange_intbandText.setText("干扰带宽:");
        freqrange_intbandText.setTextColor(Color.parseColor("#4CAF50"));
        freqrange_intbandStr.clear();
        freqrange_intbandStr.add("5KHz");
        freqrange_intbandStr.add("10KHz");
        freqrange_intbandStr.add("25KHz");
        freqrange_intbandSpin.setAdapter(freqrange_intbandAdapt);
        freqrange_intbandSpin.setEnabled(true);
        freqrange_intbandSpin.setOnItemSelectedListener(null);

        freqspeedText.setText("扫频速率:");
        freqspeedText.setTextColor(Color.parseColor("#8A000000"));
        freqspeedStr.clear();
        freqspeedStr.add("1KHz");
        freqspeedSpin.setAdapter(freqspeedAdapt);
        freqspeedSpin.setEnabled(false);

        msg_noiseText.setVisibility(View.INVISIBLE);
        msg_noiseSpin.setVisibility(View.INVISIBLE);
        msgEdit.setVisibility(View.INVISIBLE);

        TextView demodway = findViewById(R.id.demodway_text);
        demodway.setTextColor(Color.parseColor("#8A000000"));
        TextView demodsou = findViewById(R.id.demodsource_text);
        demodsou.setTextColor(Color.parseColor("#8A000000"));


        demodwaySpin.setOnItemSelectedListener(null);

//        调制方式
        demodwayStr.clear();
        demodwayStr.add("AM");
        demodwayStr.add("FM");
        demodwayStr.add("USB");
        demodwayStr.add("LSB");
        demodwayStr.add("DSB");
        demodwayStr.add("CW");
        demodwayStr.add("FSK");
        demodwayStr.add("多音FSK");
        demodwayStr.add("LINK数据链");
        demodwaySpin.setAdapter(demodwayAdapt);
        demodwaySpin.setSelection(1);
        demodwaySpin.setEnabled(false);
        msgcontent_Text.setText("   噪声");
    }

    //按钮监听事件
    public void clickEvent(View view) {
        switch (view.getId()) {
            case R.id.speed_btn:
                View freqLayout = getLayoutInflater().inflate(R.layout.freqlist, null);
                ListView freqList = freqLayout.findViewById(R.id.freq_list);
                ArrayList<String> freqStrList = new ArrayList<>();
                ArrayAdapter freqListAdapt = new ArrayAdapter(MainActivity.this, android.R.layout.simple_list_item_1, freqStrList);
                String str = "";
                for (int i = 0; i < freqListDArray.length; i++) {
                    str = "         ";
                    str += (String.format("%03d", (i + 1)));
                    str += "              ";
                    str += (String.format("%4.6f", freqListDArray[i]));
                    str += "MHz";
                    freqStrList.add(str);
                }
                freqList.setAdapter(freqListAdapt);
                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                builder.setView(freqLayout)
                        .setTitle("图案序号         频率列表")
                        .setNegativeButton("退出", null)
                        .create().show();
                break;
            case R.id.send_btn:
                if (ismsgSend) {
                    if (msgEdit.getText().toString().trim().length() != 0) {
                        param.msgStr = msgEdit.getText().toString().trim().toUpperCase();
                        param.msgBits = morse.stringToBits(param.msgStr);
                        param.msgBytes = morse.bitsToBytes(param.msgBits);
                    } else {
                        debug("输入不能为空", false);
                    }
                }
                if (bleGatt != null && bleCharacteristic != null)
                    bleSend();


                break;

            case R.id.connect_btn:
                connectBtn.setText("连接设备");
                bar.setVisibility(View.VISIBLE);
                final AutoCompleteTextView edit = new AutoCompleteTextView(this);
                edit.setThreshold(2);
                final String[] hintStr = {"ProjectZero", "BLE SPS"};
                ArrayAdapter<String> adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, hintStr);
                edit.setAdapter(adapter);
                edit.setHint("输入蓝牙设备名");
                AlertDialog.Builder editBuilder = new AlertDialog.Builder(this);
                editBuilder.setTitle("输入设备名")
                        .setView(edit)
                        .setPositiveButton("确定", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialogInterface, int i) {
                                if (edit.getText().toString().length() != 0) {
                                    param.bleName = edit.getText().toString().trim();
                                    TextView model = findViewById(R.id.model_text);
                                    model.setText(param.bleName);
                                } else {
                                    debug("输入不能为空", false);
                                }
                            }
                        })
                        .setNegativeButton("取消", null)
                        .create().show();
                if (bleAdapt != null) {
                    bleAdapt.getBluetoothLeScanner().startScan(scanCallBack);
                }

                break;
        }
    }

    /**
     * 蓝牙扫描回调函数，获取蓝牙设备名和设备地址
     */
    public ScanCallback scanCallBack = new ScanCallback() {
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
            debug(scanResult.toString(), true);
            if (param.bleName.equals(name)) {
                bleDevice = result.getDevice();
                bleAdapt.getBluetoothLeScanner().stopScan(scanCallBack);
                bleGatt = bleDevice.connectGatt(MainActivity.this, true, gattCallBack);
                debug("discover device", true);
            }
        }
    };

    /**
     * 蓝牙服务回调函数，执行读写和连接状态的回读
     */
    public BluetoothGattCallback gattCallBack = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            super.onConnectionStateChange(gatt, status, newState);
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                if (bleGatt != null) {
                    bleGatt.discoverServices();
                }
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            super.onServicesDiscovered(gatt, status);
            List<BluetoothGattService> gattServices = gatt.getServices();
            for (BluetoothGattService bgs : gattServices) {
                String uuid = bgs.getUuid().toString();
//                debug("services:" + uuid, true);
                List<BluetoothGattCharacteristic> gattCharacteristics = bgs.getCharacteristics();
                for (BluetoothGattCharacteristic bgc : gattCharacteristics) {
                    uuid = bgc.getUuid().toString();
//                    debug("characteristics:" + uuid, true);
                    if (uuid.equals("f0001111-0451-4000-b000-000000000000")) {
                        red = bgc;//LED红
                    }
                    if (uuid.equals("f0001112-0451-4000-b000-000000000000")) {
                        bleCharacteristic = bgc;//LED绿
                    }
                    if (uuid.equals("0000fee1-0000-1000-8000-00805f9b34fb")) {
                        bleCharacteristic = bgc;
                    }
                  /*  if (uuid.equals("f0001121-0451-4000-b000-000000000000")) {
                        btnbgc = bgc;//按键
                    }*/

                    /*int charaProp = bgc.getProperties();
                    if ((charaProp & BluetoothGattCharacteristic.PROPERTY_READ) > 0) {
                        debug("可读", true);
                    }
                    if ((charaProp & BluetoothGattCharacteristic.PROPERTY_WRITE) > 0) {
                        debug("可写", true);

                    }
                    if ((charaProp & BluetoothGattCharacteristic.PROPERTY_NOTIFY) > 0) {
                        debug("通知", true);
                    }*/

                }
            }
            debug("onServicesDiscovered", true);
            Message msg = handle.obtainMessage();
            msg.what = 1;
            handle.sendMessage(msg);
            debug("state:connectend", true);
//            new ReadCharacteristic(green).start();
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            super.onCharacteristicRead(gatt, characteristic, status);
            rcvBuf = characteristic.getValue();
            String str = "";
            for (int i = 0; i < rcvBuf.length; i++) {
                str += String.format("0x%02x;", rcvBuf[i]);
            }
            debug("Read:" + str, true);
            isRead = true;
        }

        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            super.onCharacteristicWrite(gatt, characteristic, status);
            byte[] writebuf = characteristic.getValue();
            debug("Write:" + ClsUtils.toHexString(writebuf), true);
            isWrite = true;
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            super.onCharacteristicChanged(gatt, characteristic);
            debug("onCharacteristicChanged", true);
        }

        @Override
        public void onDescriptorRead(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            super.onDescriptorRead(gatt, descriptor, status);
            debug("onDescriptorRead", true);

        }

        @Override
        public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            super.onDescriptorWrite(gatt, descriptor, status);
            debug("onDescriptorWrite", true);
        }
    };

    public void bleWrite(byte[] buf) {
//        bleGatt.setCharacteristicNotification(bleCharacteristic, true);
        bleCharacteristic.setValue(buf);
//        bleCharacteristic.setWriteType(bleCharacteristic.WRITE_TYPE_NO_RESPONSE);
        bleGatt.writeCharacteristic(bleCharacteristic);
    }


    public class ReadCharacteristic extends Thread {
        BluetoothGattCharacteristic characteristic;
        Message msg = null;

        public ReadCharacteristic(BluetoothGattCharacteristic temp) {
            this.characteristic = temp;
        }

        @Override
        public void run() {

            while (bleGatt != null && characteristic != null) {
                try {
                    bleGatt.readCharacteristic(characteristic);
                    isRead = false;
                    sleep(3000);

                } catch (Exception e) {

                }
            }

        }
    }


    //判断是否点击了editview
    public boolean isEditView(View view, MotionEvent motionEvent) {
        if (view != null && view instanceof EditText) {
            int leftTop[] = {0, 0};
            view.getLocationInWindow(leftTop);
            int left = leftTop[0];
            int top = leftTop[1];
            int right = leftTop[0] + view.getWidth();
            int bottom = leftTop[1] + view.getHeight();
            if (motionEvent.getX() > left && motionEvent.getX() < right && motionEvent.getY() > top && motionEvent.getY() < bottom) {
                return true;
            } else {
                return false;
            }
        }
        return false;
    }

    //为editview设置边框
    @Override
    public boolean dispatchTouchEvent(MotionEvent motionEvent) {
        if (motionEvent.getAction() == MotionEvent.ACTION_DOWN) {
            View view = getCurrentFocus();
            EditText edit = (EditText) view;
            if (edit.equals(msgEdit)) {
                if (getWindow().superDispatchTouchEvent(motionEvent)) {
                    return true;
                }
                return onTouchEvent(motionEvent);
            }
            if (isEditView(view, motionEvent)) {
                edit.requestFocus();
                edit.setCursorVisible(true);
                edit.setBackgroundResource(R.drawable.edit);
            } else {
                edit.setCursorVisible(false);
                edit.setBackground(null);
                edit.clearFocus();
                InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                if (imm != null) {
                    imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
                }
                if (edit.equals(freqEdit)) {
                    if (freqEdit.getText().toString().length() != 0) {
                        param.freq = Double.parseDouble(freqEdit.getText().toString());
                    } else {
                        debug("输入不能为空", false);
                    }
                    if (param.freq < 3) {
                        param.freq = 3;
                        debug("频率最小:3MHz", false);
                    } else if (param.freq > 1000) {
                        param.freq = 1000;
                        debug("频率最大:1000MHz", false);
                    }
                    freqEdit.setText("   " + String.format("%4.6f", param.freq));
                }

                if (edit.equals(powerEdit)) {
                    if (powerEdit.getText().toString().length() != 0) {
                        param.power = Double.parseDouble(powerEdit.getText().toString());
                    } else {
                        debug("输入不能为空", false);
                    }
                    if (param.power < 0.001) {
                        param.power = 0.001;
                        debug("功率最小:1mW", false);
                    } else if (param.power > 10) {
                        param.power = 10;
                        debug("功率最大:10W", false);
                    }
                    powerEdit.setText("     " + String.format("%1.3f", param.power));
                }
            }
            return super.dispatchTouchEvent(motionEvent);
        }
        if (getWindow().superDispatchTouchEvent(motionEvent)) {
            return true;
        }
        return onTouchEvent(motionEvent);
    }

    public void debug(String str, boolean print) {
        if (print) {
            System.out.println(str);
        } else {
            Toast.makeText(MainActivity.this, str, Toast.LENGTH_SHORT).show();
        }
    }

    Spinner.OnItemSelectedListener intWaySpinListener = new Spinner.OnItemSelectedListener() {

        @Override
        public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
            switch (i) {
                case 0:
                    freqrange_intbandStr.clear();
                    freqrange_intbandStr.add("5KHz");
                    freqrange_intbandStr.add("10KHz");
                    freqrange_intbandStr.add("25KHz");
                    freqrange_intbandSpin.setAdapter(freqrange_intbandAdapt);

                    freqspeedText.setTextColor(Color.parseColor("#8A000000"));
                    freqspeedSpin.setEnabled(false);
                    break;
                case 1:
                    freqrange_intbandStr.clear();
                    freqrange_intbandStr.add("200KHz");
                    freqrange_intbandStr.add("500KHz");
                    freqrange_intbandStr.add("1MHz");
                    freqrange_intbandStr.add("2MHz");
                    freqrange_intbandStr.add("5MHz");
                    freqrange_intbandStr.add("10MHz");
                    freqrange_intbandStr.add("20MHz");
                    freqrange_intbandSpin.setAdapter(freqrange_intbandAdapt);

                    freqspeedText.setTextColor(Color.parseColor("#4CAF50"));
                    freqspeedStr.clear();
                    freqspeedStr.add("10Hz");
                    freqspeedStr.add("100Hz");
                    freqspeedStr.add("1KHz");
                    freqspeedStr.add("5KHz");
                    freqspeedStr.add("10KHz");
                    freqspeedStr.add("50KHz");
                    freqspeedStr.add("100KHz");
                    freqspeedStr.add("1MHz");
                    freqspeedSpin.setAdapter(freqspeedAdapt);
                    freqspeedSpin.setEnabled(true);

                    break;
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> adapterView) {

        }
    };
    Spinner.OnItemSelectedListener comFreqRangeSpinListener = new Spinner.OnItemSelectedListener() {

        @Override
        public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
            String str = null;
            int length = 0;
            int index = 0;
            int randnum = 0;
            double freq = param.freq;

            if (freqrange_intbandStr.get(i).contains("KHz")) {
                str = freqrange_intbandStr.get(i).replace("KHz", "");
                randnum = Integer.parseInt(str) / 4 * 2; // 上下限
                freq -= Double.parseDouble(str) / 1000;
            } else {
                str = freqrange_intbandStr.get(i).replace("MHz", "");
                randnum = Integer.parseInt(str) * 1000 / 4 * 2;
                freq -= Double.parseDouble(str);
            }
            if (freq < 0) {
                freq = 0;
            }
            length = randnum;
            if (randnum > 100) {
                length = 100;
            }
            freqListIArray = new int[length];
            int rand = 0;
            Random random = new Random();
            while (index < length - 1) {
                rand = random.nextInt(randnum);
                while (isInArray(rand, freqListIArray)) {
                    rand = random.nextInt(randnum);
                }
                freqListIArray[index++] = rand;
            }

            rand = random.nextInt(length);
            freqListIArray[length - 1] = freqListIArray[rand];
            freqListIArray[rand] = 0;

            freqListDArray = new double[length];
            for (int randomIndex = 0; randomIndex < length; randomIndex++) {
                freqListDArray[randomIndex] = freq + (double) freqListIArray[randomIndex] * 4 / 1000;
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> adapterView) {

        }
    };

    boolean isInArray(int a, int b[]) {
        for (int i = 0; i < b.length; i++) {
            if (b[i] == a) {
                return true;
            }
        }
        return false;
    }

    Spinner.OnItemSelectedListener comFreqwaySpinListener = new Spinner.OnItemSelectedListener() {
        @Override
        public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {

            switch (i) {
                case 0:
                    isfreqListSend = false;
                    freqrange_intbandStr.clear();
                    freqrange_intbandStr.add("200KHz");
                    freqrange_intbandSpin.setAdapter(freqrange_intbandAdapt);
                    freqrange_intbandSpin.setEnabled(false);
                    freqrange_intbandText.setTextColor(Color.parseColor("#8A000000"));

                    freqspeedStr.clear();
                    freqspeedStr.add("20跳/秒");
                    freqspeedSpin.setAdapter(freqspeedAdapt);
                    freqspeedSpin.setEnabled(false);
                    freqspeedText.setTextColor(Color.parseColor("#8A000000"));

                    demodwayStr.clear();
                    demodwayStr.add("AM");
                    demodwayStr.add("FM");
                    demodwayStr.add("USB");
                    demodwayStr.add("LSB");
                    demodwayStr.add("DSB");
                    demodwayStr.add("CW");
                    demodwayStr.add("FSK");
                    demodwayStr.add("多音FSK");
                    demodwaySpin.setAdapter(demodwayAdapt);

                    freqlistBtn.setVisibility(View.INVISIBLE);

                    break;
                case 1:
                    isfreqListSend = true;//跳频需要下发跳频频率
                    freqlistBtn.setVisibility(View.VISIBLE);
                    freqrange_intbandSpin.setEnabled(true);
                    freqrange_intbandText.setTextColor(Color.parseColor("#4CAF50"));
                    freqspeedText.setTextColor(Color.parseColor("#4CAF50"));
                    freqspeedSpin.setEnabled(true);
                    freqspeedStr.clear();
                    freqrange_intbandStr.clear();

                    freqrange_intbandStr.add("200KHz");
                    freqrange_intbandStr.add("500KHz");
                    freqrange_intbandStr.add("1MHz");
                    freqrange_intbandStr.add("2MHz");
                    freqrange_intbandStr.add("5MHz");
                    freqrange_intbandStr.add("10MHz");
                    freqrange_intbandStr.add("20MHz");
                    freqrange_intbandStr.add("27MHz");
                    freqrange_intbandStr.add("80MHz");
                    if (param.freq >= 3 && param.freq <= 30) {
                        freqrange_intbandStr.remove("80MHz");
                    } else if (param.freq >= 110 && param.freq <= 1000) {
                        freqrange_intbandStr.remove("27MHz");
                        freqrange_intbandStr.remove("80MHz");
                    }
                    freqrange_intbandSpin.setAdapter(freqrange_intbandAdapt);

                    freqspeedStr.add("20跳/秒");
                    freqspeedStr.add("50跳/秒");
                    freqspeedStr.add("100跳/秒");
                    freqspeedStr.add("500跳/秒");
                    freqspeedStr.add("1000跳/秒");
                    freqspeedSpin.setAdapter(freqspeedAdapt);

                    demodwayStr.clear();
                    demodwayStr.add("AM");
                    demodwayStr.add("FM");
                    demodwayStr.add("USB");
                    demodwayStr.add("LSB");
                    demodwayStr.add("DSB");
                    demodwaySpin.setAdapter(demodwayAdapt);
                    break;
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> adapterView) {

        }
    };

    Spinner.OnItemSelectedListener comDemodWaySpinListener = new Spinner.OnItemSelectedListener() {
        @Override
        public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
            switch (i) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                    msg_noiseSpin.setVisibility(View.VISIBLE);
                    msgEdit.setVisibility(View.INVISIBLE);
                    msg_noiseText.setVisibility(View.VISIBLE);
                    ismsgSend = false;
                    msgcontent_Text.setText("   语音报文");
                    break;
                case 5:
                    msg_noiseSpin.setVisibility(View.INVISIBLE);
                    msgEdit.setVisibility(View.VISIBLE);
                    msg_noiseText.setVisibility(View.VISIBLE);
                    ismsgSend = true; // 需要下发报文序列
                    msgcontent_Text.setText("   等幅报文");
                    break;
                case 6:
                case 7:
                    msg_noiseSpin.setVisibility(View.INVISIBLE);
                    msgEdit.setVisibility(View.INVISIBLE);
                    msg_noiseText.setVisibility(View.INVISIBLE);
                    ismsgSend = false;
                    msgcontent_Text.setText("   伪随机序列");
                    break;
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> adapterView) {

        }

    };

    /**
     * 检查权限
     */
    private void checkPermissions() {
        String[] permissions = {Manifest.permission.ACCESS_FINE_LOCATION};
        List<String> permissionDeniedList = new ArrayList<>();
        for (String permission : permissions) {
            int permissionCheck = ContextCompat.checkSelfPermission(this, permission);
            if (permissionCheck == PackageManager.PERMISSION_GRANTED) {
                onPermissionGranted(permission);
            } else {
                permissionDeniedList.add(permission);
            }
        }
        if (!permissionDeniedList.isEmpty()) {
            String[] deniedPermissions = permissionDeniedList.toArray(new String[permissionDeniedList.size()]);
            ActivityCompat.requestPermissions(this, deniedPermissions, 1);
        }
    }

    /**
     * 权限回调
     *
     * @param requestCode
     * @param permissions
     * @param grantResults
     */
    @Override
    public final void onRequestPermissionsResult(int requestCode,
                                                 @NonNull String[] permissions,
                                                 @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        switch (requestCode) {
            case 1:
                if (grantResults.length > 0) {
                    for (int i = 0; i < grantResults.length; i++) {
                        if (grantResults[i] == PackageManager.PERMISSION_GRANTED) {
                            onPermissionGranted(permissions[i]);
                        }
                    }
                }
                break;
        }
    }

    /**
     * 开启GPS
     *
     * @param permission
     */
    private void onPermissionGranted(String permission) {
        switch (permission) {
            case Manifest.permission.ACCESS_FINE_LOCATION:
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && !checkGPSIsOpen()) {
                    new AlertDialog.Builder(this)
                            .setTitle("提示")
                            .setMessage("当前手机扫描蓝牙需要打开定位功能。")
                            .setNegativeButton("取消",
                                    new DialogInterface.OnClickListener() {
                                        @Override
                                        public void onClick(DialogInterface dialog, int which) {
                                            finish();
                                        }
                                    })
                            .setPositiveButton("前往设置",
                                    new DialogInterface.OnClickListener() {
                                        @Override
                                        public void onClick(DialogInterface dialog, int which) {
                                            Intent intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
                                            startActivityForResult(intent, 1);
                                        }
                                    })

                            .setCancelable(false)
                            .show();
                } else {
                    //GPS已经开启了
                }
                break;
        }
    }

    /**
     * 检查GPS是否打开
     *
     * @return
     */
    private boolean checkGPSIsOpen() {
        LocationManager locationManager = (LocationManager) this.getSystemService(Context.LOCATION_SERVICE);
        if (locationManager == null)
            return false;
        return locationManager.isProviderEnabled(android.location.LocationManager.GPS_PROVIDER);
    }

    public void bleSend() {

        param.packageIndex = 1;//包的序号
        sendIndex = 0;
        int length = 3;
        if (isfreqListSend)
            length += 25;
        if (ismsgSend)
            length += Math.ceil(param.msgBytes.length / 16.0);
        sendBuf = new byte[length * 20];//为sendBuf分配内存
        Arrays.fill(sendBuf, (byte) 0);//初始化数组

        powerData();
        freqData();
        paramData();
        if (isfreqListSend) {
            freqListData();
        }
        if (ismsgSend) {
            msgData();
        }
        new bleSendThread().start();
    }

    public class bleSendThread extends Thread {
        @Override
        public void run() {
            byte writeBuf[] = new byte[20];//每个包20个字节
            int writeIndex = 0;
            for (int i = 0; i < sendBuf.length; i += 20) {
                for (int j = i; j < i + 20; j++)
                    writeBuf[writeIndex++] = sendBuf[j];
                try {
                    bleWrite(writeBuf);
                    writeIndex = 0;
                    sleep(20);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    void powerData() {
        byte powerbuf[] = new byte[12];
        sendBuf[sendIndex++] = param.packageIndex++;
        sendBuf[sendIndex++] = 12;
        sendBuf[sendIndex++] = 3;//参数设置一共三个包
        if (isfreqListSend) {
            sendBuf[sendIndex - 1] += 25;//频率列表25个包
        }
        if (ismsgSend) {
            sendBuf[sendIndex - 1] += (byte) Math.ceil(param.msgBytes.length / 16.0);//报文下发的包数
        }
        powerbuf = ClsUtils.doubleToBytesUnit(param.power * 1000);
        for (byte tmp : powerbuf) {
            sendBuf[sendIndex++] = tmp;
        }
        sendBuf[19] = ClsUtils.arraySum(sendBuf, 0, 20);//校验和
        sendIndex = 20;

    }

    void freqData() {
        byte freqbuf[] = new byte[12];
        sendBuf[sendIndex++] = param.packageIndex++;
        sendBuf[sendIndex++] = 16;
        sendBuf[sendIndex++] = 3;
        if (isfreqListSend) {
            sendBuf[sendIndex - 1] += 25;
        }
        if (ismsgSend) {
            sendBuf[sendIndex - 1] += (byte) Math.ceil(param.msgBytes.length / 16.0);
        }
        freqbuf = ClsUtils.doubleToBytesUnit(param.freq * 1000000);
        for (byte tmp : freqbuf) {
            sendBuf[sendIndex++] = tmp;
        }
        sendBuf[39] = ClsUtils.arraySum(sendBuf, 20, 40);//校验和
        sendIndex = 40;
//        bleWrite(sendBuf);
    }

    void paramData() {
        sendIndex = 40;//第三个包
        int bitsLength = param.msgBits.length();
        sendBuf[sendIndex++] = param.packageIndex++;
        sendBuf[sendIndex++] = 13;
        sendBuf[sendIndex++] = 3;
        if (isfreqListSend) {
            sendBuf[sendIndex - 1] += 25;
        }
        if (ismsgSend) {
            sendBuf[sendIndex - 1] += (byte) Math.ceil(param.msgBytes.length / 16.0);
        }
        sendBuf[sendIndex++] = (byte) (comModeSpin.getSelectedItemPosition() + 0xf0);
        sendBuf[sendIndex++] = (byte) (freqway_intwaySpin.getSelectedItemPosition() + 0xf1);
        sendBuf[sendIndex++] = (byte) (demodwaySpin.getSelectedItemPosition() + 0xf1);
        if (emitSwitch.isChecked())
            sendBuf[sendIndex++] = (byte) 0xf1;//原先调制源现在不需要下发
        else
            sendBuf[sendIndex++] = (byte) 0xf2;
        sendBuf[sendIndex++] = (byte) (msg_noiseSpin.getSelectedItemPosition() + 0xf1);
        sendBuf[sendIndex++] = (byte) (freqrange_intbandSpin.getSelectedItemPosition() + 0xf1);
        sendBuf[sendIndex++] = (byte) (freqspeedSpin.getSelectedItemPosition() + 0xf1);
        sendBuf[sendIndex++] = (byte) (bitsLength & 0xff);
        sendBuf[sendIndex++] = (byte) ((bitsLength >> 8) & 0xff);

        sendBuf[59] = ClsUtils.arraySum(sendBuf, 40, 60);//校验和
        sendIndex = 60;
//        bleWrite(sendBuf);
    }

    /**
     * 准备下发频率列表的数据
     */
    void freqListData() {
        for (int i = 0; i < freqListIArray.length; i += 4) {
            sendBuf[sendIndex++] = param.packageIndex++;//每个包的包头
            sendBuf[sendIndex++] = 20;//每个包的包长
            sendBuf[sendIndex++] = 25 + 3;//每个包的字节数
            if (ismsgSend) {
                sendBuf[sendIndex - 1] = (byte) Math.ceil(param.msgBytes.length / 16.0);
            }
            for (int j = i; j < i + 4; j++) {
//                把每个字写入包中
                sendBuf[sendIndex++] = (byte) (freqListIArray[j] & 0xff);
                sendBuf[sendIndex++] = (byte) ((freqListIArray[j] >> 8) & 0xff);
                sendBuf[sendIndex++] = (byte) ((freqListIArray[j] >> 16) & 0xff);
                sendBuf[sendIndex++] = (byte) ((freqListIArray[j] >> 24) & 0xff);
            }
            sendBuf[sendIndex++] = ClsUtils.arraySum(sendBuf, sendIndex - 20, sendIndex);
        }
    }

    public class freqListSend extends Thread {
        @Override
        public void run() {
            byte sendBuf[] = new byte[20];
            int sendIndex = 0;
            byte[] buf = new byte[freqListIArray.length * 8];

            sendBuf[sendIndex++] = 0x0c;//包头
            sendBuf[sendIndex++] = 20;//包的数据长度
            sendBuf[sendIndex++] = 25 + 3;//
            if (ismsgSend) {
                sendBuf[2] += (byte) Math.ceil(param.msgBytes.length / 16.0);
            }

            for (int tmp : freqListIArray) {
                sendBuf[sendIndex++] = (byte) (tmp & 0xff);
                sendBuf[sendIndex++] = (byte) ((tmp >> 8) & 0xff);
                sendBuf[sendIndex++] = (byte) ((tmp >> 16) & 0xff);
                sendBuf[sendIndex++] = (byte) ((tmp >> 24) & 0xff);
                if (sendIndex == 19) {
                    try {
//                        sendBuf[sendIndex] = ClsUtils.arraySum(sendBuf);//校验和
                        bleWrite(sendBuf);
                        sleep(15);
                        sendIndex = 3;
                        Arrays.fill(sendBuf, 3, 19, (byte) 0);

                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }

    /**
     * 动态生成报文下发的数据
     *
     * @return 需要下发的报文数据
     */
    void msgData() {
        if (param.msgStr == null) {
            return;
        }
        byte[] msgbuf = param.msgBytes;
        byte msgNum = (byte) Math.ceil(msgbuf.length / 16.0);//需要下发的报文包数
        byte[] retBuf = new byte[msgNum * 20];
        byte packageIndex = 29;//报文下发从第二十九包开始
        for (int i = 0; i < msgbuf.length; i += 16) {//每次整理一个包
            sendBuf[sendIndex++] = packageIndex++;
            if (msgbuf.length < 16)
                sendBuf[sendIndex++] = (byte) (msgbuf.length + 4);//报文包的长度小于16个字节
            else if (i + 16 < msgbuf.length)
                sendBuf[sendIndex++] = 20;//中间的包长
            else
                sendBuf[sendIndex++] = (byte) (msgbuf.length % 16 + 4);//最后一个包的包长;
            sendBuf[sendIndex++] = (byte) Math.ceil(msgbuf.length / 16.0 + 3);//一次下发的所有包数
            if (isfreqListSend) {
                sendBuf[sendIndex - 1] += (byte) 25;
            }
            if (msgbuf.length <= 16) { // 报文包的数据少于16个
                for (byte temp : msgbuf)
                    sendBuf[sendIndex++] = temp;
                sendIndex = sendIndex + 19 - sendIndex % 20;
                sendBuf[sendIndex] = ClsUtils.arraySum(sendBuf, sendIndex - 20, sendIndex);//校验字节
            } else {
                if (i + 16 < msgbuf.length) {//不是最后一个包
                    for (int j = i; j < i + 16; j++) {
                        sendBuf[sendIndex++] = msgbuf[j];
                    }
                    sendBuf[sendIndex++] = ClsUtils.arraySum(sendBuf, sendIndex - 20, sendIndex);//校验字节
                } else {//最后一个包
                    for (int j = i; j < msgbuf.length; j++)
                        sendBuf[sendIndex++] = msgbuf[j];
                    sendIndex = sendIndex + 19 - sendIndex % 20;//保存校验的索引
                    sendBuf[sendIndex] = ClsUtils.arraySum(sendBuf, sendIndex - 20, sendIndex);//校验字节
                }
            }
        }
    }

    public class msgSend extends Thread {
        @Override
        public void run() {
            if (param.msgStr == null) {
                return;
            }
            int index = 0;
            byte[] sendBuf = new byte[20];
            byte msgBytes[] = param.msgBytes;
            sendBuf[index++] = 0x0b;//包头
            sendBuf[index++] = 0;//包长待定
            sendBuf[index++] = (byte) Math.ceil(msgBytes.length / 16.0 + 3);//包数
            if (isfreqListSend) {
                sendBuf[2] += (byte) 25;
            }
            if (msgBytes.length <= 16) {
                sendBuf[1] = (byte) (msgBytes.length + 4);
                for (byte tmp : msgBytes) {
                    sendBuf[index++] = tmp;
                }
//                sendBuf[19] = ClsUtils.arraySum(sendBuf);//校验和
                bleWrite(sendBuf);
            } else {
                for (int i = 0; i < msgBytes.length; i++) {
                    sendBuf[index++] = msgBytes[i];
                    if (index == 19) {
                        try {
                            sendBuf[1] = 20;//发送一个20字节的包
//                            sendBuf[index] = ClsUtils.arraySum(sendBuf);//校验和
                            bleWrite(sendBuf);
                            sleep(15);
                            index = 3;
                            Arrays.fill(sendBuf, 3, 19, (byte) 0);
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    } else if (i + 1 == msgBytes.length && msgBytes.length % 16 != 0) {
                        sendBuf[1] = (byte) (msgBytes.length % 16 + 4);//最后一个包的包长
//                        sendBuf[19] = ClsUtils.arraySum(sendBuf);//校验和
                        bleWrite(sendBuf);//
                    }
                }
            }

        }

    }

    public class msgHandle extends Handler {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
//              蓝牙连接事件
                case 1:
                    connectBtn.setText("已连接");
                    bar.setVisibility(View.INVISIBLE);
                    break;
            }
        }
    }
}
