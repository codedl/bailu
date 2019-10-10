package com.example.application;

import androidx.appcompat.app.AppCompatActivity;

import android.content.SharedPreferences;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class ch6 extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ch6);

        final EditText key = findViewById(R.id.key);
        final EditText value = findViewById(R.id.value);
        Button save = findViewById(R.id.save);
        Button read = findViewById(R.id.read);
        final SharedPreferences sp = getSharedPreferences("sp_share", MODE_PRIVATE);
        final SharedPreferences.Editor editor = sp.edit();

        save.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String keyEdit = key.getText().toString();
                String valueEdit = value.getText().toString();
                editor.putString(keyEdit, valueEdit);
                editor.commit();
            }
        });

        read.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String k = key.getText().toString();
                String v = sp.getString(k, "NO_Value");
                value.setText(v);
            }
        });

        final EditText id = findViewById(R.id.id);
        final EditText name = findViewById(R.id.name);
        final EditText score = findViewById(R.id.score);
        final EditText list = findViewById(R.id.list);
        Button datasave = findViewById(R.id.datasave);
        Button dataread = findViewById(R.id.dataread);
        final SQLiteDatabase db = SQLiteDatabase.openOrCreateDatabase(this.getFilesDir().toString() + "/stu.db3", null);
        System.out.println(this.getFilesDir().toString() + "/stu.db3");
//        db.execSQL("create table student(id integer, name varchar(50), score integer)");
        datasave.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String i = id.getText().toString();
                String n = name.getText().toString();
                String s = score.getText().toString();
                db.execSQL("insert into student values(?,?,?) ", new String[]{i, n, s});
                Toast.makeText(ch6.this, "save succeed", Toast.LENGTH_SHORT).show();
            }
        });

        dataread.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                list.setText("");
                Cursor cursor = db.rawQuery("select * from student", null);
                while (true) {
                    if (cursor.moveToNext() == false)
                        break;
                    int i = cursor.getInt(0);
                    String n = cursor.getString(1);
                    int s = cursor.getInt(2);
                    String temp = list.getText().toString();
                    list.setText(temp + "\n" + i + "    " + n + "   " + s);
                }
            }
        });
    }
}
