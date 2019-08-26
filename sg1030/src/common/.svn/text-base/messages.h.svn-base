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
  {_RESULT_UNKNOWN, "unknown error", "未知错误"},
};

static struct errMessageDef errMessage[] =
{
  {_MSG_SYSTEM_CMDPHASEERROR, "system phase error", "系统语法错误"},
  {_MSG_SYSTEM_CMDEXECERROR, "system command error", "系统指令错误"},
  {_MSG_SYSTEM_FILECREATEERROR, "system create file error", "创建系统文件失败"},
  {_MSG_SYSTEM_FILEOPENERROR, "file open failed", "系统文件打开错误"},
  {_MSG_SYSTEM_FILECHMODERROR, "file property chmod failed", "文件属性修改失败"},
  {_MSG_SYSTEM_CREATEDIRERROR, "create system directory failed", "创建系统文件夹失败"},

  {_MSG_USBDISK_NOTFOUND, "not found usb disk", "未发现USB闪存设备"},
  {_MSG_USBDISK_READONLY, "usb disk is read only", "USB闪存设备只读"},
  {_MSG_USBDISK_NOSPACE, "usb disk no free space", "USB闪存设备无可用空间"},
  {_MSG_USBDISK_WRITEFAIL, "usb disk write fail", "USB闪存设备写失败"},
  {_MSG_USBDISK_MOUNTFAIL, "usb disk load failed", "USB闪存设备加载失败"},
  {_MSG_USBDISK_UNMOUNTFAIL, "usb disk unload failed", "USB闪存设备卸载失败"},

  {_MSG_LOCAL_FILE_TOMAXLIMIT, "saved files reach to max limit", "储存文件数量已达到上限"},
  {_MSG_LOCAL_NOSPACE, "no free space", "磁盘空间不足"},

  {_MSG_LOCAL_PRINTER_NOTFOUND, "not found printer", "未发现打印机"},
  {_MSG_LOCAL_PRINTER_NOTSUPPORT, "printer not support", "打印机不支持"},

  {_MSG_SERIAL_NOTFOUND, "serial port not found", "未发现串口"},
  {_MSG_SERIAL_OPEN_FAIL, "serial port open failed", "串口打开失败"},
  {_MSG_SERIAL_SET_FAIL, "serial port config failed", "串口配置失败"},

};

#endif
