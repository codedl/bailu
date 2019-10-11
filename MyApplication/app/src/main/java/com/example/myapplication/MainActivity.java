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
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;

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
import android.view.Window;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Chronometer;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.Spinner;
import android.widget.Switch;
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
import java.util.Collections;
import java.util.Date;
import java.util.List;
import java.util.NavigableSet;
import java.util.Set;

public class MainActivity extends AppCompatActivity implements NavigationView.OnNavigationItemSelectedListener {
    private DrawerLayout drawerLayout;//左侧菜单栏
    private LocationManager lm;//定位
    private TextView txt_zhukong;
    private TextView txt_zhuzhuang;
    private TextView txt_gps;
    private fg_zhuk zhuk;//主控显示界面
    private fg_zhuz zhuz;//驻装显示界面
    FragmentManager fgManager;

    ListView list;
    ArrayList<String> listFiles = new ArrayList<>();//列表只显示文件名
    ArrayList<String> files = new ArrayList<>();//files包含文件完整路径，用来创建文件对象
    ArrayAdapter adapter;//驻装适配器

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        requestWindowFeature(Window.FEATURE_NO_TITLE);//去掉系统的标题栏，使用自定的标题栏
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
        System.out.println("onCreateOptionsMenu");
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
        layoutInit();
        barInit();
        checkPermissions();
        gpsInit();
    }

    private void layoutInit() {
        txt_zhukong = findViewById(R.id.txt_zhukong);
        txt_zhuzhuang = findViewById(R.id.txt_zhuzhuang);

        //Fragmen相关初始化
        zhuz = new fg_zhuz();
        zhuk = new fg_zhuk();
        fgManager = getSupportFragmentManager();
        FragmentTransaction fragmentTransaction = fgManager.beginTransaction();
        fragmentTransaction.add(R.id.frame, zhuz);
        fragmentTransaction.add(R.id.frame, zhuk);//添加fragment到frame
        fragmentTransaction.show(zhuk);//显示主控界面
        fragmentTransaction.hide(zhuz);//隐藏驻装界面
        fragmentTransaction.commit();

        //文件列表显示初始化
        list = findViewById(R.id.list);
        adapter = new ArrayAdapter(MainActivity.this, android.R.layout.simple_list_item_1, listFiles);
        listFiles("record");
        list.setAdapter(adapter);
    }


    public void barInit() {

        Toolbar toolbar = findViewById(R.id.toolbar);
//        setSupportActionBar(toolbar);右上角选项支持
        drawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawerLayout, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawerLayout.addDrawerListener(toggle);
        toggle.syncState();
        NavigationView navigationView = findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);

        Switch sw = navigationView.findViewById(R.id.ble);

      /*  sw.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                System.out.println(b);
            }
        });*/
    }

    public void gpsInit() {
        txt_gps = findViewById(R.id.gps);
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
            sb.append("经度:" + String.format("%03.9f", location.getLongitude()) + "\n");
            sb.append("纬度:" + String.format("%03.9f", location.getLatitude()));
            txt_gps.setText(sb.toString());
        } else {
            txt_gps.setText("经度117.13542156\n纬度031.83975458");
        }
    }

    public boolean isGPSable(LocationManager lm) {
        return lm.isProviderEnabled(LocationManager.GPS_PROVIDER) ? true : false;
    }

    //复位标题栏选中
    private void resetTitle(FragmentTransaction transaction) {
        txt_zhuzhuang.setSelected(false);
        txt_zhukong.setSelected(false);
        txt_zhuzhuang.setTextSize(30);
        txt_zhukong.setTextSize(30);
        transaction.hide(zhuz);
        transaction.hide(zhuk);
    }

    //标题栏点击事件
    public void titleSelect(View view) {
        FragmentTransaction transaction = fgManager.beginTransaction();
        resetTitle(transaction);
        switch (view.getId()) {
            case R.id.txt_zhukong:
                txt_zhukong.setSelected(true);
                txt_zhukong.setTextSize(35);
                transaction.show(zhuk);
                listFiles("record");
                Collections.sort(listFiles);//升序进行排序
                Collections.sort(files);
                adapter.notifyDataSetChanged();//更新列表
                break;
            case R.id.txt_zhuzhuang:
                txt_zhuzhuang.setSelected(true);
                txt_zhuzhuang.setTextSize(35);
                transaction.show(zhuz);
                listFiles("disturb");
                Collections.sort(files);
                Collections.sort(listFiles);//升序进行排序
                adapter.notifyDataSetChanged();//更新列表
                break;
        }
        transaction.commit();

    }


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


    //遍历sd卡sdPath目录文件
    private   void listFiles(String sdPath){
        listFiles.clear();
        files.clear();
        File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/zhuzh/" +sdPath);
         file.list(new FilenameFilter() {
            @Override
            public boolean accept(File file, String s) {
                File temp = new File(file.toString() + "/" +s);

                if (temp.isDirectory())
                    return false;
                else {
                    listFiles.add(s);
                    files.add(temp.toString());
                    return true;
                }
            }
        });
    }
}
