#ifndef MESSAGES_H_
#define MESSAGES_H_


//function return value
#define _RESULT_SUCCESS               0
#define _RESULT_FAILED                -1
#define _RESULT_UNKNOWN               -2

//system
#define _MSG_SYSTEM_CMDPHASEERROR     -0x000a
#define _MSG_SYSTEM_CMDEXECERROR      -0x000b
#define _MSG_SYSTEM_FILEOPENERROR     -0x000c
#define _MSG_SYSTEM_FILECREATEERROR   -0x000d
#define _MSG_SYSTEM_FILECHMODERROR    -0x000e
#define _MSG_SYSTEM_CREATEDIRERROR    -0x000f

//usb disk message
#define _MSG_USBDISK_NOTFOUND         -0x1000
#define _MSG_USBDISK_READONLY         -0x1001
#define _MSG_USBDISK_NOSPACE          -0x1002
#define _MSG_USBDISK_WRITEFAIL        -0x1003
#define _MSG_USBDISK_MOUNTFAIL        -0x1004
#define _MSG_USBDISK_UNMOUNTFAIL      -0x1005

//local space
#define _MSG_LOCAL_FILE_TOMAXLIMIT    -0x1010
#define _MSG_LOCAL_NOSPACE            -0x1011

//printer
#define _MSG_LOCAL_PRINTER_NOTFOUND   -0x1020
#define _MSG_LOCAL_PRINTER_NOTSUPPORT -0x1021

//serial
#define _MSG_SERIAL_NOTFOUND          -0x1030
#define _MSG_SERIAL_OPEN_FAIL         -0x1031
#define _MSG_SERIAL_SET_FAIL          -0x1032

struct errMessageDef
{
  long int code;
  const char* msgEn;
  const char* msgCh;
};

static struct errMessageDef unknownMessage[1] =
{
  {_RESULT_UNKNOWN, "unknown error", "δ֪����"},
};

static struct errMessageDef errMessage[] =
{
  {_MSG_SYSTEM_CMDPHASEERROR, "system phase error", "ϵͳ�﷨����"},
  {_MSG_SYSTEM_CMDEXECERROR, "system command error", "ϵͳָ�����"},
  {_MSG_SYSTEM_FILECREATEERROR, "system create file error", "����ϵͳ�ļ�ʧ��"},
  {_MSG_SYSTEM_FILEOPENERROR, "file open failed", "ϵͳ�ļ��򿪴���"},
  {_MSG_SYSTEM_FILECHMODERROR, "file property chmod failed", "�ļ������޸�ʧ��"},
  {_MSG_SYSTEM_CREATEDIRERROR, "create system directory failed", "����ϵͳ�ļ���ʧ��"},

  {_MSG_USBDISK_NOTFOUND, "not found usb disk", "δ����USB�����豸"},
  {_MSG_USBDISK_READONLY, "usb disk is read only", "USB�����豸ֻ��"},
  {_MSG_USBDISK_NOSPACE, "usb disk no free space", "USB�����豸�޿��ÿռ�"},
  {_MSG_USBDISK_WRITEFAIL, "usb disk write fail", "USB�����豸дʧ��"},
  {_MSG_USBDISK_MOUNTFAIL, "usb disk load failed", "USB�����豸����ʧ��"},
  {_MSG_USBDISK_UNMOUNTFAIL, "usb disk unload failed", "USB�����豸ж��ʧ��"},

  {_MSG_LOCAL_FILE_TOMAXLIMIT, "saved files reach to max limit", "�����ļ������Ѵﵽ����"},
  {_MSG_LOCAL_NOSPACE, "no free space", "���̿ռ䲻��"},

  {_MSG_LOCAL_PRINTER_NOTFOUND, "not found printer", "δ���ִ�ӡ��"},
  {_MSG_LOCAL_PRINTER_NOTSUPPORT, "printer not support", "��ӡ����֧��"},

  {_MSG_SERIAL_NOTFOUND, "serial port not found", "δ���ִ���"},
  {_MSG_SERIAL_OPEN_FAIL, "serial port open failed", "���ڴ�ʧ��"},
  {_MSG_SERIAL_SET_FAIL, "serial port config failed", "��������ʧ��"},

};

#endif
