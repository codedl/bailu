package com.example.myapplication;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import cz.msebera.android.httpclient.Header;
import it.sauronsoftware.ftp4j.FTPClient;

import android.bluetooth.BluetoothA2dp;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.session.MediaSession;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.webkit.WebView;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.view.View;

import com.loopj.android.http.AsyncHttpClient;
import com.loopj.android.http.AsyncHttpResponseHandler;
import com.loopj.android.http.RequestParams;

import java.io.File;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

public class btActivity extends AppCompatActivity {
    private boolean classic = false;
    public BluetoothAdapter BA;
    public BluetoothDevice btDevice;
    bluetoothReceiver btReceiver;

    private EditText urlEdit;

    public ArrayList<BluetoothDevice> devices = new ArrayList<>();
    public ArrayList<String> deviceName = new ArrayList<>();
    ListView listView;
    ArrayAdapter adapter;
    private MediaSession mediaSession;
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
        urlEdit = findViewById(R.id.url_edit);
        listView = (ListView) findViewById(R.id.list);
        adapter = new ArrayAdapter(btActivity.this, android.R.layout.simple_expandable_list_item_1, deviceName);

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
                if (classic == true) {
                    btDevice = devices.get(i);
                    System.out.println(btDevice.getName() + btDevice.getAddress());
                    devices.clear();
                    bluetoothClassic.cancelDiscovery();
                    bluetoothClassic.createBond(btDevice);
                    bluetoothClassic.connect(btDevice);
                } else {
                    BluetoothDevice bluetoothDevice = blec.bleDevices.get(i);
                    blec.bleConnect(bluetoothDevice);
                    System.out.println(bluetoothDevice.getName() + bluetoothDevice.getAddress());
                    blec.stopScan();
                    blec.bleDevices.clear();
                }
                deviceName.clear();
                listView.setAdapter(adapter);
                listView.setVisibility(View.INVISIBLE);
            }
        });
        mediaSession = new MediaSession(this, "tag");
        mediaSession.setCallback(new MediaSession.Callback() {
            @Override
            public boolean onMediaButtonEvent(Intent mediaButtonIntent) {
                System.out.println(mediaButtonIntent.getAction());
                KeyEvent keyEvent = (KeyEvent) mediaButtonIntent.getParcelableExtra(Intent.EXTRA_KEY_EVENT);
                System.out.println(keyEvent);
                return super.onMediaButtonEvent(mediaButtonIntent);
            }
        });
        mediaSession.setFlags(MediaSession.FLAG_HANDLES_MEDIA_BUTTONS | MediaSession.FLAG_HANDLES_TRANSPORT_CONTROLS);
        mediaSession.setActive(true);
    }


    public class bluetoothReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String name, addr;

            String action = intent.getAction();
            System.out.println(action);
            btDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

            switch (action) {
                case Action.BLE_SCAN_FOUND:

                    String str = intent.getStringExtra("devicestr");
                    System.out.println(str);
                    deviceName.add(str);
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
                    deviceName.add(name + ":" + addr);
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
                            audio.connect(device);
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
                            audio.startBluetoothSco();
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
        String txtfile = Environment.getExternalStorageDirectory().getAbsolutePath() + "/java.txt";
        String jpgfile = Environment.getExternalStorageDirectory().getAbsolutePath() + "/123.jpg";
        String mp3file = Environment.getExternalStorageDirectory().getAbsolutePath() + "/record.mp3";
        switch (btn.getId()) {
            case R.id.start_btn:
                audio.startRecord(new File(mp3file));
                break;

            case R.id.stop_btn:
                audio.stopRecord();
                break;

            case R.id.btSearch_btn:
                bluetoothClassic.startDiscovery();
                classic = true;
                break;
            case R.id.play_btn:
                audio.audioPlay(mp3file);
                break;
            case R.id.send_btn:

                break;
            case R.id.rcv_btn:

                break;

            case R.id.up_btn:

                try {

                    AsyncHttpClient client = new AsyncHttpClient();
                    client.setMaxRetriesAndTimeout(3,10000);
                    client.setUserAgent(fileManager.getAgent());
                    System.out.println(fileManager.getAgent());
                    FileOutputStream output = new FileOutputStream(txtfile);
                    PrintStream print = new PrintStream(output);
                    SimpleDateFormat simpleDateFormat = new SimpleDateFormat("yyyy年MM月dd日 HH:mm:ss");// HH:mm:ss
                    Date date = new Date(System.currentTimeMillis());//获取当前时间
                    print.println(date);

                    File uploadFile = new File(txtfile);
//                    String path = "http://192.168.2.172/";
//                    String path = "ftp://record:record@192.168.2.172/";
                    String path = urlEdit.getText().toString().trim();
                    System.out.println(path);
                    System.out.println(txtfile);
                    RequestParams params = new RequestParams();
//                    client.setProxy("192.168.42.62", 80);
                    params.put(uploadFile.getName(), txtfile);
                    /*client.get(path, new AsyncHttpResponseHandler() {
                        @Override
                        public void onSuccess(int i, Header[] headers, byte[] bytes) {
                            System.out.println(new String(bytes,0,bytes.length));
                        }

                        @Override
                        public void onFailure(int i, Header[] headers, byte[] bytes, Throwable throwable) {

                        }
                    });*/
                    client.post(path, params, new AsyncHttpResponseHandler() {

                        @Override
                        public void onSuccess(int statusCode, Header[] headers, byte[] responseBody) {
                            System.out.println("succeed !");
                        }

                        @Override
                        public void onFailure(int statusCode, Header[] headers, byte[] responseBody, Throwable error) {
                            System.out.println("failed !");

                        }
                    });

                } catch (Exception e) {
                    e.printStackTrace();
                }
                /*new Thread(){
                    @Override
                    public void run(){
                        OkHttpClient client = new OkHttpClient();
                        File ufile = new File(file);
                        RequestBody request = new MultipartBody.Builder()
                                .setType(MultipartBody.FORM)
                                .addFormDataPart("username","dingle")
                                .addFormDataPart("mp3",ufile.getName(),RequestBody.create(MediaType.parse("multipart/form-data"),file))
                                .build();
                        Request req = new Request.Builder()
                                .header("Authorization", "Client-ID " + UUID.randomUUID())
                                .url("http://192.168.2.172")
                                .post(request)
                                .build();
                        try {
                            Response response = client.newCall(req).execute();
                            if(response.isSuccessful()){
                                System.out.println("file succeed");
                            }
                        }catch (Exception e){
                            e.printStackTrace();
                        }
                    }
                }.start();*/

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
        unregisterReceiver(btReceiver);
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


}
