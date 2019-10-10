package com.example.tab;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;

import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.FrameLayout;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    private TextView txt_topbar;
    private TextView txt_channel;
    private TextView txt_message;
    private TextView txt_better;
    private TextView txt_setting;
    private FrameLayout ly_content;

    private MyFragment fg1, fg2, fg3, fg4;
    private FragmentManager fManager;

    @Override

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_main);
        fManager = getSupportFragmentManager();
        bindView();
        txt_channel.performClick();
    }

    private void bindView() {
        txt_topbar = (TextView) findViewById(R.id.txt_topbar);
        txt_channel = findViewById(R.id.txt_channel);
        txt_better = findViewById(R.id.txt_better);
        txt_message = findViewById(R.id.txt_message);
        txt_setting = findViewById(R.id.txt_setting);

        txt_channel.setOnClickListener(this);
        txt_better.setOnClickListener(this);
        txt_message.setOnClickListener(this);
        txt_setting.setOnClickListener(this);
    }

    private void setSelected() {
        txt_channel.setSelected(false);
        txt_better.setSelected(false);
        txt_message.setSelected(false);
        txt_setting.setSelected(false);
    }

    private void hide(FragmentTransaction fragmentTransaction) {
        if (fg1 != null) fragmentTransaction.hide(fg1);
        if (fg2 != null) fragmentTransaction.hide(fg2);
        if (fg3 != null) fragmentTransaction.hide(fg3);
        if (fg4 != null) fragmentTransaction.hide(fg4);
    }

    @Override
    public void onClick(View view) {
        FragmentTransaction fragmentTransaction = fManager.beginTransaction();
        hide(fragmentTransaction);
        switch (view.getId()) {
            case R.id.txt_channel:
                setSelected();
                txt_channel.setSelected(true);
                if (fg1 == null) {
                    fg1 = new MyFragment("first fragment");
                    fragmentTransaction.add(R.id.ly_content, fg1);
                } else {
                    fragmentTransaction.show(fg1);
                }
                break;
            case R.id.txt_message:
                setSelected();
                txt_message.setSelected(true);
                if (fg2 == null) {
                    fg2 = new MyFragment("第二个Fragment");
                    fragmentTransaction.add(R.id.ly_content, fg2);
                } else {
                    fragmentTransaction.show(fg2);
                }
                break;
            case R.id.txt_better:
                setSelected();
                txt_better.setSelected(true);
                if (fg3 == null) {
                    fg3 = new MyFragment("第三个Fragment");
                    fragmentTransaction.add(R.id.ly_content, fg3);
                } else {
                    fragmentTransaction.show(fg3);
                }
                break;
            case R.id.txt_setting:
                setSelected();
                txt_setting.setSelected(true);
                if (fg4 == null) {
                    fg4 = new MyFragment("第四个Fragment");
                    fragmentTransaction.add(R.id.ly_content, fg4);
                } else {
                    fragmentTransaction.show(fg4);
                }
                break;
        }
        fragmentTransaction.commit();
    }
}
