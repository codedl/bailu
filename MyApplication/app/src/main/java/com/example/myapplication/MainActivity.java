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
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
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
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintStream;
import java.net.Socket;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.List;
import java.util.NavigableSet;
import java.util.Set;

public class MainActivity extends AppCompatActivity implements NavigationView.OnNavigationItemSelectedListener {
    DrawerLayout drawerLayout;
    private ArrayList<String> fileStr = new ArrayList<>();
    private ArrayAdapter fileAdapter;
    public Spinner fileSpinner;

    private ArrayList<String> listStr = new ArrayList<>();
    private ArrayList<File> files = new ArrayList<>();
    private ArrayList<BluetoothDevice> devices = new ArrayList<>();
    private ArrayList<BluetoothGatt> gattArrayList = new ArrayList<>();
    private ArrayMap<String, BluetoothGatt> gattArrayMap = new ArrayMap<>();
    private ArrayList<String> addrStr = new ArrayList<>();
    private ArrayAdapter adapter;
    public ListView list;//显示蓝牙设备和文件名

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

    private BluetoothAdapter bleAdapter;
    boolean iscalled = false;
    private LocationManager lm;

    private BluetoothLowEnergy ble;
    private BluetoothClassic btc;
    private boolean isBt;
    private boolean isPlay;
    private boolean isUp;
    receiver btReceiver;
    audio audio;
    socket socket;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        init();
    }

    //创建设置菜单
    @Override
    public void onBackPressed() {
        DrawerLayout drawer = findViewById(R.id.drawer_layout);
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            drawer.closeDrawer(GravityCompat.START);
        } else {
            super.onBackPressed();
        }
    }

    //创建右上角设置功能
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.setting, menu);
        return true;
    }

    //右上角选项点击事件
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        switch (id) {
            case R.id.action_setting1:
                Toast.makeText(this, "setting1 selected", Toast.LENGTH_SHORT).show();
                break;
            case R.id.action_setting2:
                Toast.makeText(this, "setting2 selected", Toast.LENGTH_SHORT).show();
                break;
            default:
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    //菜单项目点击事件
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
                edit2.setText("8080");
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
                edit3.setText(param.httpServer);
                new AlertDialog.Builder(this).setTitle("设置服务器地址").setView(edit3).setPositiveButton("确定", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int i) {
                        if (edit3.getText().toString().trim().length() != 0) {
                            String str = edit3.getText().toString();
                            //bleText.setText("模块名:" + str + ";");
                            param.httpServer = str;
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
        audio = new audio(MainActivity.this);
        ble = new BluetoothLowEnergy(MainActivity.this);
        btc = new BluetoothClassic(MainActivity.this);
        IntentFilter intent = new IntentFilter(Action.BLE_SCAN_FOUND);
        intent.addAction(BluetoothDevice.ACTION_FOUND);
        intent.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        intent.addAction(BluetoothDevice.ACTION_PAIRING_REQUEST);
        btReceiver = new receiver();
        registerReceiver(btReceiver, intent);
        //列表上连接蓝牙
        list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                if (isBt) {
                    if (devices.size() == 0)
                        return;
                    BluetoothDevice btDevice = devices.get(i);
                    if (btDevice == null)
                        return;
                    System.out.println(btDevice.getName() + btDevice.getAddress());
                    btc.cancelDiscovery();
                    btc.createBond(btDevice);
                    btc.connect(btDevice);
                    devices.clear();
                    listStr.clear();
                    list.setAdapter(adapter);
                    isBt = false;
                }
                if (isPlay) {
                    System.out.println("path:" + files.get(i));
                    audio.mediaPlay(files.get(i).toString());
                }
                if (files.get(i) != null && isUp)
                    new fileManager().upload(param.httpServer, files.get(i).toString());
            }
        });

        list.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            @Override
            public boolean onItemLongClick(AdapterView<?> adapterView, View view, final int i, long l) {
                final File file = files.get(i);
                AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                builder.setTitle("文件属性");
                builder.setMessage(fileManager.fileInfo(file));
                builder.setPositiveButton("退出",null);
                builder.setNegativeButton("删除",new  DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int index) {
                        if(file.exists())
                            file.delete();
                        files.remove(i);
                        listStr.remove(i);
                        list.setAdapter(adapter);
                    }
                });
                builder.create().show();

                return true;//返回true则不会触发短按事件

            }
        });
    }


    public void layoutInit() {
        list = findViewById(R.id.main_list);
        adapter = new ArrayAdapter(this, android.R.layout.simple_list_item_1, listStr);


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
        //有按键发生时，列表功能重置
        isPlay = false;//取消点击列表播放音乐
        isBt = false;//取消点击列表的蓝牙连接
        isUp = false;//取消点击列表上传文件
        btc.cancelDiscovery();//取消搜索

        Date date = new Date(System.currentTimeMillis());
        SimpleDateFormat simpleDateFormat = new SimpleDateFormat("MMdd_HHmmss");
//        System.out.println(simpleDateFormat.format(date));
        String path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/audio/";
        if (new File(path).exists() != true) {
            new File(path).mkdir();

        }
        final String pcmfile = path + simpleDateFormat.format(date) + ".pcm";
        final String wavfile = path + simpleDateFormat.format(date) + ".wav";

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
            case R.id.model_btn:
                isBt = true;
                System.out.println("startDiscovery");
                listStr.clear();
                devices.clear();
                btc.startDiscovery();
                list.setVisibility(View.VISIBLE);
                break;

            case R.id.play_btn:
            case R.id.upfile_btn:
                if (btn.getId() == R.id.play_btn)
                    isPlay = true;
                if (btn.getId() == R.id.upfile_btn)
                    isUp = true;
                files.clear();
                listStr.clear();
                File file = new File(path);
                String[] fileList = file.list(new FilenameFilter() {
                    @Override
                    public boolean accept(File file, String s) {
                        File item = new File(file.getAbsolutePath() + "/" + s);
                        if (item.isDirectory())
                            return false;
                        else {
                            listStr.add(s);
                            files.add(item);
                            return true;
                        }
                    }
                });
                list.setVisibility(View.VISIBLE);
                list.setAdapter(adapter);
                break;
            case R.id.audio_btn:
                ToggleButton toggleButton = findViewById(R.id.audio_btn);
                if (toggleButton.isChecked()) {
                    audio.startAudioRecord(pcmfile);
                } else {
                    audio.stopAudioRecord();
                    audio.pcmToWav(pcmfile, wavfile);
                }
                break;
            /*case R.id.upfile_btn:
                Intent intent1 = new Intent(MainActivity.this, btActivity.class);
                startActivity(intent1);
                break;*/
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
                Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.RECORD_AUDIO, Manifest.permission.BLUETOOTH_PRIVILEGED};
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
                new Thread() {
                    byte buf0[] = new byte[]{(byte) 0x90, (byte) 0x00, (byte) 0x01, (byte) 0x20, (byte) 0x12, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x13, (byte) 0x08, (byte) 0x19, (byte) 0x0a
                            , (byte) 0x10, (byte) 0x10, (byte) 0x74, (byte) 0x9b, (byte) 0xe7, (byte) 0x42, (byte) 0x80, (byte) 0x4b
                            , (byte) 0xe5, (byte) 0x41, (byte) 0xcd, (byte) 0xcc, (byte) 0x4c, (byte) 0x3f, (byte) 0xa0};
                    byte buf1[] = new byte[]{(byte) 0x90, (byte) 0x00, (byte) 0x01, (byte) 0x20, (byte) 0x12, (byte) 0x00, (byte) 0x00
                            , (byte) 0x00, (byte) 0x13, (byte) 0x08, (byte) 0x19, (byte) 0x0a, (byte) 0x12, (byte) 0x10, (byte) 0x11, (byte) 0x97
                            , (byte) 0xe7, (byte) 0x42, (byte) 0x31, (byte) 0x52, (byte) 0xe5, (byte) 0x41, (byte) 0xcd, (byte) 0xcc, (byte) 0x4c, (byte) 0x3f, (byte) 0xa0};
                    byte buf2[] = new byte[]{(byte) 0x90, (byte) 0x00, (byte) 0x01, (byte) 0x71, (byte) 0x17, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x13
                            , (byte) 0x08, (byte) 0x19, (byte) 0x0a, (byte) 0x05, (byte) 0x10, (byte) 0xf8, (byte) 0x01, (byte) 0x01, (byte) 0x01, (byte) 0x01, (byte) 0x00, (byte) 0x80, (byte) 0x36
                            , (byte) 0x42, (byte) 0x01, (byte) 0x01, (byte) 0x01, (byte) 0x01, (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0xa0};
                    byte buf3[] = new byte[]{(byte) 0x90, (byte) 0x00, (byte) 0x01, (byte) 0x71, (byte) 0x17, (byte) 0x00, (byte) 0x00, (byte) 0x00, (byte) 0x13, (byte) 0x08, (byte) 0x19, (byte) 0x0a, (byte) 0x18, (byte) 0x10, (byte) 0xf7, (byte) 0x03
                            , (byte) 0x00, (byte) 0x01, (byte) 0x01, (byte) 0x00, (byte) 0x80, (byte) 0x36, (byte) 0x42, (byte) 0x02, (byte) 0x02, (byte) 0x02, (byte) 0x08, (byte) 0x02
                            , (byte) 0x01, (byte) 0x00, (byte) 0x00, (byte) 0xa0};

                    @Override
                    public void run() {
                        while (os != null) {
                            try {
                                sleep(2000);
                                os.write(buf0);
                                os.flush();
                                sleep(2000);
                                os.write(buf1);
                                os.flush();
                                sleep(2000);
                                os.write(buf2);
                                os.flush();
                                sleep(2000);
                                os.write(buf3);
                                os.flush();
                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }.start();
            } catch (Exception e) {
                e.printStackTrace();
            }
            int count = 0;
            while (param.clientSocket != null) {
                try {
                    sleep(100);
                    System.out.println("read");
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
            listStr.add(scanResult.toString());
            list.setAdapter(adapter);

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
                    Button btn = findViewById(R.id.model_btn);
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

    class receiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            String name, addr;
            System.out.println(intent.getAction());
            BluetoothDevice btDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
            switch (intent.getAction()) {
                case BluetoothDevice.ACTION_FOUND:
                    name = btDevice.getName();
                    addr = btDevice.getAddress();
                    System.out.println(name + addr);
                    if (devices.contains(btDevice))
                        return;
                    listStr.add(name + ":" + addr);
                    devices.add(btDevice);
                    list.setAdapter(adapter);
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
                    switch (btDevice.getBondState()) {
                        case BluetoothDevice.BOND_NONE:
                            System.out.println("BluetoothDevice.BOND_NONE");
                            break;
                        case BluetoothDevice.BOND_BONDING:
                            System.out.println("BluetoothDevice.BOND_BONDING");
                            break;
                        case BluetoothDevice.BOND_BONDED:
                            BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                            //audio.connect(device);
                            System.out.println("BluetoothDevice.BOND_BONDED");
                            break;
                    }
                    break;
            }
        }
    }
}
