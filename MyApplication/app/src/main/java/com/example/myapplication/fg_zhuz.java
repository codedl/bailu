package com.example.myapplication;

import android.bluetooth.BluetoothDevice;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import androidx.appcompat.app.ActionBarDrawerToggle;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
import androidx.drawerlayout.widget.DrawerLayout;
import androidx.fragment.app.Fragment;

public class fg_zhuz extends Fragment implements View.OnClickListener {
    private String tag = "fg_zhuz";
    private ToggleButton connect;
    private ToggleButton record;
    private ToggleButton disturb;
    private ToggleButton sample;
    private TextView txt_progress;
    private TextView txt_data;
    private TextView txt_phone;
    private TextView txt_disturb;
    private ProgressBar progress;
    private boolean isBle;
    private boolean isBt;
    private audio audio;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup group, Bundle bundle) {
        View view = inflater.inflate(R.layout.zhuz, group, false);
        connect = view.findViewById(R.id.connect);
        connect.setOnClickListener(this);
        record = view.findViewById(R.id.record);
        record.setOnClickListener(this);
        disturb = view.findViewById(R.id.disturb);
        disturb.setOnClickListener(this);
        sample = view.findViewById(R.id.sample);
        sample.setOnClickListener(this);
        txt_progress = view.findViewById(R.id.txt);
        progress = view.findViewById(R.id.progress);
        txt_data = getActivity().findViewById(R.id.data);
        txt_phone = getActivity().findViewById(R.id.phone);
        txt_disturb = getActivity().findViewById(R.id.disturb);
        audio = new audio(getActivity());

        return view;
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            //连接蓝牙
            case R.id.connect:
                if (connect.isChecked()) {
                    //断开连接
                    if (param.bleData != null)
                        param.ble.disconnect();
                    if (param.btPhone != null)
                        param.btc.disconnect();
                    //从保存已连接的设备列表中删除设备
                    if (param.btc.getBondedDevices() != null)
                        for (BluetoothDevice device : param.btc.getBondedDevices()) {
                            BluetoothClassic.removeBond(device);
                        }

                    progress.setVisibility(View.INVISIBLE);
                    txt_progress.setVisibility(View.INVISIBLE);
                    txt_phone.setText("耳机:未连接");
                    txt_data.setText("数据:未连接");

                } else {
                    if (param.bind) {//是否绑定连接设备
                        //是否连接设备
                        if (param.bleData != null) {//是否设置了连接的设备
                            //是否已经连接设备，如果已经连接检查已经连接的设备与将要连接的设备是否相同
                            if (param.ble.connectDevice != null &&
                                    param.ble.connectDevice.getAddress().equals(param.bleData.getAddress()))
                                isBle = false;
                            else {
                                param.ble.bleConnect(param.bleData);
                                isBle = true;
                            }

                        }
                        if (param.btPhone != null) {//连接参数是否设置
                            //已经连接设备，但是和将要连接的设备不相同时
                            if (param.btc.getBondedDevices() != null &&
                                    param.btc.isConnected(param.btPhone))
                                isBt = false;
                            else {
                                param.btc.connect(param.btPhone);
                                isBt = true;
                            }
                        }
                        new asyncProgress().execute();
                        progress.setVisibility(View.VISIBLE);
                        txt_progress.setVisibility(View.VISIBLE);
                        progress.setMax(100);
                        progress.setProgress(0);
                    } else {
                        connect.setChecked(true);
                        //弹出抽屉菜单
                        DrawerLayout drawerLayout = getActivity().findViewById(R.id.drawer_layout);
                        drawerLayout.openDrawer(Gravity.LEFT);
                        Toast.makeText(getActivity(), "需要绑定设备", Toast.LENGTH_SHORT).show();
                    }
                }
                break;
            case R.id.record:
                if (record.isChecked()) {
                    audio.stopAudioRecord();
                } else {
                    Log.d(tag, "record");
                    audio.startAudioRecord(param.pathRecord + ClsUtils.currentTime());
                }
                break;
            case R.id.disturb:
                if (disturb.isChecked()) {
                    if (!param.file_disturb.isEmpty()) {
                        audio.mediaStop();
                        txt_disturb.setText("干扰状态:没有干扰");
                    }
                } else {
                    if (!param.file_disturb.isEmpty()) {
                        audio.mediaPlay(param.file_disturb);
                        txt_disturb.setText("干扰状态:手动施加干扰");
                    }
                }
                break;
            case R.id.sample:
                Log.d(tag, "sample");
                break;
        }

    }

    class asyncProgress extends AsyncTask<String, Integer, String> {
        @Override
        protected String doInBackground(String... strings) {
            try {
                int seconds = 0;
                //显示连接socket的进度
                while (isBle) {
                    Thread.sleep(100);
                    publishProgress(BluetoothLowEnergy.progress);//100ms检查一次连接的状态
                }
                while (isBt) {
                    Thread.sleep(1000);
                    seconds++;
                    if (BluetoothClassic.progress == 20)
                        //当连接阻塞在connect时，手动改变进度条显示
                        publishProgress(BluetoothClassic.progress + seconds);
                    else
                        //1000ms检查一次连接的状态，耳机连接的时间比较长
                        publishProgress(BluetoothClassic.progress);
                }

            } catch (Exception e) {
                e.printStackTrace();
            }
            return "";
        }

        //显示连接蓝牙的进度
        @Override
        public void onProgressUpdate(Integer... values) {
            //显示连接ble的进度
            if (isBle) {
                progress.setProgress(values[0]);
                txt_progress.setText(param.bleData.getName() + param.bleData.getAddress() + "(" + values[0] + "%)");
                if (values[0] == 100) {//连接完成
                    isBle = false;
                    txt_data.setText("数据:已连接");
                }
            }

            if (isBt) {
                progress.setProgress(values[0]);
                txt_progress.setText(param.btPhone.getName() + param.btPhone.getAddress() + "(" + values[0] + "%)");
                if (values[0] == 100) {//连接完成
                    isBt = false;
                    txt_phone.setText("耳机:已连接");
                }
            }
            //其中之一连接完成就不显示进度条
            if (isBt == false && isBle == false) {
                progress.setProgress(0);
                progress.setVisibility(View.INVISIBLE);
                txt_progress.setVisibility(View.INVISIBLE);
            }

        }
    }
}
