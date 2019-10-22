package com.example.myapplication;

import android.content.Context;
import android.view.View;

import androidx.appcompat.app.AlertDialog;

public class dialog {
    private Context context;
    private AlertDialog.Builder builder;
    private AlertDialog alertDialog;

    public dialog(Context context) {
        this.context = context;
    }

    //创建对话框
    //@title:对话框标题
    //@view:填充对话框的控件
    //@cancel:
    protected void create(String title, View view) {
        this.builder = new AlertDialog.Builder(context);
        this.builder.setTitle(title)
                .setView(view)
                .setPositiveButton("确定", null)
                .setNegativeButton("取消", null);
        this.alertDialog = this.builder.create();
        this.alertDialog.setCanceledOnTouchOutside(false);//点击对话框区域外不退出
        this.alertDialog.show();
    }

    //为对话框按钮设置监听事件
    //@button对话框按钮 AlertDialog.BUTTON_POSITIVE;int BUTTON_NEGATIVE;
    //@listener对话框按钮的监听事件
    protected void setButtonListener(int button, View.OnClickListener listener) {
        this.alertDialog.getButton(button).setOnClickListener(listener);

    }

    //对话框消失
    protected void dismiss() {
        builder = null;
        this.alertDialog.dismiss();
        alertDialog = null;

        fileManager.savexml();//对话框消失时保存系统设置
    }

}
