package com.example.runoob;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.SystemClock;
import android.view.View;
import android.widget.Chronometer;
import android.widget.Toast;

public class clock extends AppCompatActivity implements Chronometer.OnChronometerTickListener {
    Chronometer chronometer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_clock);
        chronometer = findViewById(R.id.chronometer);
        chronometer.setOnChronometerTickListener(this);
    }

    public void click(View view) {
        switch (view.getId()) {
            case R.id.start:
                chronometer.start();
                break;
            case R.id.stop:
                chronometer.stop();
                break;
            case R.id.reset:
                chronometer.setBase(SystemClock.elapsedRealtime());
                break;
            case R.id.format:
                chronometer.setFormat("Time:%s");
                break;
        }
    }

    @Override
    public void onChronometerTick(Chronometer chronometer) {
        System.out.println("onChronometerTick");
        String time = chronometer.getText().toString();
        if (time.equals("00:10")) {
            Toast.makeText(clock.this, "时间到了~", Toast.LENGTH_SHORT).show();
        }

    }
}
