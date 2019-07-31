package com.example.myapp;

import android.content.Context;
import android.os.Environment;
import android.widget.Toast;

import java.io.FileInputStream;
import java.io.FileOutputStream;

public class file {
    private Context context;

    public file() {

    }

    public file(Context context) {
        super();
        this.context = context;
    }

    public void sdwrite(String file, String content) throws Exception {
        if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
            file = Environment.getExternalStorageDirectory().getCanonicalPath() + "/" + file;
            FileOutputStream os = new FileOutputStream(file);
            os.write(content.getBytes());
            os.close();
        } else Toast.makeText(context, "不支持sd操作", Toast.LENGTH_SHORT).show();
    }

    public void phwrite(String file, String content) throws Exception {
        FileOutputStream os = context.openFileOutput(file, Context.MODE_PRIVATE | context.MODE_APPEND);
        os.write(content.getBytes());
        os.close();
    }

    public String phread(String file) throws Exception {
        FileInputStream is = context.openFileInput(file);
        byte temp[] = new byte[1024];
        StringBuilder sb = new StringBuilder("");
        int len = 0;
        while ((len = is.read(temp)) > 0) {
            sb.append(new String(temp, 0, len));
        }
        is.close();
        return sb.toString();
    }

}
