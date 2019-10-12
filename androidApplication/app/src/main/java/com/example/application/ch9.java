package com.example.application;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.location.LocationManager;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.provider.Settings;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.MediaController;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.VideoView;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

public class ch9 extends AppCompatActivity {
    private Button StartMusic;
    private Button pauseMusic;
    private Button resetMusic;
    private Button closeMusic;
    private SeekBar volumeBar;
    private SeekBar posBar;
    private TextView curVol;
    private TextView curPos;
    private MediaPlayer mp;
    private static String fileName;
    private static int volume = 0;
    private static int times = 0;
    private AudioManager am;
    private File audioFile;
    private final String VIDEO_PATH = "/storage/emulated/0/video.mp4";
    private MediaPlayer player;
    private SurfaceView surface;
    private SurfaceHolder surfaceHolder;
    private Button play, pause, stop;
    private SeekBar vBar;
    private int vtimes;

    private static final int REQUEST_SUCCESS_CODE = 200;
    private String imagePath;
    private ImageView   image;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ch9);
        checkPermissions();
        StartMusic = findViewById(R.id.StartMusic);
        pauseMusic = findViewById(R.id.pauseMusic);
        resetMusic = findViewById(R.id.resetMusic);
        closeMusic = findViewById(R.id.resetMusic);
        volumeBar = findViewById(R.id.volumeBar);
        posBar = findViewById(R.id.posBar);
        curVol = findViewById(R.id.curVol);
        curPos = findViewById(R.id.curPos);
        StartMusic.setOnClickListener(new AudioListener());
        pauseMusic.setOnClickListener(new AudioListener());
        resetMusic.setOnClickListener(new AudioListener());
        closeMusic.setOnClickListener(new AudioListener());

        mp = new MediaPlayer();

        /*VideoView videoView = findViewById(R.id.videwView);
        MediaController mc = new MediaController(this);
        videoView.setMediaController(mc);
        videoView.setVideoURI(Uri.parse("file://" + Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + "test.mp4"));
        videoView.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(MediaPlayer mediaPlayer) {
                Toast.makeText(ch9.this,"视频播放完了",Toast.LENGTH_SHORT).show();
            }
        });
        videoView.requestFocus();
        videoView.start();*/

        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        play = findViewById(R.id.play);
        pause = findViewById(R.id.pause);
        stop = findViewById(R.id.stop);
        surface = findViewById(R.id.surface);
        vBar = findViewById(R.id.vBar);
        play.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                init();
                mp.reset();
                surfaceHolder = surface.getHolder();
                mp.setDisplay(surfaceHolder);
                try {
                    mp.setDataSource(VIDEO_PATH);
                    mp.prepare();
                } catch (Exception e) {
                    e.printStackTrace();
                }
                mp.start();
            }
        });
        pause.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mp.isPlaying())
                    mp.pause();
                else
                    mp.start();
            }
        });
        stop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mp.isPlaying()) {
                    mp.stop();
                }
            }
        });

        Button b = findViewById(R.id.start);
        image = findViewById(R.id.pic);
        b.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                imagePath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/pic.jpg";
                File imageFile = new File(imagePath);
                Uri uri = Uri.fromFile(imageFile);
                Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
                intent.putExtra(MediaStore.EXTRA_OUTPUT, uri);
                startActivityForResult(intent, REQUEST_SUCCESS_CODE);
            }
        });
    }

    @Override
    protected void onActivityResult(int request, int result, Intent intent) {
        if (request == REQUEST_SUCCESS_CODE) {
            if (result == RESULT_OK) {
                Bitmap bmp = BitmapFactory.decodeFile(imagePath);
                image.setImageBitmap(bmp);
            } else {
                Toast.makeText(this, "放弃拍照", Toast.LENGTH_SHORT).show();
            }
        }
        super.onActivityResult(request, result, intent);
    }

    public void init() {
        try {
            mp.setDataSource(VIDEO_PATH);
            mp.prepare();
        } catch (Exception e) {
            e.printStackTrace();
        }
        if (times == 0)
            times = mp.getDuration();
        vBar.setMax(times);
        vBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                mp.pause();
                mp.seekTo(seekBar.getProgress());
                mp.start();
            }
        });
    }

    @Override
    protected void onDestroy() {
        mp.release();

        super.onDestroy();
    }

    public void stop(View view) {
        if (mp.isPlaying())
            mp.stop();
    }

    private void play() throws IOException {
        if (mp.isPlaying())
            return;
        audioFile = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + "test.wav");
        System.out.println(audioFile);
        mp.reset();
        mp.setDataSource(Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + "test.wav");
        mp.prepare();
        if (times == 0)
            times = mp.getDuration();
        setMusicParams();
        mp.start();
    }

    private void setMusicParams() {
        am = (AudioManager) getSystemService(AUDIO_SERVICE);
        //只设置音频音量
        ch9.this.setVolumeControlStream(AudioManager.STREAM_MUSIC);
        //设置bar的最大值为系统音量最大值
        volumeBar.setMax(am.getStreamMaxVolume(AudioManager.STREAM_MUSIC));
        //获取当前音量
        volume = am.getStreamVolume(AudioManager.STREAM_MUSIC);
        //设置当前音量
        volumeBar.setProgress(volume);
        //显示当前音量
        curVol.setText(String.valueOf(volume));
        volumeBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                curVol.setText(String.valueOf(i));//显示当前音量
                am.setStreamVolume(AudioManager.STREAM_MUSIC, i, AudioManager.FLAG_PLAY_SOUND);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        posBar.setMax(times);
        posBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                mp.pause();//暂停播放
                mp.seekTo(seekBar.getProgress());//跳到指定位置
                mp.start();//开始播放
                curPos.setText(seekBar.getProgress() / 1000 + "s");
            }
        });
    }

    private class AudioListener implements View.OnClickListener {

        @Override
        public void onClick(View view) {
            try {
                switch (view.getId()) {
                    case R.id.StartMusic:
                        play();
                        Timer timer = new Timer();
                        timer.schedule(new TimerTask() {
                            @Override
                            public void run() {
                                posBar.setProgress(mp.getCurrentPosition());
                            }
                        }, 0, 50);
                        break;
                    case R.id.pauseMusic:
                        if (mp.isPlaying()) {
                            mp.pause();
                            pauseMusic.setText("继续");
                        } else {
                            mp.start();
                            pauseMusic.setText("暂停");
                        }
                        break;
                    case R.id.resetMusic:
                        if (mp.isPlaying())
                            mp.seekTo(0);
                        else
                            play();
                        break;
                    case R.id.closeMusic:
                        System.out.println("close");
                        if (mp.isPlaying())
                            mp.stop();
                        break;
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private void checkPermissions() {
        String[] permissions = { Manifest.permission.WRITE_EXTERNAL_STORAGE,
                Manifest.permission.READ_EXTERNAL_STORAGE, };
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

    private boolean checkGPSIsOpen() {
        LocationManager locationManager = (LocationManager) this.getSystemService(Context.LOCATION_SERVICE);
        if (locationManager == null)
            return false;
        return locationManager.isProviderEnabled(android.location.LocationManager.GPS_PROVIDER);
    }
}
