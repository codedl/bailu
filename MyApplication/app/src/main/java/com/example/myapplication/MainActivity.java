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
import android.app.Activity;
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
import android.opengl.Visibility;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.SystemClock;
import android.util.ArrayMap;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.provider.Settings;
import android.view.Window;
import android.view.WindowManager;
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

import com.google.android.material.internal.NavigationMenu;
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

public class MainActivity extends AppCompatActivity implements NavigationView.OnNavigationItemSelectedListener,
        CompoundButton.OnCheckedChangeListener, AdapterView.OnItemClickListener, AdapterView.OnItemLongClickListener {
    private String tag = "MainActivity";
    private DrawerLayout drawerLayout;//左侧菜单栏
    private gps gps;//定位
    private TextView txt_zhukong;
    private TextView txt_zhuzhuang;
    private fg_zhuk zhuk;//主控显示界面
    private fg_zhuz zhuz;//驻装显示界面
    private Switch bind_Switch;
    private boolean isZhuk = true;
    private long backTime = 0;//记录按返回键的时间
    private FragmentManager fgManager;

    private ListView list;
    private ArrayList<String> listFiles = new ArrayList<>();//列表只显示文件名
    private ArrayList<String> files = new ArrayList<>();//files包含文件完整路径，用来创建文件对象
    private ArrayAdapter adapter;//适配器，list显示用

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        requestWindowFeature(Window.FEATURE_NO_TITLE);//去掉系统的标题栏，使用自定的标题栏
        //全屏显示应用
//        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        init();
    }

    //接收startActivityForResult的结果
    @Override
    protected void onActivityResult(int request, int result, Intent intent) {
        Log.d("MainActivity", "onActivityResult");
        if (result == Activity.RESULT_OK && request == 200) {
            Log.d("MainActivity", intent.getData().getPath());
        }
    }

    //创建设置菜单
    @Override
    public void onBackPressed() {
        DrawerLayout drawer = findViewById(R.id.drawer_layout);
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            drawer.closeDrawer(GravityCompat.START);//关闭drawer
        } else {
            //两秒内按两次back键退出应用
            if (SystemClock.elapsedRealtime() - backTime > 2000) {
                Toast.makeText(MainActivity.this, "再按一次退出", Toast.LENGTH_SHORT).show();
                backTime = SystemClock.elapsedRealtime();
            } else {
                fileManager.savexml();//保存参数
                super.onBackPressed();
            }
        }

    }

    //创建右上角设置功能
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        Log.d("MainActivity", "onCreateOptionsMenu");
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

    public void init() {
        layoutInit();
        gps = new gps(MainActivity.this, MainActivity.this);
        checkPermissions();
        param.init(MainActivity.this);
        fileManager.readxml();//从xml文件中读取保存的配置参数
        fileManager.savexml();//保存参数
        barInit();
        fileManager.copyFile(MainActivity.this);

    }

    private void layoutInit() {
        //标题栏控制fragment切换
        txt_zhukong = findViewById(R.id.txt_zhukong);
        txt_zhuzhuang = findViewById(R.id.txt_zhuzhuang);

        //Fragment相关初始化
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
        list.setOnItemClickListener(this);
        list.setOnItemLongClickListener(this);
        adapter = new ArrayAdapter(MainActivity.this, android.R.layout.simple_list_item_single_choice, listFiles);
        listFiles("record");
        Collections.sort(listFiles);
        Collections.sort(files);
        list.setAdapter(adapter);
    }

    //菜单栏
    public void barInit() {

        Toolbar toolbar = findViewById(R.id.toolbar);
//        setSupportActionBar(toolbar);//右上角选项支持
        drawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawerLayout, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawerLayout.addDrawerListener(toggle);
        toggle.syncState();
        NavigationView navigationView = findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);
        Menu menu = navigationView.getMenu();
        bind_Switch = (Switch) menu.findItem(R.id.set_binding).getActionView();
        bind_Switch.setOnCheckedChangeListener(this);//点击switch调用onCheckedChanged

    }

    //菜单项目点击事件
    @SuppressWarnings("StatementWithEmptyBody")
    @Override
    public boolean onNavigationItemSelected(MenuItem item) {
        int id = item.getItemId();
        final dialog dialog = new dialog(MainActivity.this);
        View view = null;
        switch (id) {
            case R.id.set_zhuk:
                view = this.getLayoutInflater().inflate(R.layout.menu_zhuk, null);
                final EditText ip = view.findViewById(R.id.edit_ip);
                ip.setText(param.zhuk_ip);
                final EditText port = view.findViewById(R.id.edit_port);
                port.setText(param.zhuk_port);
                final EditText server = view.findViewById(R.id.edit_server);
                server.setText(param.zhuk_server);

                dialog.create("主控连接设置", view);
                dialog.setButtonListener(AlertDialog.BUTTON_POSITIVE, new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        param.zhuk_ip = ip.getText().toString().trim();
                        param.zhuk_port = port.getText().toString().trim();
                        param.zhuk_server = server.getText().toString().trim();
                        if (param.zhuk_ip.isEmpty() && param.zhuk_port.isEmpty() && param.zhuk_server.isEmpty())
                            Toast.makeText(MainActivity.this, "输入不能为空", Toast.LENGTH_SHORT).show();
                        else dialog.dismiss();
                    }
                });
                break;
            case R.id.set_zhuz:
                final set_dialog set = new set_dialog(MainActivity.this);
                view = set.getView();
                dialog.create("驻装终端连接设置", view);
                dialog.setButtonListener(AlertDialog.BUTTON_POSITIVE, new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        if (set.BUTTON_POSITIVE_Event() == true) {
                            dialog.dismiss();
                        } else
                            Toast.makeText(MainActivity.this, "需要选择连接设备", Toast.LENGTH_SHORT).show();
                    }
                });
                dialog.setButtonListener(AlertDialog.BUTTON_NEGATIVE, new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        set.BUTTON_NEGATIVE_Event();
                        dialog.dismiss();
                    }
                });

                break;
            case R.id.set_app:

                break;
            case R.id.set_binding:

                break;
        }
        return true;
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
                isZhuk = true;
                txt_zhukong.setSelected(true);
                txt_zhukong.setTextSize(35);
                transaction.show(zhuk);
                listFiles("record");
                break;
            case R.id.txt_zhuzhuang:
                isZhuk = false;
                txt_zhuzhuang.setSelected(true);
                txt_zhuzhuang.setTextSize(35);
                transaction.show(zhuz);
                listFiles("disturb");
                break;
        }
        Collections.sort(listFiles);//升序进行排序
        Collections.sort(files);
        adapter.notifyDataSetChanged();//更新列表
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
    private void listFiles(String sdPath) {
        listFiles.clear();
        files.clear();
        File file = new File(param.path + sdPath);
        file.list(new FilenameFilter() {
            @Override
            public boolean accept(File file, String s) {
                File temp = new File(file.toString() + "/" + s);
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

    @Override
    public void onCheckedChanged(CompoundButton compoundButton, boolean checked) {

        if (checked == true) {
            //检查是否设置了蓝牙连接对象
            if ((param.btPhone != null || param.bleData != null)) {
                param.bind = true;//只要设置蓝牙耳机和数据终端其中之一就可以连接设备
            } else {
                Toast.makeText(MainActivity.this, "设置驻装连接", Toast.LENGTH_SHORT).show();
                bind_Switch.setChecked(false);
            }
        } else {
            param.bind = false;
        }

        fileManager.savexml();
    }

    //list点击事件
    @Override
    public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
        if (isZhuk) {
            param.file_record = files.get(i);
        } else {
            param.file_disturb = files.get(i);
        }
    }

    //list长按事件
    @Override
    public boolean onItemLongClick(final AdapterView<?> adapterView, View view, final int i, long l) {
        final dialog dialog = new dialog(MainActivity.this);
        TextView txt = new TextView(MainActivity.this);
        txt.setText(fileManager.fileInfo(files.get(i)));
        dialog.create("是否删除此文件", txt);
        dialog.setButtonListener(AlertDialog.BUTTON_POSITIVE, new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                File file = new File(files.get(i));
                if (file.exists())
                    file.delete();
                listFiles.remove(i);
                files.remove(i);
                adapter.notifyDataSetChanged();
                dialog.dismiss();
            }
        });
        return true;//true表示事件已经处理，不会触发短按事件了
    }
}
