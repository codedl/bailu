package com.example.painter;


import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class MainActivity extends Activity {
    private HandWrite handWrite = null;
    Button red, blue, clear, brush, eraser;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        handWrite = (HandWrite) findViewById(R.id.handwriteview);
        red = (Button) findViewById(R.id.red);
        blue = (Button) findViewById(R.id.blue);
        clear = (Button) findViewById(R.id.clear);
        brush = (Button) findViewById(R.id.brush);
        eraser = (Button) findViewById(R.id.eraser);
        clear.setOnClickListener(new cClick());
        red.setOnClickListener(new rClick());
        blue.setOnClickListener(new bClick());
        brush.setOnClickListener(new brClick());
        eraser.setOnClickListener(new eClick());

    }

    class cClick implements OnClickListener {
        public void onClick(View v) {
            handWrite.clear();
        }
    }

    class rClick implements OnClickListener {
        public void onClick(View v) {
            handWrite.red();
        }
    }

    class bClick implements OnClickListener {
        public void onClick(View v) {
            handWrite.blue();
        }
    }

    class brClick implements OnClickListener {
        public void onClick(View v) {
            handWrite.brush();
        }
    }

    class eClick implements OnClickListener {
        public void onClick(View v) {
            handWrite.eraser();
        }
    }

}
