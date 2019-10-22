package com.example.myapplication;

import android.bluetooth.BluetoothA2dp;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.MediaPlayer;
import android.media.MediaRecorder;
import android.os.Build;
import android.util.Log;

import androidx.annotation.RequiresApi;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.lang.reflect.Method;
import java.text.SimpleDateFormat;
import java.util.Date;


public class audio {
    private String tag = "audio";
    private Context context;
    private BluetoothA2dp a2dp;
    private MediaRecorder mediaRecorder;//录音使用的工具
    private AudioRecord audioRecord;//pcm格式的录音工具
    private boolean isRecord;//是否正在录音
    private AudioManager audioManager;
    private MediaPlayer mediaPlayer;

    public audio(Context context) {
        this.context = context;
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        adapter.getProfileProxy(context, new BluetoothProfile.ServiceListener() {
            @Override
            public void onServiceConnected(int i, BluetoothProfile bluetoothProfile) {
                if (i == BluetoothA2dp.A2DP) {
                    a2dp = (BluetoothA2dp) bluetoothProfile;
                    Log.d("audio", "a2dp get succeed");
                }
            }

            @Override
            public void onServiceDisconnected(int i) {
                a2dp = null;
                Log.d("audio", "a2dp put");
            }
        }, BluetoothA2dp.A2DP);
        audioManager = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
        mediaPlayer = new MediaPlayer();

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
            Log.d("audio", "method exception");
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.O)
    public void startMediaRecorder(File file) {
        mediaRecorder = new MediaRecorder();
        mediaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
        mediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.AAC_ADTS);
        mediaRecorder.setOutputFile(file);
        mediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);
        mediaRecorder.setAudioSamplingRate(16000);//16k
        mediaRecorder.setAudioEncodingBitRate(16);//16bits
        try {
            mediaRecorder.prepare();
            mediaRecorder.start();
            Log.d("audio", "start audio record");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void stopMediaRecorder() {
        if (mediaRecorder != null) {
            mediaRecorder.stop();
            mediaRecorder.release();
            mediaRecorder = null;
        }
    }

    public void mediaPlay(String file) {
       /*  //设置系统音量
       int maxVol = audioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        Log.d("audio","maxvol:" + maxVol);
        maxVol *= 0.8;
        audioManager.setStreamVolume(AudioManager.STREAM_MUSIC, maxVol, 0);*/
        mediaPlayer.reset();
        try {
            mediaPlayer.setDataSource(file);
            mediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
                @Override
                public void onCompletion(MediaPlayer mediaPlayer) {
                    mediaPlayer.stop();
                }
            });
            mediaPlayer.setOnErrorListener(new MediaPlayer.OnErrorListener() {
                @Override
                public boolean onError(MediaPlayer mediaPlayer, int i, int i1) {
                    Log.d("audio", "play error");
                    return false;
                }
            });
            mediaPlayer.prepare();
            mediaPlayer.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void mediaStop() {
        mediaPlayer.stop();
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
                    Log.d("audio", "sco connected");
                } else {
                    try {
                        Thread.sleep(1000);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    audioManager.startBluetoothSco();
                    Log.d("audio", "prepare sco connect");
                }
            }
        }, new IntentFilter(AudioManager.ACTION_SCO_AUDIO_STATE_UPDATED));
    }

    //录音
    public void startAudioRecord(final String path) {
        int hz = 16000;//采样率
        int channel = AudioFormat.CHANNEL_IN_MONO;//单声道
        int bits = AudioFormat.ENCODING_PCM_16BIT;//音频格式16bits
        final int minBufferSize = AudioRecord.getMinBufferSize(hz, channel, bits);
        audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, hz, channel, bits, minBufferSize);
        final byte data[] = new byte[minBufferSize];
        final File file = new File(path);
        if (file.exists())
            file.delete();
        audioRecord.startRecording();
        isRecord = true;
        new Thread(new Runnable() {
            @Override
            public void run() {
                FileOutputStream os = null;
                try {
                    os = new FileOutputStream(file);
                    if (os != null) {
                        while (isRecord) {
                            int read = audioRecord.read(data, 0, minBufferSize);
                            if (read != AudioRecord.ERROR_INVALID_OPERATION) {
                                try {
                                    os.write(data);
                                } catch (Exception e) {
                                    e.printStackTrace();
                                }
                            }
                        }
                        os.close();
                        //将pcm格式的录音文件保存成wav格式的文件
                        pcmToWav(path, path + ".wav");

                        File file = new File(param.path + "record.txt");
                        RandomAccessFile raf = new RandomAccessFile(file, "rw");
                        raf.seek(file.length());
                        Date date = new Date(System.currentTimeMillis());
                        SimpleDateFormat simpleDateFormat = new SimpleDateFormat("yyyy.MM.dd-HH:mm:ss");
                        //保存文件上传日志
                        raf.write(("******" + "Time:" + simpleDateFormat.format(date) + "\r\n").getBytes());
                        raf.write(("File:" + path + ".wav" + "\r\n").getBytes());
                        raf.write(("Length:" + file.length() + "bytes" + "\r\n").getBytes());
                        raf.close();
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    public void stopAudioRecord() {
        isRecord = false;
        if (audioRecord != null) {
            audioRecord.stop();
            audioRecord.release();
            audioRecord = null;
        }
    }

    //pcm转化成wav文件，写wav文件头
    public void pcmToWav(String inFilename, String outFilename) {
        Log.d("audio", "pcm to wav");
        FileInputStream in;
        FileOutputStream out;
        long totalAudioLen;
        long totalDataLen;
        long longSampleRate = 16000;//采样率
        int channels = 1;//单声道
        long byteRate = 16 * 16000 * channels / 8;
        int mBufferSize = AudioRecord.getMinBufferSize(16000, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT);
        byte[] data = new byte[mBufferSize];
        try {
            in = new FileInputStream(inFilename);
            out = new FileOutputStream(outFilename);
            totalAudioLen = in.getChannel().size();
            totalDataLen = totalAudioLen + 36;

            writeWaveFileHeader(out, totalAudioLen, totalDataLen,
                    longSampleRate, channels, byteRate);
            while (in.read(data) != -1) {
                out.write(data);
            }
            in.close();
            out.close();
            File pcm = new File(inFilename);
            if (pcm.exists()) {
                pcm.delete();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    //写wav文件头
    private void writeWaveFileHeader(FileOutputStream out, long totalAudioLen,
                                     long totalDataLen, long longSampleRate, int channels, long byteRate)
            throws IOException {
        byte[] header = new byte[44];
        // RIFF/WAVE header
        header[0] = 'R';
        header[1] = 'I';
        header[2] = 'F';
        header[3] = 'F';
        header[4] = (byte) (totalDataLen & 0xff);
        header[5] = (byte) ((totalDataLen >> 8) & 0xff);
        header[6] = (byte) ((totalDataLen >> 16) & 0xff);
        header[7] = (byte) ((totalDataLen >> 24) & 0xff);
        //WAVE
        header[8] = 'W';
        header[9] = 'A';
        header[10] = 'V';
        header[11] = 'E';
        // 'fmt ' chunk
        header[12] = 'f';
        header[13] = 'm';
        header[14] = 't';
        header[15] = ' ';
        // 4 bytes: size of 'fmt ' chunk
        header[16] = 16;
        header[17] = 0;
        header[18] = 0;
        header[19] = 0;
        // format = 1
        header[20] = 1;
        header[21] = 0;
        header[22] = (byte) channels;
        header[23] = 0;
        header[24] = (byte) (longSampleRate & 0xff);
        header[25] = (byte) ((longSampleRate >> 8) & 0xff);
        header[26] = (byte) ((longSampleRate >> 16) & 0xff);
        header[27] = (byte) ((longSampleRate >> 24) & 0xff);
        header[28] = (byte) (byteRate & 0xff);
        header[29] = (byte) ((byteRate >> 8) & 0xff);
        header[30] = (byte) ((byteRate >> 16) & 0xff);
        header[31] = (byte) ((byteRate >> 24) & 0xff);
        // block align
        header[32] = (byte) (2 * 16 / 8);
        header[33] = 0;
        // bits per sample
        header[34] = 16;
        header[35] = 0;
        //data
        header[36] = 'd';
        header[37] = 'a';
        header[38] = 't';
        header[39] = 'a';
        header[40] = (byte) (totalAudioLen & 0xff);
        header[41] = (byte) ((totalAudioLen >> 8) & 0xff);
        header[42] = (byte) ((totalAudioLen >> 16) & 0xff);
        header[43] = (byte) ((totalAudioLen >> 24) & 0xff);
        out.write(header, 0, 44);
    }


}
