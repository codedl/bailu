package com.example.appzhuzhuang;

import android.os.Handler;
import android.os.Message;

//处理ui线程与非ui线程交互事件
public class uiHandler extends Handler {
    private String tag = "uiHandler";

    @Override
    public void handleMessage(Message msg) {
        switch (msg.what) {
            case 0x01://与主控连接断开
                fg_zhuk.setConnectClick(true);
                break;
            case 0x70://发送文件
                fg_zhuk.sendAll();
                break;
            case 0x72://开启干扰
                //设置button为true，点击成false，开始播放
                fg_zhuz.setDisturb(true);
                break;
            case 0x73://关闭干扰
                //设置button为false，点击成true，停止播放
                fg_zhuz.setDisturb(false);
                fg_zhuk.socket.send(procotol.disturbPrepare(false, true));//成功关闭干扰回复主机
                break;
        }
    }
}
