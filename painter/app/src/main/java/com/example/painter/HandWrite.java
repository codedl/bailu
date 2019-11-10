package com.example.painter;

import android.content.Context;
import android.graphics.*;
import android.graphics.Paint.Style;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Environment;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.text.SimpleDateFormat;
import java.util.Date;

import static android.content.Context.SENSOR_SERVICE;

public class HandWrite extends View implements SensorEventListener {
    boolean isDone = false;
    Paint paint = null;
    Bitmap originalBitmap = null;
    Bitmap new1_Bitmap = null;
    Bitmap new2_Bitmap = null;
    float startX = 0, startY = 0;
    float clickX = 0, clickY = 0;
    boolean isMove = true;
    boolean isClear = false;
    int color = Color.BLUE;
    float strokeWidth = 10.0f;
    float angle = 0 ;
    String path;
    private SensorManager sManager;
    private Sensor mSensorOrientation;
    int width ,height;
    public HandWrite(Context context, AttributeSet attrs) {
        super(context, attrs);
        originalBitmap = BitmapFactory
                .decodeResource(getResources(), R.drawable.pbg).copy(Bitmap.Config.ARGB_8888, true);
        new1_Bitmap = Bitmap.createBitmap(originalBitmap);
        width = new1_Bitmap.getWidth();
        height = new1_Bitmap.getHeight();
        color = Color.RED;

        File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/painter/");
        if (!file.exists())
            file.mkdir();
        path = file.getAbsolutePath() + "/";
        System.out.println("path:" + path);

        sManager = (SensorManager) context.getSystemService(SENSOR_SERVICE);
        mSensorOrientation = sManager.getDefaultSensor(Sensor.TYPE_ORIENTATION);
        sManager.registerListener(this, mSensorOrientation, SensorManager.SENSOR_DELAY_UI);
    }

    public void done() {
        isDone = true;
        new Thread() {
            @Override
            public void run() {
                try {
                    File file = new File(path + currentTime() + ".png");
                    FileOutputStream fos = new FileOutputStream(file);
                    new1_Bitmap.compress(Bitmap.CompressFormat.PNG, 100, fos);
                    fos.flush();
                    fos.close();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }.start();
        invalidate();
    }

    public void clear() {
        isClear = true;
        new1_Bitmap = Bitmap.createBitmap(originalBitmap);
        invalidate();//让view进行刷新重新绘制
    }

    public void red() {
        isClear = false;
        color = Color.RED;
    }

    public void blue() {
        isClear = false;
        color = Color.BLUE;
    }

    public void brush() {
        strokeWidth = 20.0f;
    }

    public void eraser() {
        color = Color.WHITE;
        strokeWidth = 80.0f;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        canvas.drawBitmap(HandWriting(new1_Bitmap), 0, 0, null);
    }

    public Bitmap HandWriting(Bitmap o_Bitmap) {
        if(isDone)
            return o_Bitmap;
        Canvas canvas = new Canvas(o_Bitmap);
        paint = new Paint();
        paint.setStyle(Style.STROKE);
        paint.setAntiAlias(true);
        paint.setColor(color);
        paint.setStrokeWidth(strokeWidth);
        if (isMove) {
            canvas.drawLine(startX, startY, clickX, clickY, paint);
        }
        startX = clickX;
        startY = clickY;
        return o_Bitmap;
    }


    @Override
    public boolean onTouchEvent(MotionEvent event) {
        clickX = event.getX();
        clickY = event.getY();
        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            isMove = false;
            invalidate();
            return true;
        } else if (event.getAction() == MotionEvent.ACTION_MOVE) {
            isMove = true;
            invalidate();
            return true;
        }
        return super.onTouchEvent(event);
    }

    private String currentTime() {
        SimpleDateFormat format = new SimpleDateFormat("yyyyMMddHHmmss");//20191023180330
        Date date = new Date(System.currentTimeMillis());
        return format.format(date);
    }

    @Override
    public void onSensorChanged(SensorEvent sensorEvent) {
        if(angle == 0){
            angle = (float) (Math.round(sensorEvent.values[0] * 100)) / 100;
            return;
        }
        float scale =  ((float) (Math.round(sensorEvent.values[0] * 100)) / 100 - angle);
        angle = (float) (Math.round(sensorEvent.values[0] * 100)) / 100;
        System.out.println("scale:" + scale);

        System.out.println("方位角：" + (float) (Math.round(sensorEvent.values[0] * 100)) / 100);
        System.out.println("倾斜角：" + (float) (Math.round(sensorEvent.values[1] * 100)) / 100);
        System.out.println("滚动角：" + (float) (Math.round(sensorEvent.values[2] * 100)) / 100);
        if(isDone) {
            if(scale > 0.5)
                new1_Bitmap = Bitmap.createScaledBitmap(
                        new1_Bitmap,(int)(new1_Bitmap.getWidth()*1.01),(int)(new1_Bitmap.getHeight()*1.01),false);
            else if(scale < -1)
                new1_Bitmap = Bitmap.createScaledBitmap(
                        new1_Bitmap,(int)(new1_Bitmap.getWidth()*0.99),(int)(new1_Bitmap.getHeight()*0.99),false);
            invalidate();
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {

    }
}
