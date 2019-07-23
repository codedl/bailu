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
import android.os.Message;
import android.provider.Settings;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.Adapter;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.example.myapp.R;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class MainActivity extends AppCompatActivity {
    private Spinner freqway_intwaySpin;
    private ArrayList<String> freqway_intwayStr = new ArrayList<>();
    private ArrayAdapter freqway_intwayAdapt;
    private Spinner comModeSpin;
    private ArrayList<String> comModeStr = new ArrayList<>();
    private ArrayAdapter comModeAdapt;
    private Spinner freqrange_intbandSpin;
    private ArrayList<String> freqrange_intbandStr = new ArrayList<>();
    private ArrayAdapter freqrange_intbandAdapt;
    private Spinner freqspeedSpin;
    private ArrayList<String> freqspeedStr = new ArrayList<>();
    private ArrayAdapter freqspeedAdapt;
    private Spinner demodwaySpin;
    private ArrayList<String> demodwayStr = new ArrayList<>();
    private ArrayAdapter demodwayAdapt;
    private Spinner demodsourceSpin;
    private ArrayList<String> demodsourceStr = new ArrayList<>();
    private ArrayAdapter demodsourceAdapt;
    private Spinner msg_noiseSpin;
    private ArrayList<String> msg_noiseStr = new ArrayList<>();
    private ArrayAdapter msg_noiseAdapt;

    private TextView freqrange_intbandText;
    private TextView freqspeedText;
    private EditText freqEdit;
    private EditText msgEdit;
    private TextView msgText;
    private EditText powerEdit;
    private Button freqlistBtn;
    private TextView freqway_intwayText;
    private TextView msg_noiseText;

    private double randomArray[];

    BluetoothAdapter bleAdapt;
    BluetoothGatt bleGatt;
    BluetoothDevice bleDevice;
    BluetoothGattCharacteristic red;
    BluetoothGattCharacteristic green;
    boolean iscalled;

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

    void bleInit() {
        //打开蓝牙
        Intent intent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        startActivityForResult(intent, 1);

        BluetoothManager bm = (BluetoothManager) getSystemService(BLUETOOTH_SERVICE);
        bleAdapt = bm.getAdapter();
    }

    void comModeInit() {
        freqway_intwayText.setText("频率方式");
        freqrange_intbandText.setText("跳频范围");
        freqspeedText.setText("跳频速率");
        msg_noiseText.setText("报文:");

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

        demodsourceStr.clear();
        demodsourceStr.add("语音报文");
        demodsourceStr.add("手咪语音");
        demodsourceSpin.setAdapter(demodsourceAdapt);

        msg_noiseStr.clear();
        msg_noiseStr.add("报文1");
        msg_noiseStr.add("报文2");
        msg_noiseStr.add("报文3");
        msg_noiseSpin.setVisibility(View.VISIBLE);
        msg_noiseSpin.setAdapter(msg_noiseAdapt);
        msgEdit.setVisibility(View.INVISIBLE);
        msgText.setVisibility(View.INVISIBLE);

        demodwaySpin.setOnItemSelectedListener(comDemodWaySpinListener);
    }

    void layoutInit() {
        msg_noiseText = findViewById(R.id.noise_msg_text);
        freqway_intwayText = findViewById(R.id.intway_freqway_text);
        freqrange_intbandText = findViewById(R.id.intband_freqrange_text);
        freqspeedText = findViewById(R.id.freqspeed_text);
        freqEdit = findViewById(R.id.freq_edit);
        powerEdit = findViewById(R.id.power_edit);
        freqlistBtn = findViewById(R.id.speed_btn);
        msg_noiseSpin = findViewById(R.id.noise_msgcontent_spin);
        msg_noiseAdapt = new ArrayAdapter(this, android.R.layout.simple_list_item_1, msg_noiseStr);
        demodsourceSpin = findViewById(R.id.demodsource_spin);
        demodsourceAdapt = new ArrayAdapter(this, android.R.layout.simple_list_item_1, demodsourceStr);
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
        msgText = findViewById(R.id.noise_msgcontent_text);

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
        demodwaySpin.setAdapter(demodwayAdapt);
        demodwaySpin.setSelection(0, true);
        demodwaySpin.setOnItemSelectedListener(comDemodWaySpinListener);

    }

    void intModeInit() {
        freqway_intwayText.setText("干扰样式");
        freqway_intwaySpin.setOnItemSelectedListener(null);
        freqway_intwayStr.clear();
        freqway_intwayStr.add("瞄准式");
        freqway_intwayStr.add("扫频式");
        freqway_intwaySpin.setAdapter(freqway_intwayAdapt);
        freqway_intwaySpin.setSelection(0, true);
        freqway_intwaySpin.setOnItemSelectedListener(intWaySpinListener);

        freqrange_intbandText.setText("干扰带宽");
        freqrange_intbandText.setTextColor(Color.parseColor("#4CAF50"));
        freqrange_intbandStr.clear();
        freqrange_intbandStr.add("5KHz");
        freqrange_intbandStr.add("10KHz");
        freqrange_intbandStr.add("25KHz");
        freqrange_intbandSpin.setAdapter(freqrange_intbandAdapt);
        freqrange_intbandSpin.setEnabled(true);
        freqrange_intbandSpin.setOnItemSelectedListener(null);

        freqspeedText.setText("扫频速率");
        freqspeedText.setTextColor(Color.parseColor("#8A000000"));
        freqspeedStr.clear();
        freqspeedStr.add("1KHz");
        freqspeedSpin.setAdapter(freqspeedAdapt);
        freqspeedSpin.setEnabled(false);

        demodsourceStr.clear();
        demodsourceStr.add("噪声");
        demodsourceSpin.setAdapter(demodsourceAdapt);

        msg_noiseText.setText("噪声:");
        msgText.setText("高斯白噪声");
        msgText.setVisibility(View.VISIBLE);
        msg_noiseSpin.setVisibility(View.INVISIBLE);
        msgEdit.setVisibility(View.INVISIBLE);

        demodwaySpin.setOnItemSelectedListener(null);

        demodwayStr.clear();
        demodwayStr.add("AM");
        demodwayStr.add("USB");
        demodwayStr.add("LSB");
        demodwayStr.add("DSB");
        demodwayStr.add("CW");
        demodwayStr.add("FSK");
        demodwayStr.add("FM");
        demodwayStr.add("多音FSK");
        demodwaySpin.setAdapter(demodwayAdapt);
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
                for (int i = 0; i < randomArray.length; i++) {
                    str = "         ";
                    str += (String.format("%03d", (i + 1)));
                    str += "              ";
                    str += (String.format("%4.6f", randomArray[i]));
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
                new bleSend().start();
                debug("开始发送", false);
                break;

            case R.id.connect_btn:
                final EditText edit = new EditText(this);
                edit.setText(param.bleName);
                AlertDialog.Builder editBuilder = new AlertDialog.Builder(this);
                editBuilder.setTitle("输入设备名")
                        .setView(edit)
                        .setPositiveButton("确定", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialogInterface, int i) {
                                if (edit.getText().toString().length() != 0) {
                                    param.bleName = edit.getText().toString().trim();
                                } else {
                                    debug("输入不能为空", false);
                                }
                            }
                        })
                        .setNegativeButton("取消", null)
                        .create().show();
//                bleAdapt.getBluetoothLeScanner().startScan(scanCallBack);
                break;
        }
    }

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
                debug("discover service", true);
            }
        }
    };

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
            debug("onServicesDiscovered", true);
            List<BluetoothGattService> gattServices = gatt.getServices();
            for (BluetoothGattService bgs : gattServices) {
                String uuid = bgs.getUuid().toString();
                debug("servers uuid:" + uuid, true);
                List<BluetoothGattCharacteristic> gattCharacteristics = bgs.getCharacteristics();
                for (BluetoothGattCharacteristic bgc : gattCharacteristics) {
                    uuid = bgc.getUuid().toString();
                    if (uuid.equals("f0001111-0451-4000-b000-000000000000")) {
                        red = bgc;//LED红
                    }
                    if (uuid.equals("f0001112-0451-4000-b000-000000000000")) {
                        green = bgc;//LED绿
                    }
                  /*  if (uuid.equals("f0001121-0451-4000-b000-000000000000")) {
                        btnbgc = bgc;//按键
                    }*/
                    debug("character uuid:" + uuid, true);

                    int charaProp = bgc.getProperties();
                    if ((charaProp & BluetoothGattCharacteristic.PROPERTY_READ) > 0) {
                        debug("可读", true);
                    }
                    if ((charaProp & BluetoothGattCharacteristic.PROPERTY_WRITE) > 0) {
                        debug("可写", true);

                    }
                    if ((charaProp & BluetoothGattCharacteristic.PROPERTY_NOTIFY) > 0) {
                        debug("通知", true);

                    }

                }
            }
//            new ReadCharacteristic(green).start();
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            super.onCharacteristicRead(gatt, characteristic, status);
            byte[] buf = characteristic.getValue();
            String str = "";
            for (int i = 0; i < buf.length; i++) {
                str += String.format("0x%02x;", buf[i]);
            }
            debug("onCharacteristicRead:" + str, true);
            iscalled = true;
        }

        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            super.onCharacteristicWrite(gatt, characteristic, status);
            byte[] writebuf = characteristic.getValue();
            debug("onCharacteristicWrite:" + ClsUtils.toHexString(writebuf), true);
            iscalled = true;
//            bleGatt.readCharacteristic(characteristic);
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

    public void bleWrite(BluetoothGatt blegatt, BluetoothGattCharacteristic characteristic, byte val[]) {
        blegatt.setCharacteristicNotification(characteristic, true);
        characteristic.setValue(val);
        characteristic.setWriteType(characteristic.WRITE_TYPE_NO_RESPONSE);
        blegatt.writeCharacteristic(characteristic);
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
                    iscalled = false;
                    sleep(3000);

                } catch (Exception e) {

                }
            }

        }
    }

    public class bleSend extends Thread {

        @Override
        public void run() {
            System.out.println("send");
            powerfreqSend();
//            paramSend();
        }
    }

    public void powerfreqSend() {
        int index = 0;
        byte doubuf[] = new byte[8];
        byte sendbuf[] = new byte[20];
        sendbuf[index++] = 0x0D;
        sendbuf[index++] = 16;
        sendbuf[index++] = 1;
        doubuf = ClsUtils.doubleToBytes(param.power*1000000);
        for (byte tmp : doubuf) {
            sendbuf[index++] = tmp;
        }
        doubuf = ClsUtils.doubleToBytes(param.freq);
        for (byte tmp : doubuf) {
            sendbuf[index++] = tmp;
        }
        bleWrite(bleGatt, green, sendbuf);
    }

    public void paramSend() {
        int index = 0;
        byte sendbuf[] = new byte[20];
        sendbuf[index++] = 0x0A;
        sendbuf[index++] = 7;
        sendbuf[index++] = 1;
        sendbuf[index++] = (byte) (comModeSpin.getSelectedItemPosition() + 0xf0);
        sendbuf[index++] = (byte) (freqway_intwaySpin.getSelectedItemPosition() + 0xf1);
        sendbuf[index++] = (byte) (demodwaySpin.getSelectedItemPosition() + 0xf1);
        sendbuf[index++] = (byte) (demodsourceSpin.getSelectedItemPosition() + 0xf1);
        sendbuf[index++] = (byte) (msg_noiseSpin.getSelectedItemPosition() + 0xf1);
        sendbuf[index++] = (byte) (freqrange_intbandSpin.getSelectedItemPosition() + 0xf1);
        sendbuf[index++] = (byte) (freqspeedSpin.getSelectedItemPosition() + 0xf1);
        bleWrite(bleGatt, green, sendbuf);
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
                    freqEdit.setText(String.format("%4.6f", param.freq));
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
                    powerEdit.setText(String.format("%1.3f", param.power));
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
            double freq = Double.parseDouble(freqEdit.getText().toString().trim());

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
            int a[] = new int[length];
            int rand = 0;
            Random random = new Random();
            while (index < length - 1) {
                rand = random.nextInt(randnum);
                while (isInArray(rand, a)) {
                    rand = random.nextInt(randnum);
                }
                a[index++] = rand;
            }

            rand = random.nextInt(length);
            a[length - 1] = a[rand];
            a[rand] = 0;

            randomArray = new double[length];
            for (int randomIndex = 0; randomIndex < length; randomIndex++) {
                randomArray[randomIndex] = freq + (double) a[randomIndex] * 4 / 1000;
                System.out.println(randomArray[randomIndex]);
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
                case 6:
                    msg_noiseSpin.setVisibility(View.VISIBLE);
                    msgEdit.setVisibility(View.INVISIBLE);
                    msgText.setVisibility(View.INVISIBLE);
                    break;
                case 4:
                    msg_noiseSpin.setVisibility(View.INVISIBLE);
                    msgEdit.setVisibility(View.VISIBLE);
                    msgText.setVisibility(View.INVISIBLE);
                    break;
                case 5:
                case 7:
                    msg_noiseSpin.setVisibility(View.INVISIBLE);
                    msgEdit.setVisibility(View.INVISIBLE);
                    msgText.setVisibility(View.VISIBLE);
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
}
