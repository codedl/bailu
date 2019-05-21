#include <QtGui/QApplication>
#include <QtCore/QTextCodec>
#include <QtGui/QDesktopWidget>
#include "main/mainform.h"
#include "main/mainthread.h"
#include "common/global.h"

int main(int argc, char ** argv)
{

	//printf("freq max : %lld\n",FREQRESPHZ[350]);

    printf("\nnow enter main codes(v%d.%d.%d.%d.%d)\n", VER_MAJOR, VER_MINOR, VER_REVISION, \
  	VER_BUILD, VER_TAG);
  
  //program create
  QApplication a( argc, argv );

  //print message
  if (__DEBUG)
  {
  	printf("\nnow enter main codes(v%d.%d.%d.%d.%d)\n", VER_MAJOR, VER_MINOR, VER_REVISION, VER_BUILD, VER_TAG);
  }

  //system text code
  QTextCodec *codec = QTextCodec::codecForName("GBK");
  QTextCodec::setCodecForLocale(codec);
  QTextCodec::setCodecForCStrings(codec);
  QTextCodec::setCodecForTr(codec);

  //lcd size
  SCREENWIDTH = a.desktop()->width();
  SCREENHEIGHT = a.desktop()->height();
  //SCREENWIDTH = 800;
  //SCREENHEIGHT = 600;

  //system main form
  mainForm mf;
  mf.hide();

  //system main thread
  mainThread mt;
  mt.sysScpi = mf.sysScpi;
  mt.setStackSize(1024 * 1024 * 2);
  QObject::connect(&mt, SIGNAL(drawTop()), &mf, SLOT(drawTop()));
  QObject::connect(&mt, SIGNAL(drawCanvas()), &mf, SLOT(drawCanvas()));
  QObject::connect(&mt, SIGNAL(drawHelp()), &mf, SLOT(drawHelp()));
  QObject::connect(&mt, SIGNAL(drawButton()), &mf, SLOT(drawButton()));
  QObject::connect(&mt, SIGNAL(drawState()), &mf, SLOT(drawState()));
  QObject::connect(&mt, SIGNAL(drawAll()), &mf, SLOT(drawAll()));
  mt.start();

  //system minor thread
  minorThread nt;
  nt.sysScpi = mf.sysScpi;
  nt.setStackSize(1024 * 1024 * 2);
  QObject::connect(&nt, SIGNAL(drawTop()), &mf, SLOT(drawTop()));
  QObject::connect(&nt, SIGNAL(drawState()), &mf, SLOT(drawState()));
  QObject::connect(&nt, SIGNAL(drawInput()), &mf, SLOT(drawInput()));
  nt.start();

  //system data thread
  dataThread dt;
  dt.sysScpi = mf.sysScpi;
  dt.setStackSize(1024 * 1024 * 4);
  dt.start();

  //fft thread
  fftThread fourier;
  fourier.sysScpi = mf.sysScpi;
  fourier.setStackSize(1024 * 1024 * 2);
  fourier.start();

  //server socket thread
  tSocketThread net;
  net.setStackSize(1024 * 1024 * 2);
  QObject::connect(&net, SIGNAL(clientSocketDataArrive(int, int)), mf.sysScpi, SLOT(clientSocketDataArrive(int, int)));
  net.start();


  printf("main codes over, program start successed\n");

  //program loop
  return a.exec();
}
