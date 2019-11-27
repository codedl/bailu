package com.example.application;

import androidx.appcompat.app.AppCompatActivity;

import android.os.AsyncTask;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

import java.util.Timer;
import java.util.TimerTask;

public class ch7 extends AppCompatActivity {
    TextView textView;
    ProgressBar bar1;
    ProgressBar bar2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ch7);
        new myAsyncTask().execute((String) null);
        Button button = findViewById(R.id.get);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                bar1.setVisibility(View.VISIBLE);
                bar2.setVisibility(View.VISIBLE);
                textView.setVisibility(View.VISIBLE);
                new asyncTask().execute((String) null);
            }
        });
        bar1 = findViewById(R.id.bar1);
        bar2 = findViewById(R.id.bar2);
        textView = findViewById(R.id.show);
        bar1.setMax(100);
        bar1.setVisibility(View.INVISIBLE);
        bar2.setVisibility(View.INVISIBLE);
        textView.setVisibility(View.INVISIBLE);

        TimerTask task = new TimerTask() {
            @Override
            public void run() {
                Thread t = new Thread() {
                    @Override
                    public void run() {
                        TextView textView = findViewById(R.id.text);
                        textView.setText("runOnUiThread()");
                    }

                };
                runOnUiThread(t);
            }
        };

        Timer timer = new Timer();
        timer.schedule(task, 5000);
    }

    class myAsyncTask extends AsyncTask<String, Integer, String> {

        @Override
        protected String doInBackground(String... strings) {
            try {
                Thread.sleep(3000);
                publishProgress(50);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            return null;
        }

        @Override
        public void onProgressUpdate(Integer... values) {
            TextView textView = findViewById(R.id.text);
            textView.setText("value:" + values[0]);
        }
    }

    class asyncTask extends AsyncTask<String, Integer, String> {

        @Override
        protected String doInBackground(String... strings) {
            for (int i = 0; i <= 100; i++) {
                try {
                    Thread.sleep(100);
                    publishProgress(i);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            return null;
        }

        @Override
        public void onProgressUpdate(Integer... values) {
            textView.setText(values[0] + "%");
            bar1.setProgress(values[0]);
            if (values[0] == 100)
                bar2.setVisibility(View.INVISIBLE);
        }
    }
}
