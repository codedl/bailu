package com.example.myapplication;

import android.bluetooth.BluetoothA2dp;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaRecorder;
import android.os.Build;

import androidx.annotation.RequiresApi;

import java.io.File;
import java.lang.reflect.Method;


public class audio {
    Context context;
    BluetoothA2dp a2dp;
    MediaRecorder mediaRecorder;//录音使用的工具
    AudioManager audioManager;

    public audio(Context context) {
        this.context = context;
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        adapter.getProfileProxy(context, new BluetoothProfile.ServiceListener() {
            @Override
            public void onServiceConnected(int i, BluetoothProfile bluetoothProfile) {
                if (i == BluetoothA2dp.A2DP) {
                    a2dp = (BluetoothA2dp) bluetoothProfile;
                    System.out.println("a2dp get succeed");
                }
            }

            @Override
            public void onServiceDisconnected(int i) {
                a2dp = null;
                System.out.println("a2dp put");
            }
        }, BluetoothA2dp.A2DP);
        audioManager = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
    }

    public void connect(BluetoothDevice device) {
        if (device == null) {
            return;
        }
        if (a2dp == null) {
            return;
        }
        try {
            Method method = a2dp.getClass().getMethod("connect", BluetoothDevice.class);
            method.setAccessible(true);
            method.invoke(a2dp, device);

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("method exception");
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.O)
    public void startRecord(File file) {
        mediaRecorder = new MediaRecorder();
        mediaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
        mediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.AAC_ADTS);
        mediaRecorder.setOutputFile(file);
        mediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);
        try {
            mediaRecorder.prepare();
            mediaRecorder.start();
            System.out.println("start audio record");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void stopRecord() {
        if (mediaRecorder != null) {
            mediaRecorder.stop();
            mediaRecorder.release();
            mediaRecorder = null;
        }
    }

    public void audioPlay(String file) {
       /*  //设置系统音量
       int maxVol = audioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        System.out.println("maxvol:" + maxVol);
        maxVol *= 0.8;
        audioManager.setStreamVolume(AudioManager.STREAM_MUSIC, maxVol, 0);*/
        MediaPlayer mediaPlayer = new MediaPlayer();
        mediaPlayer.reset();
        try {
            mediaPlayer.setDataSource(file);
            mediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
                @Override
                public void onCompletion(MediaPlayer mediaPlayer) {
                    mediaPlayer.stop();
                    mediaPlayer.release();
                }
            });
            mediaPlayer.setOnErrorListener(new MediaPlayer.OnErrorListener() {
                @Override
                public boolean onError(MediaPlayer mediaPlayer, int i, int i1) {
                    System.out.println("play error");
                    return false;
                }
            });
            mediaPlayer.prepare();
            mediaPlayer.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void startBluetoothSco() {
        audioManager.stopBluetoothSco();
        audioManager.startBluetoothSco();
        context.registerReceiver(new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                int state = intent.getIntExtra(AudioManager.EXTRA_SCO_AUDIO_STATE, -1);
                if (AudioManager.SCO_AUDIO_STATE_CONNECTED == state) {
                    audioManager.setBluetoothScoOn(true);
                    audioManager.setMode(AudioManager.STREAM_MUSIC);
                    context.unregisterReceiver(this);
                    System.out.println("sco connected");
                } else {
                    try {
                        Thread.sleep(1000);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    audioManager.startBluetoothSco();
                    System.out.println("prepare sco connect");
                }
            }
        }, new IntentFilter(AudioManager.ACTION_SCO_AUDIO_STATE_UPDATED));
    }
}
