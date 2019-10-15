package com.example.runoob;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


    }

    public void click(View view) {
        Intent intent = null;
        switch (view.getId()) {
            case R.id.scoll:
                intent = new Intent(MainActivity.this, scroll.class);
                startActivity(intent);
                break;
            case R.id.clock:
                intent = new Intent(MainActivity.this, clock.class);
                startActivity(intent);
                break;
            case R.id.adapt:
                intent = new Intent(MainActivity.this, adapt.class);
                startActivity(intent);
                break;
        }
    }
}
