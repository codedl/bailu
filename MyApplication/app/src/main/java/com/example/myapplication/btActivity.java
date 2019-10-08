package com.example.myapplication;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import android.annotation.TargetApi;
import android.bluetooth.BluetoothA2dp;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.media.session.MediaSession;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.provider.MediaStore;
import android.view.KeyEvent;
import android.webkit.WebView;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.view.View;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.SimpleTimeZone;


public class btActivity extends AppCompatActivity {
    private boolean classic = false;
    public BluetoothAdapter BA;
    bluetoothReceiver btReceiver;

    private EditText urlEdit;
    public ArrayList<BluetoothDevice> devices = new ArrayList<>();
    public ArrayList<String> itemNames = new ArrayList<>();
    public ArrayList<File> upFiles = new ArrayList<>();
    ListView listView;
    ArrayAdapter adapter;
    private MediaSession mediaSession;
    AudioManager audioManager;

    private BluetoothLowEnergy blec;
    private BluetoothClassic bluetoothClassic;
    byte val[] = new byte[]{0};
    audio audio;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bt);
        btInit();

    }

    @Override
    public boolean onKeyDown(int keycode, KeyEvent event) {
        super.onKeyDown(keycode, event);
        System.out.println("keycode:" + keycode);
        return true;
    }


    public void btInit() {
        blec = new BluetoothLowEnergy(btActivity.this);
        bluetoothClassic = new BluetoothClassic(btActivity.this);
        audio = new audio(btActivity.this);
        audioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        audioManager.registerMediaButtonEventReceiver(new ComponentName(this, MyReceiver.class));
        urlEdit = findViewById(R.id.url_edit);
        listView = (ListView) findViewById(R.id.list);
        adapter = new ArrayAdapter(btActivity.this, android.R.layout.simple_expandable_list_item_1, itemNames);

        BA = BluetoothAdapter.getDefaultAdapter();
        if (!BA.isEnabled()) {
            Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(turnOn, 1);
        }

        IntentFilter intentFilter = new IntentFilter(BluetoothDevice.ACTION_FOUND);//注册广播接收信号
        intentFilter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        intentFilter.addAction(BluetoothDevice.ACTION_PAIRING_REQUEST);
        intentFilter.addAction(BluetoothA2dp.ACTION_CONNECTION_STATE_CHANGED);
        intentFilter.addAction(BluetoothA2dp.ACTION_PLAYING_STATE_CHANGED);
        intentFilter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        intentFilter.addAction(Action.BLE_SCAN_FOUND);

        btReceiver = new bluetoothReceiver();
        registerReceiver(btReceiver, intentFilter);//用BroadcastReceiver 来取得结果

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                if (upFiles.get(i) != null)
                    new fileManager().upload(urlEdit.getText().toString().trim(), upFiles.get(i).toString());

                if (classic == true) {
                    if (devices.size() == 0)
                        return;
                    BluetoothDevice btDevice = devices.get(i);
                    if (btDevice == null)
                        return;
                    System.out.println(btDevice.getName() + btDevice.getAddress());
                    devices.clear();
                    bluetoothClassic.cancelDiscovery();
                    bluetoothClassic.createBond(btDevice);
                    bluetoothClassic.connect(btDevice);
                } else {
                    if (blec.bleDevices.size() == 0)
                        return;
                    BluetoothDevice bluetoothDevice = blec.bleDevices.get(i);
                    if (bluetoothDevice == null)
                        return;
                    blec.bleConnect(bluetoothDevice);
                    System.out.println(bluetoothDevice.getName() + bluetoothDevice.getAddress());
                    blec.stopScan();
                    blec.bleDevices.clear();
                }
                itemNames.clear();
                listView.setAdapter(adapter);
                listView.setVisibility(View.INVISIBLE);
            }
        });

        listView.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            @Override
            public boolean onItemLongClick(AdapterView<?> adapterView, View view, int i, long l) {
                File file = upFiles.get(i);

                AlertDialog.Builder builder = new AlertDialog.Builder(btActivity.this);
                builder.setTitle("文件属性");
                builder.setMessage(fileManager.fileInfo(file));
                builder.create().show();

                return true;//返回true则不会触发短按事件

            }
        });

    }

    class audioListener implements AudioManager.OnAudioFocusChangeListener {

        @Override
        public void onAudioFocusChange(int i) {
            System.out.println("onAudioFocusChange:" + i);
        }
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private void setMediaButtonEvent() {
        mediaSession = new MediaSession(btActivity.this, "tag");
        //        mediaSession.setFlags(MediaSession.FLAG_HANDLES_MEDIA_BUTTONS | MediaSession.FLAG_HANDLES_TRANSPORT_CONTROLS);
        mediaSession.setActive(true);
        mediaSession.setCallback(new MediaSession.Callback() {
            @Override
            public boolean onMediaButtonEvent(Intent mediaButtonIntent) {
                System.out.println("onMediaButtonEvent");
                System.out.println(mediaButtonIntent.getAction());
                KeyEvent keyEvent = (KeyEvent) mediaButtonIntent.getParcelableExtra(Intent.EXTRA_KEY_EVENT);
                System.out.println(keyEvent);
                return super.onMediaButtonEvent(mediaButtonIntent);
            }

            @Override
            public void onPlay() {
                super.onPlay();
                System.out.println("onplay");
            }

            @Override
            public void onPause() {
                super.onPause();
                System.out.println("onPause");
            }
        });


    }

    public class bluetoothReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String name, addr;

            String action = intent.getAction();
            System.out.println(action);
            BluetoothDevice btDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

            switch (action) {
                case Action.BLE_SCAN_FOUND:

                    String str = intent.getStringExtra("devicestr");
                    System.out.println(str);
                    itemNames.add(str);
                    listView.setAdapter(adapter);
                    listView.setVisibility(View.VISIBLE);
                    findViewById(R.id.web).setVisibility(View.INVISIBLE);
                    adapter.notifyDataSetChanged();//更新
                    break;
                case BluetoothDevice.ACTION_FOUND:
                    name = btDevice.getName();
                    addr = btDevice.getAddress();
                    System.out.println(name + addr);
                    if (devices.contains(btDevice))
                        return;
                    itemNames.add(name + ":" + addr);
                    devices.add(btDevice);
                    listView.setAdapter(adapter);
                    listView.setVisibility(View.VISIBLE);
                    findViewById(R.id.web).setVisibility(View.INVISIBLE);
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

                case BluetoothA2dp.ACTION_CONNECTION_STATE_CHANGED:
                    switch (intent.getIntExtra(BluetoothA2dp.EXTRA_STATE, -1)) {
                        case BluetoothA2dp.STATE_CONNECTED:
                            System.out.println("BluetoothA2dp.STATE_CONNECTED");
                            break;
                        case BluetoothA2dp.STATE_CONNECTING:
                            System.out.println("BluetoothA2dp.STATE_CONNECTING");
//                           建立蓝牙sco连接
//                            audio.startBluetoothSco();
                            break;
                        case BluetoothA2dp.STATE_DISCONNECTING:
                            System.out.println("BluetoothA2dp.STATE_DISCONNECTING");
                            break;
                        case BluetoothA2dp.STATE_DISCONNECTED:
                            System.out.println("BluetoothA2dp.STATE_DISCONNECTED");
                            break;

                    }
                    break;

                case BluetoothA2dp.ACTION_PLAYING_STATE_CHANGED:
                    switch (intent.getIntExtra(BluetoothA2dp.EXTRA_STATE, -1)) {
                        case BluetoothA2dp.STATE_PLAYING:
                            System.out.println("BluetoothA2dp.STATE_PLAYING");
                            break;
                        case BluetoothA2dp.STATE_NOT_PLAYING:
                            System.out.println("BluetoothA2dp.STATE_NOT_PLAYING");
                            break;
                    }
                    break;
            }
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.O)
    public void btEvent(View btn) {
        String sdPath = Environment.getExternalStorageDirectory().getAbsolutePath();
        String txtfile = sdPath + "/http.txt";
        String jpgfile = sdPath + "/123.jpg";
        final String pcmfile = sdPath + "/ddhgdw.pcm";
        final String wavfile = sdPath + "/ddhgdw.wav";
        switch (btn.getId()) {
            case R.id.stopscan_btn:
                bluetoothClassic.cancelDiscovery();
                blec.stopScan();
                listView.setVisibility(View.INVISIBLE);
                break;

            case R.id.start_btn:
                audio.startAudioRecord(pcmfile);
                break;

            case R.id.stop_btn:
                audio.stopAudioRecord();
                audio.pcmToWav(pcmfile, wavfile);
                break;

            case R.id.btSearch_btn:
                bluetoothClassic.startDiscovery();
                classic = true;
                break;
            case R.id.play_btn:
                audio.mediaPlay(wavfile);
                break;
            case R.id.send_btn:

                break;
            case R.id.rcv_btn:

                break;

            case R.id.up_btn:
                itemNames.clear();
                File file = new File(sdPath);
                String[] fileList = file.list(new FilenameFilter() {
                    @Override
                    public boolean accept(File file, String s) {
                        File list = new File(file.getAbsolutePath() + "/" + s);

                        if (list.isDirectory())
                            return false;
                        else {
                            itemNames.add(s);
                            upFiles.add(list);
                            return true;
                        }
                    }
                });
                listView.setVisibility(View.VISIBLE);
                listView.setAdapter(adapter);
                break;
            case R.id.web_btn:
                blec.stopScan();
                bluetoothClassic.cancelDiscovery();
                listView.setVisibility(View.INVISIBLE);
                WebView webView = findViewById(R.id.web);
                webView.setVisibility(View.VISIBLE);
                try {
                    URL url = new URL(urlEdit.getText().toString().trim());
                    webView.loadUrl(urlEdit.getText().toString().trim());
                } catch (Exception e) {
                    e.printStackTrace();
                }
                break;
            case R.id.blescan_btn:
                blec.startScan();
                classic = false;
                break;
            case R.id.red_btn:
                val[0] ^= 1;
                blec.bleWrite(blec.red, val);
                break;
            case R.id.green_btn:
                val[0] ^= 1;
                blec.bleWrite(blec.green, val);
                break;
        }
    }


    protected void onDestroy() {
        super.onDestroy();//解除注册
        // unregisterReceiver(btReceiver);
    }

    class handle extends Handler {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case 1:
                    System.out.println("handle message 1");
                    break;
            }
        }
    }

    public void pcmToWav(String inFilename, String outFilename) {
        System.out.println("pcm to wav");
        FileInputStream in;
        FileOutputStream out;
        long totalAudioLen;
        long totalDataLen;
        long longSampleRate = 16000;
        int channels = 1;
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
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


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
