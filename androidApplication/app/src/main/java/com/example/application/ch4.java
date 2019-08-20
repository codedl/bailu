package com.example.application;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.view.KeyEventDispatcher;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

public class ch4 extends AppCompatActivity {
    int times = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ch4);
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
                Toast.makeText(ch4.this,"you clicked"+i,Toast.LENGTH_SHORT).show();
            }
        });

        CheckBox wife = findViewById(R.id.wife);
        wife.setOnCheckedChangeListener(listener);
        CheckBox girl = findViewById(R.id.girlfriend);
        girl.setOnCheckedChangeListener(listener);
        CheckBox love = findViewById(R.id.lover);
        love.setOnCheckedChangeListener(listener);
    }

    public CompoundButton.OnCheckedChangeListener listener = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
            if(b){
                Toast.makeText(ch4.this,"you clicked"+compoundButton.getText(),Toast.LENGTH_SHORT).show();
            }
        }
    };
}
