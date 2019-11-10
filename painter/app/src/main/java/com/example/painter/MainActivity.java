package com.example.painter;

import android.Manifest;
import android.app.Activity;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import androidx.appcompat.app.AlertDialog;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends Activity {
    private HandWrite handWrite;
    private ImageView iv,back;
    private Button clear, done;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        checkPermissions();
        handWrite = findViewById(R.id.handwrite);
        handWrite.setVisibility(View.INVISIBLE);
        handWrite.isDone = false;
        iv = findViewById(R.id.image);
        back = findViewById(R.id.back);
        iv.setVisibility(View.VISIBLE);
        clear = findViewById(R.id.clear);
        done = findViewById(R.id.done);
    }


    public void click(View view) {
        switch (view.getId()) {
            case R.id.done:
                handWrite.done();
                clear.setVisibility(View.INVISIBLE);
                iv.setVisibility(View.INVISIBLE);
                done.setVisibility(View.INVISIBLE);
                back.setVisibility(View.VISIBLE);
                break;
            case R.id.clear:
                handWrite.clear();
                break;
            case R.id.image:
                handWrite.setVisibility(View.VISIBLE);
                clear.setVisibility(View.VISIBLE);
                done.setVisibility(View.VISIBLE);
                break;
            case R.id.back:
                handWrite.isDone = false;
                back.setVisibility(View.INVISIBLE);
                clear.setVisibility(View.VISIBLE);
                done.setVisibility(View.VISIBLE);
                break;
        }
    }

    private void checkPermissions() {
        String[] permissions = {Manifest.permission.WRITE_EXTERNAL_STORAGE,
                Manifest.permission.READ_EXTERNAL_STORAGE,};
        List<String> permissionDeniedList = new ArrayList<>();
        for (String permission : permissions) {
            int permissionCheck = ContextCompat.checkSelfPermission(this, permission);
            if (permissionCheck == PackageManager.PERMISSION_GRANTED) {
//                onPermissionGranted(permission);
            } else {
                permissionDeniedList.add(permission);
            }
        }
        if (!permissionDeniedList.isEmpty()) {
            String[] deniedPermissions = permissionDeniedList.toArray(new String[permissionDeniedList.size()]);
            ActivityCompat.requestPermissions(this, deniedPermissions, 1);
        }
    }


}
