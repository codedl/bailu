package com.example.app;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.Adapter;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Random;

public class MainActivity extends AppCompatActivity {
    private Spinner freqway_intwaySpin;
    private ArrayList<String> freqway_intwayStr = new ArrayList<>();
    private ArrayAdapter freqway_intwayAdapt;
    private Spinner comModeSpin;
    private ArrayList<String> comModeStr = new ArrayList<>();
    private ArrayAdapter comModeAdapt;
    private Spinner freqrange_intbandSpin;
    private ArrayList<String> freqrange_intbandStr = new ArrayList<>();
    private ArrayAdapter freqrange_intbandAdapt;
    private Spinner freqspeedSpin;
    private ArrayList<String> freqspeedStr = new ArrayList<>();
    private ArrayAdapter freqspeedAdapt;
    private Spinner demodwaySpin;
    private ArrayList<String> demodwayStr = new ArrayList<>();
    private ArrayAdapter demodwayAdapt;
    private Spinner demodsourceSpin;
    private ArrayList<String> demodsourceStr = new ArrayList<>();
    private ArrayAdapter demodsourceAdapt;
    private Spinner msg_noiseSpin;
    private ArrayList<String> msg_noiseStr = new ArrayList<>();
    private ArrayAdapter msg_noiseAdapt;

    private TextView freqrange_intbandText;
    private TextView freqspeedText;
    private EditText freqEdit;
    private EditText msgEdit;
    private TextView msgText;
    private EditText powerEdit;
    private Button freqlistBtn;
    private TextView freqway_intwayText;
    private TextView msg_noiseText;

    private double randomArray[];

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        init();
    }

    void init() {
        layoutInit();
        comModeInit();
    }

    void comModeInit() {
        freqway_intwayText.setText("频率方式");
        freqrange_intbandText.setText("跳频范围");
        freqspeedText.setText("跳频速率");
        msg_noiseText.setText("报文:");

        freqrange_intbandText.setTextColor(Color.parseColor("#8A000000"));
        freqrange_intbandSpin.setEnabled(false);
        freqrange_intbandStr.clear();
        freqrange_intbandStr.add("200KHz");
        freqrange_intbandSpin.setAdapter(freqrange_intbandAdapt);
        freqrange_intbandSpin.setSelection(0, true);
        freqrange_intbandSpin.setOnItemSelectedListener(comFreqRangeSpinListener);

        freqspeedText.setTextColor(Color.parseColor("#8A000000"));
        freqspeedSpin.setEnabled(false);
        freqspeedStr.clear();
        freqspeedStr.add("20跳/秒");
        freqspeedSpin.setAdapter(freqspeedAdapt);

        freqway_intwaySpin.setOnItemSelectedListener(null);
        freqway_intwayStr.clear();
        freqway_intwayStr.add("定频");
        freqway_intwayStr.add("跳频");
        freqway_intwaySpin.setAdapter(freqway_intwayAdapt);
        freqway_intwaySpin.setSelection(0, true);
        freqway_intwaySpin.setOnItemSelectedListener(comFreqwaySpinListener);

        demodsourceStr.clear();
        demodsourceStr.add("语音报文");
        demodsourceStr.add("手咪语音");
        demodsourceSpin.setAdapter(demodsourceAdapt);

        msg_noiseStr.clear();
        msg_noiseStr.add("报文1");
        msg_noiseStr.add("报文2");
        msg_noiseStr.add("报文3");
        msg_noiseSpin.setVisibility(View.VISIBLE);
        msg_noiseSpin.setAdapter(msg_noiseAdapt);
        msgEdit.setVisibility(View.INVISIBLE);
        msgText.setVisibility(View.INVISIBLE);

        demodwaySpin.setOnItemSelectedListener(comDemodWaySpinListener);
    }

    void layoutInit() {
        msg_noiseText = findViewById(R.id.noise_msg_text);
        freqway_intwayText = findViewById(R.id.intway_freqway_text);
        freqrange_intbandText = findViewById(R.id.intband_freqrange_text);
        freqspeedText = findViewById(R.id.freqspeed_text);
        freqEdit = findViewById(R.id.freq_edit);
        powerEdit = findViewById(R.id.power_edit);
        freqlistBtn = findViewById(R.id.speed_btn);
        msg_noiseSpin = findViewById(R.id.noise_msgcontent_spin);
        msg_noiseAdapt = new ArrayAdapter(this, android.R.layout.simple_list_item_1, msg_noiseStr);
        demodsourceSpin = findViewById(R.id.demodsource_spin);
        demodsourceAdapt = new ArrayAdapter(this, android.R.layout.simple_list_item_1, demodsourceStr);
        demodwaySpin = findViewById(R.id.demodway_spin);
        demodwayAdapt = new ArrayAdapter(this, android.R.layout.simple_list_item_1, demodwayStr);
        freqway_intwaySpin = findViewById(R.id.intway_freqway_spin);
        freqway_intwayAdapt = new ArrayAdapter(this, android.R.layout.simple_list_item_1, freqway_intwayStr);
        comModeSpin = findViewById(R.id.mode_spin);
        comModeAdapt = new ArrayAdapter(this, android.R.layout.simple_list_item_1, comModeStr);
        freqspeedSpin = findViewById(R.id.freqspeed_spin);
        freqspeedAdapt = new ArrayAdapter(this, android.R.layout.simple_list_item_1, freqspeedStr);
        freqrange_intbandSpin = findViewById(R.id.intband_freqrange_spin);
        freqrange_intbandAdapt = new ArrayAdapter(this, android.R.layout.simple_list_item_1, freqrange_intbandStr);
        msgEdit = findViewById(R.id.noise_msgcontent_edit);
        msgText = findViewById(R.id.noise_msgcontent_text);

        comModeStr.add("通信模式");
        comModeStr.add("干扰模式");
        comModeSpin.setAdapter(comModeAdapt);
        comModeSpin.setSelection(0, true);
        comModeSpin.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                switch (i) {
                    case 0:
                        comModeInit();
                        break;
                    case 1:
                        intModeInit();
                        break;
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        });

        demodwayStr.add("AM");
        demodwayStr.add("USB");
        demodwayStr.add("LSB");
        demodwayStr.add("DSB");
        demodwayStr.add("CW");
        demodwayStr.add("FSK");
        demodwayStr.add("FM");
        demodwayStr.add("多音FSK");
        demodwaySpin.setAdapter(demodwayAdapt);
        demodwaySpin.setSelection(0, true);
        demodwaySpin.setOnItemSelectedListener(comDemodWaySpinListener);

    }

    void intModeInit() {
        freqway_intwayText.setText("干扰样式");
        freqway_intwaySpin.setOnItemSelectedListener(null);
        freqway_intwayStr.clear();
        freqway_intwayStr.add("瞄准式");
        freqway_intwayStr.add("扫频式");
        freqway_intwaySpin.setAdapter(freqway_intwayAdapt);
        freqway_intwaySpin.setSelection(0, true);
        freqway_intwaySpin.setOnItemSelectedListener(intWaySpinListener);

        freqrange_intbandText.setText("干扰带宽");
        freqrange_intbandText.setTextColor(Color.parseColor("#4CAF50"));
        freqrange_intbandStr.clear();
        freqrange_intbandStr.add("5KHz");
        freqrange_intbandStr.add("10KHz");
        freqrange_intbandStr.add("25KHz");
        freqrange_intbandSpin.setAdapter(freqrange_intbandAdapt);
        freqrange_intbandSpin.setEnabled(true);
        freqrange_intbandSpin.setOnItemSelectedListener(null);

        freqspeedText.setText("扫频速率");
        freqspeedText.setTextColor(Color.parseColor("#8A000000"));
        freqspeedStr.clear();
        freqspeedStr.add("1KHz");
        freqspeedSpin.setAdapter(freqspeedAdapt);
        freqspeedSpin.setEnabled(false);

        demodsourceStr.clear();
        demodsourceStr.add("噪声");
        demodsourceSpin.setAdapter(demodsourceAdapt);

        msg_noiseText.setText("噪声:");
        msgText.setText("高斯白噪声");
        msgText.setVisibility(View.VISIBLE);
        msg_noiseSpin.setVisibility(View.INVISIBLE);
        msgEdit.setVisibility(View.INVISIBLE);

        demodwaySpin.setOnItemSelectedListener(null);
    }

    //按钮监听事件
    public void clickEvent(View view) {
        switch (view.getId()) {
            case R.id.speed_btn:
                View freqLayout = getLayoutInflater().inflate(R.layout.freqlist, null);
                ListView freqList = freqLayout.findViewById(R.id.freq_list);
                ArrayList<String> freqStrList = new ArrayList<>();
                ArrayAdapter freqListAdapt = new ArrayAdapter(MainActivity.this, android.R.layout.simple_list_item_1, freqStrList);
                String str = "";
                for (int i = 0; i < randomArray.length; i++) {
                    str = "         ";
                    str += (String.format("%03d", (i + 1)));
                    str += "              ";
                    str += (String.format("%4.6f", randomArray[i]));
                    str += "MHz";
                    freqStrList.add(str);
                }
                freqList.setAdapter(freqListAdapt);
                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                builder.setView(freqLayout)
                        .setTitle("图案序号         频率列表")
                        .setNegativeButton("退出", null)
                        .create().show();
                break;
            case R.id.send_btn:
                System.out.println("id:" + comModeSpin.getSelectedItemId());
                System.out.println("postion:" + comModeSpin.getSelectedItemPosition());

                break;
        }
    }

    //判断是否点击了editview
    public boolean isEditView(View view, MotionEvent motionEvent) {
        if (view != null && view instanceof EditText) {
            int leftTop[] = {0, 0};
            view.getLocationInWindow(leftTop);
            int left = leftTop[0];
            int top = leftTop[1];
            int right = leftTop[0] + view.getWidth();
            int bottom = leftTop[1] + view.getHeight();
            if (motionEvent.getX() > left && motionEvent.getX() < right && motionEvent.getY() > top && motionEvent.getY() < bottom) {
                return true;
            } else {
                return false;
            }
        }
        return false;
    }

    //为editview设置边框
    @Override
    public boolean dispatchTouchEvent(MotionEvent motionEvent) {
        if (motionEvent.getAction() == MotionEvent.ACTION_DOWN) {
            View view = getCurrentFocus();
            EditText edit = (EditText) view;
            if (isEditView(view, motionEvent)) {
                edit.requestFocus();
                edit.setCursorVisible(true);
                edit.setBackgroundResource(R.drawable.edit);
            } else {
                edit.setCursorVisible(false);
                edit.setBackground(null);
                edit.clearFocus();
                InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                if (imm != null) {
                    imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
                }
                if (edit.equals(freqEdit)) {
                    if (freqEdit.getText().toString().length() != 0) {
                        param.freq = Double.parseDouble(freqEdit.getText().toString());
                    } else {
                        debug("输入不能为空", false);
                    }
                    if (param.freq < 3) {
                        param.freq = 3;
                        debug("频率最小:3MHz", false);
                    } else if (param.freq > 1000) {
                        param.freq = 1000;
                        debug("频率最大:1000MHz", false);
                    }
                    freqEdit.setText(String.format("%4.6f", param.freq));
                }

                if (edit.equals(powerEdit)) {
                    if (powerEdit.getText().toString().length() != 0) {
                        param.power = Double.parseDouble(powerEdit.getText().toString());
                    } else {
                        debug("输入不能为空", false);
                    }
                    if (param.power < Math.pow(1,-3)) {
                        param.power = Math.pow(1,-3);
                        debug("功率最小:1mW", false);
                    } else if (param.power > 10) {
                        param.power = 10;
                        debug("功率最大:10W", false);
                    }
                    powerEdit.setText(String.format("%1.3f", param.power));
                }
            }
            return super.dispatchTouchEvent(motionEvent);
        }
        if (getWindow().superDispatchTouchEvent(motionEvent)) {
            return true;
        }
        return onTouchEvent(motionEvent);
    }

    public void debug(String str, boolean print) {
        if (print) {
            System.out.println(str);
        } else {
            Toast.makeText(MainActivity.this, str, Toast.LENGTH_SHORT).show();
        }
    }

    Spinner.OnItemSelectedListener intWaySpinListener = new Spinner.OnItemSelectedListener() {

        @Override
        public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
            switch (i) {
                case 0:
                    freqrange_intbandStr.clear();
                    freqrange_intbandStr.add("5KHz");
                    freqrange_intbandStr.add("10KHz");
                    freqrange_intbandStr.add("25KHz");
                    freqrange_intbandSpin.setAdapter(freqrange_intbandAdapt);

                    freqspeedText.setTextColor(Color.parseColor("#8A000000"));
                    freqspeedSpin.setEnabled(false);
                    break;
                case 1:
                    freqrange_intbandStr.clear();
                    freqrange_intbandStr.add("200KHz");
                    freqrange_intbandStr.add("500KHz");
                    freqrange_intbandStr.add("1MHz");
                    freqrange_intbandStr.add("2MHz");
                    freqrange_intbandStr.add("5MHz");
                    freqrange_intbandStr.add("10MHz");
                    freqrange_intbandStr.add("20MHz");
                    freqrange_intbandSpin.setAdapter(freqrange_intbandAdapt);

                    freqspeedText.setTextColor(Color.parseColor("#4CAF50"));
                    freqspeedStr.clear();
                    freqspeedStr.add("10Hz");
                    freqspeedStr.add("100Hz");
                    freqspeedStr.add("1KHz");
                    freqspeedStr.add("5KHz");
                    freqspeedStr.add("10KHz");
                    freqspeedStr.add("50KHz");
                    freqspeedStr.add("100KHz");
                    freqspeedStr.add("1MHz");
                    freqspeedSpin.setAdapter(freqspeedAdapt);
                    freqspeedSpin.setEnabled(true);

                    break;
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> adapterView) {

        }
    };
    Spinner.OnItemSelectedListener comFreqRangeSpinListener = new Spinner.OnItemSelectedListener() {

        @Override
        public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
            String str = null;
            int length = 0;
            int index = 0;
            int randnum = 0;
            double freq = Double.parseDouble(freqEdit.getText().toString().trim());

            if (freqrange_intbandStr.get(i).contains("KHz")) {
                str = freqrange_intbandStr.get(i).replace("KHz", "");
                randnum = Integer.parseInt(str) / 4 * 2; // 上下限
                freq -= Double.parseDouble(str) / 1000;
            } else {
                str = freqrange_intbandStr.get(i).replace("MHz", "");
                randnum = Integer.parseInt(str) * 1000 / 4 * 2;
                freq -= Double.parseDouble(str);
            }
            if (freq < 0) {
                freq = 0;
            }
            length = randnum;
            if (randnum > 100) {
                length = 100;
            }
            int a[] = new int[length];
            int rand = 0;
            Random random = new Random();
            while (index < length - 1) {
                rand = random.nextInt(randnum);
                while (isInArray(rand, a)) {
                    rand = random.nextInt(randnum);
                }
                a[index++] = rand;
            }

            rand = random.nextInt(length);
            a[length - 1] = a[rand];
            a[rand] = 0;

            randomArray = new double[length];
            for (int randomIndex = 0; randomIndex < length; randomIndex++) {
                randomArray[randomIndex] = freq + (double) a[randomIndex] * 4 / 1000;
                System.out.println(randomArray[randomIndex]);
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> adapterView) {

        }
    };

    boolean isInArray(int a, int b[]) {
        for (int i = 0; i < b.length; i++) {
            if (b[i] == a) {
                return true;
            }
        }
        return false;
    }

    Spinner.OnItemSelectedListener comFreqwaySpinListener = new Spinner.OnItemSelectedListener() {
        @Override
        public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {

            switch (i) {
                case 0:
                    freqrange_intbandStr.clear();
                    freqrange_intbandStr.add("200KHz");
                    freqrange_intbandSpin.setAdapter(freqrange_intbandAdapt);
                    freqrange_intbandSpin.setEnabled(false);
                    freqrange_intbandText.setTextColor(Color.parseColor("#8A000000"));

                    freqspeedStr.clear();
                    freqspeedStr.add("20跳/秒");
                    freqspeedSpin.setAdapter(freqspeedAdapt);
                    freqspeedSpin.setEnabled(false);
                    freqspeedText.setTextColor(Color.parseColor("#8A000000"));

                    freqlistBtn.setVisibility(View.INVISIBLE);

                    break;
                case 1:
                    freqlistBtn.setVisibility(View.VISIBLE);
                    freqrange_intbandSpin.setEnabled(true);
                    freqrange_intbandText.setTextColor(Color.parseColor("#4CAF50"));
                    freqspeedText.setTextColor(Color.parseColor("#4CAF50"));
                    freqspeedSpin.setEnabled(true);
                    freqspeedStr.clear();
                    freqrange_intbandStr.clear();

                    freqrange_intbandStr.add("200KHz");
                    freqrange_intbandStr.add("500KHz");
                    freqrange_intbandStr.add("1MHz");
                    freqrange_intbandStr.add("2MHz");
                    freqrange_intbandStr.add("5MHz");
                    freqrange_intbandStr.add("10MHz");
                    freqrange_intbandStr.add("20MHz");
                    freqrange_intbandStr.add("27MHz");
                    freqrange_intbandStr.add("80MHz");
                    if (param.freq >= 3 && param.freq <= 30) {
                        freqrange_intbandStr.remove("80MHz");
                    } else if (param.freq >= 110 && param.freq <= 1000) {
                        freqrange_intbandStr.remove("27MHz");
                        freqrange_intbandStr.remove("80MHz");
                    }
                    freqrange_intbandSpin.setAdapter(freqrange_intbandAdapt);

                    freqspeedStr.add("20跳/秒");
                    freqspeedStr.add("50跳/秒");
                    freqspeedStr.add("100跳/秒");
                    freqspeedStr.add("500跳/秒");
                    freqspeedStr.add("1000跳/秒");
                    freqspeedSpin.setAdapter(freqspeedAdapt);
                    break;
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> adapterView) {

        }
    };

    Spinner.OnItemSelectedListener comDemodWaySpinListener = new Spinner.OnItemSelectedListener() {
        @Override
        public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
            switch (i) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 6:
                    msg_noiseSpin.setVisibility(View.VISIBLE);
                    msgEdit.setVisibility(View.INVISIBLE);
                    msgText.setVisibility(View.INVISIBLE);
                    break;
                case 4:
                    msg_noiseSpin.setVisibility(View.INVISIBLE);
                    msgEdit.setVisibility(View.VISIBLE);
                    msgText.setVisibility(View.INVISIBLE);
                    break;
                case 5:
                case 7:
                    msg_noiseSpin.setVisibility(View.INVISIBLE);
                    msgEdit.setVisibility(View.INVISIBLE);
                    msgText.setVisibility(View.VISIBLE);
                    break;
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> adapterView) {

        }

    };
}
