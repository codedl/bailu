package com.example.appzhuzhuang;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.provider.Settings;
import android.util.Log;
import android.widget.TextView;

public class gps {
    private Activity activity;
    private TextView txt_gps;
    private Context context;
    private LocationManager lm;

    public gps(Context context, Activity activity) {
        this.context = context;
        this.activity = activity;
//        View view = LayoutInflater.from(context).inflate(R.layout.main, null);
        txt_gps = activity.findViewById(R.id.gps);
        gpsInit();
    }

    private void gpsInit() {

        lm = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
        if (!isGPSable(lm)) {
            Intent intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
            context.startActivity(intent);
        }
        if (context.checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && context.checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            Log.d("print", "permission failed");
        }
        Location lc = lm.getLastKnownLocation(LocationManager.GPS_PROVIDER);
        updateShow(lc);
        lm.requestLocationUpdates(LocationManager.GPS_PROVIDER, 100, 1, new LocationListener() {
            @Override
            public void onLocationChanged(Location location) {
                updateShow(location);
            }

            @Override
            public void onStatusChanged(String s, int i, Bundle bundle) {

            }

            @Override
            public void onProviderEnabled(String s) {
                if (context.checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && context.checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {

                }
                updateShow(lm.getLastKnownLocation(s));
            }

            @Override
            public void onProviderDisabled(String s) {
                updateShow(null);
            }
        });

    }

    private void updateShow(Location location) {

        if (location != null) {
            StringBuilder sb = new StringBuilder();
            //保存经纬度
            param.longitude = (float) location.getLongitude();
            param.latitude = (float) location.getLatitude();
            //6位有效数字约精确到0.111322222米
            sb.append("经度:" + String.format("%03.6f", location.getLongitude()) + "\n");
            sb.append("纬度:" + String.format("%03.6f", location.getLatitude()));
            txt_gps.setText(sb.toString());
        } else {
            txt_gps.setText("经度117.134746\n纬度031.839734");
        }
    }

    public boolean isGPSable(LocationManager lm) {
        return lm.isProviderEnabled(LocationManager.GPS_PROVIDER) ? true : false;
    }

}
