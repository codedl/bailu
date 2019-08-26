/*
 * main.cpp
 *
 *  Created on: 2015-12-23
 *      Author: hai
 */
#include <QtGui/QApplication>
#include <QtCore/QTextCodec>
#include <QtGui/QDesktopWidget>]
#include <QtGui/QPushButton>

#include "mainform/mainform.h"
#include "mainform/mainthread.h"
#include "common/global.h"

int main(int argc, char ** argv)
{
  //program create
  QApplication a( argc, argv );


  //system text code
  QTextCodec *codec = QTextCodec::codecForName("GBK");
  QTextCodec::setCodecForLocale(codec);
  QTextCodec::setCodecForCStrings(codec);
  QTextCodec::setCodecForTr(codec);

  //lcd size
  SCREENWIDTH = 480;
  SCREENHEIGHT = 272;

 //system main form 系统主结构
  mainForm mf;
  mf.hide();

  //  system minor thread 系统次线程
  mainThread mt;
  mt.setStackSize(1024 * 1024);
  QObject::connect(&mt, SIGNAL(drawState(void)), &mf, SLOT(drawStateSlots(void)));	//绘制状态
  mt.start();


//  //  system minor thread 系统次线程
//  minorThread nt;
//  nt.setStackSize(1024 * 512);
//  QObject::connect(&nt, SIGNAL(drawProcess(void)), &mf, SLOT(drawProcessSlots(void)));	//绘制状态
//  nt.start();

  //print message 打印信息
  if (__DEBUG)
  {
    printf("\nmain codes over, program start successed\n");
  }

  //program loop 程序循环
  return a.exec();
}
