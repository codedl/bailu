package com.example.myapplication;

import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBarDrawerToggle;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.core.view.GravityCompat;
import androidx.drawerlayout.widget.DrawerLayout;

import android.Manifest;
import android.app.ProgressDialog;
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
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.util.ArrayMap;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.provider.Settings;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Chronometer;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import com.google.android.material.navigation.NavigationView;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.NavigableSet;
import java.util.Set;

public class MainActivity extends AppCompatActivity implements NavigationView.OnNavigationItemSelectedListener {
    DrawerLayout drawerLayout;
    private ArrayList<String> fileStr = new ArrayList<>();
    private ArrayAdapter fileAdapter;
    public Spinner fileSpinner;

    private ArrayList<String> devicesStr = new ArrayList<>();
    private ArrayList<BluetoothDevice> devices = new ArrayList<>();
    private ArrayList<BluetoothGatt> gattArrayList = new ArrayList<>();
    private ArrayMap<String, BluetoothGatt> gattArrayMap = new ArrayMap<>();
    private ArrayList<String> addrStr = new ArrayList<>();
    private ArrayAdapter devAdapter;
    public ListView devList;

    private Button socketconnectBtn;
    private Button upfileBtn;
    private Button bleconnectBtn;
    private Button moduleonBtn;
    private TextView bleText;
    private TextView ipText;
    private TextView gpsText;
    private TextView userText;
    private ToggleButton onBtn;
    private Button bleConnectBtn;
    private ToggleButton socketConnectBtn;
    private Chronometer chronometer;
    private Chronometer bleconnect_time;
    private Chronometer socketconnect_time;

    private ProgressBar bleBar;
    private ProgressBar socketBar;
    public handle mainHandle;
    private OutputStream os;
    private InputStream is;

    private BluetoothDevice bleDevice = null;
    private BluetoothGatt bleGatt;
    private BluetoothGattCharacteristic red;
    private BluetoothGattCharacteristic green;
    private BluetoothGattCharacteristic btnbgc;
    private BluetoothGattCharacteristic xdt;

    private byte ledon;
    boolean isFirstPressed = true;
    private BluetoothAdapter bleAdapter;
    boolean iscalled = false;
    private LocationManager lm;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        init();
    }

    @Override
    public void onBackPressed() {
        DrawerLayout drawer = findViewById(R.id.drawer_layout);
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            drawer.closeDrawer(GravityCompat.START);
        } else {
            super.onBackPressed();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.setting, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        switch (id) {
            case R.id.action_setting1:
                Toast.makeText(this, "settting1 selected", Toast.LENGTH_SHORT).show();
                break;
            case R.id.action_setting2:
                Toast.makeText(this, "setting2 selected", Toast.LENGTH_SHORT).show();
                break;
            default:
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    @SuppressWarnings("StatementWithEmptyBody")
    @Override
    public boolean onNavigationItemSelected(MenuItem item) {
        int id = item.getItemId();
        switch (id) {
            case R.id.user:
                View view = this.getLayoutInflater().inflate(R.layout.log, null);
                final EditText passwordEdit = view.findViewById(R.id.password_edit);
                final EditText usernameEdit = view.findViewById(R.id.username_edit);

                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                builder.setView(view);
                builder.setTitle("用户登录");
                builder.setPositiveButton("确定", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        param.username = usernameEdit.getText().toString().trim();
                        param.password = passwordEdit.getText().toString().trim();
                        // userText.setText("用户编号:" + param.username);
                    }
                });
                builder.setNegativeButton("取消", null);
                builder.create().show();
                break;
            case R.id.IP:
                final EditText edit1 = new EditText(this);
//                    edit1.setHint("请输入ip地址");
                edit1.setText(param.ServerHostIP);
                new AlertDialog.Builder(this).setTitle("设置主控IP地址").setView(edit1).setPositiveButton("确定", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        if (edit1.getText().toString().length() != 0) {
                            String str = edit1.getText().toString();
                            param.ServerHostIP = str;
                            ipText.setText("主控IP地址:" + str);

                        } else {
                            Toast.makeText(MainActivity.this, "输入不能为空", Toast.LENGTH_SHORT).show();
                        }
                    }
                }).setNegativeButton("取消", null).create().show();
                break;
            case R.id.port:
                final EditText edit2 = new EditText(this);
                AlertDialog.Builder builder2 = new AlertDialog.Builder(this);
                builder2.setView(edit2);
                builder2.setTitle("设置主控端口号");
                builder2.setPositiveButton("确定", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        if (edit2.getText().toString().length() != 0) {
                            String str = edit2.getText().toString();
                            param.ServerHostPort = Integer.parseInt(str);
                            Toast.makeText(MainActivity.this, "端口号:" + param.ServerHostPort, Toast.LENGTH_SHORT).show();

                        } else {
                            Toast.makeText(MainActivity.this, "输入不能为空", Toast.LENGTH_SHORT).show();
                        }
                    }
                });
                builder2.create().show();
                break;
            case R.id.ble:
                final EditText edit3 = new EditText(this);
                edit3.setText(param.bleName);
                new AlertDialog.Builder(this).setTitle("设置模块名称").setView(edit3).setPositiveButton("确定", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        if (edit3.getText().toString().trim().length() != 0) {
                            String str = edit3.getText().toString();
                            bleText.setText("模块名:" + str + ";");
                            param.bleName = str;
                        } else {
                            Toast.makeText(MainActivity.this, "输入不能为空", Toast.LENGTH_SHORT).show();
                        }
                    }
                }).setNegativeButton("取消", null).show();
                break;
        }
        return true;
    }

    public void init() {
        checkPermissions();
        layoutInit();
        gpsInit();
        bleInit();
        mainHandle = new handle();

    }

    public void bleInit() {
        //打开蓝牙
        Intent btEnable = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        startActivityForResult(btEnable, 1);
        //判断是否支持蓝牙ble
        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Toast.makeText(this, "不支持蓝牙ble", Toast.LENGTH_SHORT).show();
            finish();
        }
        //获取蓝牙适配器
        BluetoothManager bm = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        bleAdapter = bm.getAdapter();

        devList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {

                bleAdapter.getBluetoothLeScanner().stopScan(scanCallBack);
                BluetoothDevice bluetoothDevice = devices.get(i);
                BluetoothGatt gatt = bluetoothDevice.connectGatt(MainActivity.this, true, gattCallBack);
                devices.clear();
                devicesStr.clear();
                devList.setAdapter(devAdapter);
            }
        });
    }


    public void layoutInit() {
        fileSpinner = (Spinner) findViewById(R.id.file_list);
        fileAdapter = new ArrayAdapter(this, android.R.layout.simple_list_item_1, fileStr);
        devList = findViewById(R.id.devices_list);
        devAdapter = new ArrayAdapter(this, android.R.layout.simple_list_item_1, devicesStr);
        fileStr.add("文件列表");
        fileStr.add("FILE1");
        fileStr.add("FILE2");
        fileStr.add("FILE3");
        fileSpinner.setAdapter(fileAdapter);
        fileSpinner.setOnItemSelectedListener(new Spinner.OnItemSelectedListener() {
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                if (i != 0)
                    Toast.makeText(MainActivity.this, "选择了" + fileStr.get(i).toString(), Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
//        moduleonBtn = (Button) findViewById(R.id.ON_text);
        //moduleonBtn.setEnabled(false);
        ipText = (TextView) findViewById(R.id.ip_text);
        bleText = (TextView) findViewById(R.id.ble_text);
        /*gpsText = (TextView) findViewById(R.id.gps_text);
        userText = (TextView) findViewById(R.id.user_text);*/
//        bleConnectBtn = (ToggleButton) findViewById(R.id.bleconnect_btn);
        //onBtn = (ToggleButton) findViewById(R.id.ON_btn);
        socketConnectBtn = (ToggleButton) findViewById(R.id.socketconnect_btn);
        socketBar = (ProgressBar) findViewById(R.id.socket_bar);
        bleBar = (ProgressBar) findViewById(R.id.ble_bar);
        chronometer = (Chronometer) findViewById(R.id.timer_chr);
        chronometer.setFormat("工作时长:%s");
        bleconnect_time = (Chronometer) findViewById(R.id.contime_chr);
        bleconnect_time.setFormat("%s");
        socketconnect_time = (Chronometer) findViewById(R.id.socktime_chr);
        socketconnect_time.setFormat("%s");

        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        drawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawerLayout, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawerLayout.addDrawerListener(toggle);
        toggle.syncState();
        NavigationView navigationView = findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);
    }

    public void gpsInit() {
        lm = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
        if (!isGPSable(lm)) {
            Intent intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
            startActivityForResult(intent, 0);
        }
        if (checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            Log.d("print", "permission failed");
        }
        Location lc = lm.getLastKnownLocation(LocationManager.GPS_PROVIDER);
        updateShow(lc);
        lm.requestLocationUpdates(LocationManager.GPS_PROVIDER, 100, 1, new LocationListener() {
            @Override
            public void onLocationChanged(Location location) {
                updateShow(location);
            }

            @Override
            public void onStatusChanged(String s, int i, Bundle bundle) {

            }

            @Override
            public void onProviderEnabled(String s) {
                if (checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {

                }
                updateShow(lm.getLastKnownLocation(s));
            }

            @Override
            public void onProviderDisabled(String s) {
                updateShow(null);
            }
        });

    }

    private void updateShow(Location location) {
        if (location != null) {
            StringBuilder sb = new StringBuilder();
            sb.append("位置:");
            sb.append("经度" + String.format("%3.9f", location.getLongitude()));
            sb.append(";纬度" + String.format("%3.9f", location.getLatitude()));
            //  gpsText.setText(sb.toString());
        } else {
            //gpsText.setText("位置:经度117.14;纬度31.84");
        }
    }

    public boolean isGPSable(LocationManager lm) {
        return lm.isProviderEnabled(LocationManager.GPS_PROVIDER) ? true : false;
    }

    public void clickEvent(View btn) throws IOException {
        Intent intent = null;
        switch (btn.getId()) {
            case R.id.socketconnect_btn:
//               socketConnectBtn.setBackgroundColor(Color.parseColor("#FFFFFF"));

                if (socketConnectBtn.isChecked()) {

                    new socketConnect().start();
                    socketConnectBtn.setBackgroundResource(R.drawable.connecting);
                    socketBar.setVisibility(View.VISIBLE);
                    socketconnect_time.setVisibility(View.VISIBLE);
                    socketconnect_time.setBase(SystemClock.elapsedRealtime());
                    socketconnect_time.start();
                } else {

                    if (param.clientSocket != null) {
                        param.clientSocket.close();
                        param.clientSocket = null;
                    }
                    socketConnectBtn.setBackgroundResource(R.drawable.disconnect);
                    socketBar.setVisibility(View.INVISIBLE);
                    socketconnect_time.setVisibility(View.INVISIBLE);
                    socketconnect_time.stop();
                    Toast.makeText(MainActivity.this, "断开连接", Toast.LENGTH_SHORT).show();
                }
                break;
            case R.id.over_btn:
                /*if (bleConnectBtn.isChecked()) {
                    bleAdapter.getBluetoothLeScanner().startScan(scanCallBack);
                    bleConnectBtn.setBackgroundResource(R.drawable.connecting);
                    bleBar.setVisibility(View.VISIBLE);
                    bleconnect_time.setVisibility(View.VISIBLE);
                    bleconnect_time.setBase(SystemClock.elapsedRealtime());
                    bleconnect_time.start();
                } else {
                    if (bleGatt != null) {
                        bleGatt.close();
                        bleGatt = null;
                    }
                    chronometer.stop();
                    bleconnect_time.stop();
                    bleconnect_time.setVisibility(View.INVISIBLE);
                    bleAdapter.getBluetoothLeScanner().stopScan(scanCallBack);
                    bleConnectBtn.setBackgroundResource(R.drawable.disconnect);
                    bleBar.setVisibility(View.INVISIBLE);
                    Toast.makeText(MainActivity.this, "断开连接", Toast.LENGTH_SHORT).show();
                }*/
                devices.clear();
                devicesStr.clear();
                devList.setAdapter(devAdapter);
                bleAdapter.getBluetoothLeScanner().stopScan(scanCallBack);
                break;

            case R.id.scan_btn:
                Toast.makeText(this, "扫描蓝牙设备", Toast.LENGTH_SHORT).show();
//                bleAdapter.getBluetoothLeScanner().startScan(scanCallBack);
//                bleAdapter.startLeScan(mLeScanCallback);
                new bleScan().start();
                break;
            case R.id.connect_btn:
               /* devicesStr.clear();
                Set<BluetoothDevice> devices = bleAdapter.getBondedDevices();
                for (BluetoothDevice device : devices) {
                    if (device != null) {
                        String name = device.getName();
                        String addr = device.getAddress();
                        devicesStr.add(name + addr);
                    }
                }
                devList.setAdapter(devAdapter);*/
                break;
            case R.id.upfile_btn:
                Intent intent1 = new Intent(MainActivity.this, btActivity.class);
                startActivity(intent1);
                break;
        }
    }

    public class bleScan extends Thread {
        public void run() {
            bleAdapter.getBluetoothLeScanner().startScan(scanCallBack);
        }
    }

    ;

    /**
     * 检查权限,gps定位，sd卡读写文件，录音
     */
    private void checkPermissions() {
        String[] permissions = {Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.WRITE_EXTERNAL_STORAGE,
                Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.RECORD_AUDIO};
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

   /* *//**
     * 权限回调
     *
     * @param requestCode
     * @param permissions
     * @param grantResults
     *//*
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
    }*/

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

    public class socketConnect extends Thread {
        String str;
        Message msg;
        byte buf[] = new byte[1024];
        byte val[] = new byte[1];

        @Override
        public void run() {
            try {
                param.clientSocket = new Socket(param.ServerHostIP, param.ServerHostPort);

            } catch (IOException e) {
                e.printStackTrace();
            }
            if (param.clientSocket != null) {
                param.instruction = null;
                msg = mainHandle.obtainMessage();
                msg.what = 1;
                msg.arg1 = 1;
                mainHandle.sendMessage(msg);
            }
            try {
                is = param.clientSocket.getInputStream();
                os = param.clientSocket.getOutputStream();
                new send("#id:ABABABABABAB|gps:115.8,28.661667|state:init$").start();

            } catch (Exception e) {
                e.printStackTrace();
            }
            int count = 0;
            while (param.clientSocket != null) {
                try {
                    sleep(100);
                    count = is.read(buf);
                    if (count > 0) {
                        str = new String(buf, 0, count);
                        param.instruction = str;
                        msg = mainHandle.obtainMessage();
                        msg.obj = str;
                        msg.arg1 = count;
                        msg.what = 2;
                        mainHandle.sendMessage(msg);
                        System.out.println(str);
                        if ("on".equals(str)) {
                            val[0] = 1;
                            if (red != null) {
                                bleWrite(bleGatt, red, val);
                            }
                        }
                        if ("off".equals(str)) {
                            val[0] = 0;
                            if (red != null) {
                                bleWrite(bleGatt, red, val);
                            }
                        }
                    }

                } catch (Exception e) {
                    msg = mainHandle.obtainMessage();
                    msg.what = 1;
                    msg.arg1 = 3;
                    mainHandle.sendMessage(msg);
                    e.printStackTrace();
                    try {
                        os.close();
                    } catch (Exception f) {
                        f.printStackTrace();
                    }
                    break;
                }
            }

            msg = mainHandle.obtainMessage();
            msg.what = 1;
            msg.arg1 = 3;
            mainHandle.sendMessage(msg);
        }
    }

    public class handle extends Handler {

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case 1:
                    if (msg.arg1 == 1) {
                        socketConnectBtn.setText("已连接");
                        socketConnectBtn.setBackgroundResource(R.drawable.radius);
                        socketBar.setVisibility(View.INVISIBLE);
                        socketconnect_time.setVisibility(View.INVISIBLE);
                        socketconnect_time.stop();
                    } else if (msg.arg1 == 2) {
                        chronometer.setVisibility(View.VISIBLE);
                        chronometer.start();
                        bleConnectBtn.setBackgroundResource(R.drawable.radius);
                        bleConnectBtn.setText("已连接");
                        bleBar.setVisibility(View.INVISIBLE);
                        bleconnect_time.setVisibility(View.INVISIBLE);
                        bleconnect_time.stop();
                    } else if (msg.arg1 == 3) {
                        socketConnectBtn.setBackgroundResource(R.drawable.disconnect);
                        socketConnectBtn.setText("连接断开");
                        socketBar.setVisibility(View.INVISIBLE);
                        socketconnect_time.setVisibility(View.INVISIBLE);
                        socketconnect_time.stop();
                    } else if (msg.arg1 == 4) {
                        chronometer.stop();
                        bleConnectBtn.setBackgroundResource(R.drawable.disconnect);
                        bleConnectBtn.setText("连接断开");
                        bleBar.setVisibility(View.INVISIBLE);
                        bleconnect_time.setVisibility(View.INVISIBLE);
                        bleconnect_time.stop();
                    }
                    break;
                case 2:
                    if ("off".equals(param.instruction)) {
                        new send("offok").start();
                        new send("#id:ABABABABABAB|gps:115.8,28.661667|state:closed$").start();


                    } else if ("on".equals(param.instruction)) {
                        new send("onok").start();
                        new send("#id:ABABABABABAB|gps:115.8,28.661667|state:opened$").start();

                    }
                    param.instruction = null;
                    break;
            }
        }
    }

    public class send extends Thread {
        private String sendsStr;

        public send(String str) {
            this.sendsStr = str;
        }

        @Override
        public void run() {
            if (param.clientSocket != null && param.clientSocket.isConnected()) {
                try {
                    if (os != null) {
                        os.write(sendsStr.getBytes("UTF-8"));
                        os.flush();
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    os = null;
                    System.out.println("write exception");
                }
            } else {
                os = null;
                is = null;
                param.clientSocket = null;
            }
        }

    }

    //定时读取led状态判断ble是否掉线
    public class ReadCharacteristic extends Thread {
        BluetoothGattCharacteristic characteristic;
        BluetoothGatt gatt;
        Message msg = null;

        public ReadCharacteristic(BluetoothGattCharacteristic temp, BluetoothGatt gatt) {
            this.characteristic = temp;
            this.gatt = gatt;
        }

        @Override
        public void run() {
            Log.d("print", "connect succeed");
            Message msg = mainHandle.obtainMessage();
            msg.what = 1;
            msg.arg1 = 2;
            mainHandle.sendMessage(msg);
            while (gatt != null && characteristic != null) {
                try {
                    gatt.readCharacteristic(characteristic);
                    iscalled = false;
                    sleep(3000);
                    if (iscalled == false) {
                        msg = mainHandle.obtainMessage();
                        msg.what = 1;
                        msg.arg1 = 4;
                        mainHandle.sendMessage(msg);
                    }
                } catch (Exception e) {
                    msg = mainHandle.obtainMessage();
                    msg.what = 1;
                    msg.arg1 = 4;
                    mainHandle.sendMessage(msg);
                    e.printStackTrace();
                }
            }
            msg = mainHandle.obtainMessage();
            msg.what = 1;
            msg.arg1 = 4;
            mainHandle.sendMessage(msg);
        }
    }

    //扫描蓝牙的回调函数
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
            Log.d("print", scanResult.toString());

            if (name == null || devices.contains(dev))
                return;

            devices.add(dev);
            devicesStr.add(scanResult.toString());
            devList.setAdapter(devAdapter);

           /* if (param.bleName.equals(name)) {
                bleDevice = result.getDevice();
                bleAdapter.getBluetoothLeScanner().stopScan(scanCallBack);
                bleGatt = bleDevice.connectGatt(MainActivity.this, true, gattCallBack);
                Log.d("print", "discover device");
            }*/
        }
    };

    //蓝牙连接回调函数
    public BluetoothGattCallback gattCallBack = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            super.onConnectionStateChange(gatt, status, newState);
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                if (gatt != null) {
                    bleGatt = gatt;
                    String addr = gatt.getDevice().getAddress();
                    System.out.println(addr);
                    gatt.discoverServices();
                    Button btn = findViewById(R.id.over_btn);
                    btn.setText("已连接");
                    btn.setBackgroundResource(R.drawable.radius);
                }
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            super.onServicesDiscovered(gatt, status);
            Log.d("print", "onServicesDiscovered");
            List<BluetoothGattService> gattServices = gatt.getServices();
            for (BluetoothGattService bgs : gattServices) {
                String uuid = bgs.getUuid().toString();
                Log.d("print", "servers uuid:" + uuid);
                List<BluetoothGattCharacteristic> gattCharacteristics = bgs.getCharacteristics();
                for (BluetoothGattCharacteristic bgc : gattCharacteristics) {
                    uuid = bgc.getUuid().toString();
                    if (uuid.equals("f0001111-0451-4000-b000-000000000000")) {
                        red = bgc;//LED红
                    }
                    if (uuid.equals("f0001112-0451-4000-b000-000000000000")) {
                        green = bgc;//LED绿
                    }
                    if (uuid.equals("f0001121-0451-4000-b000-000000000000")) {
                        btnbgc = bgc;//按键
                    }
                    if (uuid.equals("0000fee1-0000-1000-8000-00805f9b34fb")) {
                        xdt = bgc;
                    }
                    Log.d("print", "character uuid:" + uuid);

                    int charaProp = bgc.getProperties();
                    if ((charaProp & BluetoothGattCharacteristic.PROPERTY_READ) > 0) {
                        Log.d("print", "可读");
                    }
                    if ((charaProp & BluetoothGattCharacteristic.PROPERTY_WRITE) > 0) {
                        Log.d("print", "可写");

                    }
                    if ((charaProp & BluetoothGattCharacteristic.PROPERTY_NOTIFY) > 0) {
                        Log.d("print", "通知");
                    }

                }
            }
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            super.onCharacteristicRead(gatt, characteristic, status);
            byte[] buf = characteristic.getValue();
            String str = "";
            for (int i = 0; i < buf.length; i++) {
                str += String.format("0x%02x;", buf[i]);
            }
            Log.d("print", "onCharacteristicRead:" + str);
            iscalled = true;
        }

        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            super.onCharacteristicWrite(gatt, characteristic, status);
            byte[] writebuf = characteristic.getValue();
            Log.d("print", "onCharacteristicWrite:" + ClsUtils.toHexString(writebuf));
            iscalled = true;
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            super.onCharacteristicChanged(gatt, characteristic);
            Log.d("print", "onCharacteristicChanged");
        }

        @Override
        public void onDescriptorRead(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            super.onDescriptorRead(gatt, descriptor, status);
            Log.d("print", "onDescriptorRead");

        }

        @Override
        public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            super.onDescriptorWrite(gatt, descriptor, status);
            Log.d("print", "onDescriptorWrite");
        }
    };

    public void bleWrite(BluetoothGatt blegatt, BluetoothGattCharacteristic characteristic, byte val[]) {
//        blegatt.setCharacteristicNotification(characteristic, true);
        characteristic.setValue(val);
//        characteristic.setWriteType(characteristic.WRITE_TYPE_NO_RESPONSE);
        blegatt.writeCharacteristic(characteristic);
    }
}
