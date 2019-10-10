package com.example.application;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.Button;

import java.io.InputStream;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
//        requestWindowFeature(Window.FEATURE_NO_TITLE);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button ch4 = findViewById(R.id.ch4_btn);
        ch4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, ch4.class);
                startActivity(intent);
            }
        });

        final Button ch5 = findViewById(R.id.ch5_btn);
        ch5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, ch5.class);
                startActivity(intent);
            }
        });

        final Button ch6 = findViewById(R.id.ch6_btn);
        ch6.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, ch6.class);
                startActivity(intent);
            }
        });

        Button ch7 = findViewById(R.id.ch7_btn);
        ch7.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, ch7.class);
                startActivity(intent);
            }
        });
    }
}
