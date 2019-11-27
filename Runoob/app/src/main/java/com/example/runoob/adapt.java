package com.example.runoob;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;

import java.util.LinkedList;
import java.util.List;
import java.util.zip.Inflater;

public class adapt extends AppCompatActivity implements AdapterView.OnItemClickListener {
    private ListView list;
    private List<animal> mData;
    private AnimalAdapter adapter;
    private Context context;
    private int flags;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_adapt);
        list = findViewById(R.id.list);
        context = adapt.this;
        LayoutInflater inflater = LayoutInflater.from(this);
        View head = inflater.inflate(R.layout.list_header, null, false);
        View tail = inflater.inflate(R.layout.list_tail, null, false);
        mData = new LinkedList<animal>();
        mData.add(new animal("dingle", "husband"));
        mData.add(new animal("wangqianlan", "wife"));
        adapter = new AnimalAdapter((LinkedList<animal>) mData, context);

        list.addHeaderView(head);
        list.addFooterView(tail);
        list.setAdapter(adapter);
        list.setOnItemClickListener(this);
    }

    @Override
    public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
        System.out.println("you click:" + i);
    }


    public void clicl(View view) {
        flags++;
        System.out.println("flags:" + flags);
        switch (view.getId()) {
            case R.id.single:
                System.out.println("R.id.single");
                adapter.add(new animal("dingle", "lovetimes" + flags));
                break;
            case R.id.multiply:
                System.out.println("R.id.multiply");
                adapter.add(4, new animal("dingle", "goodtimes" + flags));
                break;
        }
        list.setAdapter(adapter);
    }
}
