package com.example.application;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.view.KeyEventDispatcher;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

public class ch4 extends AppCompatActivity {
    int times = 0;
    int pvalue = 0;
    ProgressBar pb1;
    ProgressBar pb2;
    Handler handler;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ch4);
        pb1 = findViewById(R.id.pb1);
        pb2 = findViewById(R.id.pb2);
        final ImageView imageView = findViewById(R.id.imageview);
        ImageButton imageButton = findViewById(R.id.imagebutton);
        imageButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                times++;
                if (times == 5)
                    times = 0;
                switch (times) {
                    case 1:
                        imageView.setScaleType(ImageView.ScaleType.CENTER);
                        break;
                    case 2:
                        imageView.setScaleType(ImageView.ScaleType.FIT_CENTER);
                        break;
                    case 3:
                        imageView.setScaleType(ImageView.ScaleType.FIT_XY);
                        break;
                    case 4:
                        imageView.setScaleType(ImageView.ScaleType.MATRIX);
                        break;
                    case 5:
                        imageView.setScaleType(ImageView.ScaleType.CENTER_CROP);
                        break;
                }
            }
        });

        RadioGroup radioGroup = findViewById(R.id.radiogroup);
        radioGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup radioGroup, int i) {
                Toast.makeText(ch4.this, "you clicked" + i, Toast.LENGTH_SHORT).show();
            }
        });

        CheckBox wife = findViewById(R.id.wife);
        wife.setOnCheckedChangeListener(listener);
        CheckBox girl = findViewById(R.id.girlfriend);
        girl.setOnCheckedChangeListener(listener);
        CheckBox love = findViewById(R.id.lover);
        love.setOnCheckedChangeListener(listener);

        handler = new Handler(){
            @Override
            public void handleMessage(Message msg){
                pb2.setProgress(pvalue);
                TextView textView = findViewById(R.id.pro_text);
                textView.setText("downloading...("+msg.arg1+")");
                if(msg.what == 0x222){
                    pb1.setVisibility(View.INVISIBLE);
                    pb2.setVisibility(View.INVISIBLE);
                    textView.setText("download!");
                }
            }
        };
    }

    public CompoundButton.OnCheckedChangeListener listener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
            if (b) {
                Toast.makeText(ch4.this, "you clicked" + compoundButton.getText(), Toast.LENGTH_SHORT).show();
            }
        }
    };

    public void ch4(View view) {
        switch (view.getId()) {
            case R.id.pro_btn:
                pb1.setVisibility(View.VISIBLE);
                pb2.setVisibility(View.VISIBLE);
                pb2.setProgress(0);
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        pvalue = 0;
                        while(true){
                            pvalue += (int)(Math.random()*10);
                            try{
                                Thread.sleep(100);
                            }catch (Exception e){
                                e.printStackTrace();
                            }
                            Message msg = new Message();
                            if(pvalue<100){
                                msg.arg1 = pvalue;
                                msg.what = 0x111;
                                handler.sendMessage(msg);
                            }else {
                                msg.arg1 = 100;
                                msg.what = 0x222;
                                handler.sendMessage(msg);
                                break;
                            }
                        }
                    }
                }).start();
                break;
        }
    }
}
