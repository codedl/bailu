package com.example.myapplication;

import android.os.Build;

import com.loopj.android.http.AsyncHttpClient;

public class fileManager {
    private static AsyncHttpClient client = null;
    private static String agent;

    public fileManager() {
        client = new AsyncHttpClient();
        client.setMaxRetriesAndTimeout(3, 10000);
        client.setUserAgent(getAgent());
    }

    public static String getAgent() {
        if (agent == null || agent == "") {
            StringBuilder builder = new StringBuilder("dl");
            builder.append("android");
            builder.append("|" + Build.VERSION.RELEASE);
            builder.append("|" + Build.MODEL);
            agent = builder.toString();
        }
        return agent;
    }

    public static AsyncHttpClient getClient() {
        return client;
    }


}
