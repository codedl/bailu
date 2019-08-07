#ifndef MESSAGES_H_
#define MESSAGES_H_

//EMI 标准
#define _CE101_4                      1
#define _CE102_1                      2
#define _RE101_2                      3
#define _RE102_2                      4
#define _RE102_3                      5

//定义PCF8563地址
#define PCF8563_CONTOL_STATUS1_REG_ADDR            0x00            //控制状态1
#define PCF8563_CONTOL_STATUS2_REG_ADDR            0x01            //控制状态2
#define PCF8563_SECONDS_VL_REG_ADDR                0x02            //秒
#define PCF8563_MINUTE_REG_ADDR                    0x03            //分钟
#define PCF8563_HOURS_REG_ADDR                     0x04            //小时
#define PCF8563_DAYS_REG_ADDR                      0x05            //天
#define PCF8563_WEEKDAYS_REG_ADDR                  0x06            //星期
#define PCF8563_MONTH_CENTURY_REG_ADDR             0x07            //月份、 世纪
#define PCF8563_YEAR_REG_ADDR                      0x08            //年
#define PCF8563_MINUTE_ALARM_REG_ADDR              0x09            //分钟警告
#define PCF8563_HOUR_ALARM_REG_ADDR                0x0A            //小时警告
#define PCF8563_DAY_ALARM_REG_ADDR                 0x0B            //天警告
#define PCF8563_WEEKDAY_ALARM_REG_ADDR             0x0C            //星期警告
#define PCF8563_CLKOUT_REG_ADDR                    0x0D            //时钟输出
#define PCF8563_TIMER_CONTROL_REG_ADDR             0x0E            //时间控制
#define PCF8563_CONTDOWN_VALUE_REG_ADDR            0x0F            //计数值


//function return value
#define _RESULT_SUCCESS                            0
#define _RESULT_FAILED                            -1
#define _RESULT_UNKNOWN                           -2

#define _I2C_ERROR                                 -0x2001
#define _I2C_SUCCESS                               -0x2002

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

//datetime
#define _MSG_DATE_INVALID             -0x1040
#define _MSG_DATE_YEAR_INVALID        -0x1041
#define _MSG_DATE_MONTH_INVALID       -0x1042
#define _MSG_DATE_DAY_INVALID         -0x1043
#define _MSG_TIME_INVALID             -0x1044
#define _MSG_TIME_HOUR_INVALID        -0x1045
#define _MSG_TIME_MINUTE_INVALID      -0x1046
#define _MSG_TIME_SECOND_INVALID      -0x1047
#define _MSG_DATETIME_SETFAILED       -0x1048

struct errMessageDef
{
  long int code;
  char* msgEn;
  char* msgCh;
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

  {_MSG_DATE_INVALID, "date is invalid", "日期错误"},
  {_MSG_DATE_YEAR_INVALID, "date of year is invalid", "年份输入错误"},
  {_MSG_DATE_MONTH_INVALID, "date of month is invalid", "月份输入错误"},
  {_MSG_DATE_DAY_INVALID, "date of day is invalid", "日期输入错误"},
  {_MSG_TIME_INVALID, "time is invalid", "时间错误"},
  {_MSG_TIME_HOUR_INVALID, "time of hour is invalid", "时输入错误"},
  {_MSG_TIME_MINUTE_INVALID, "time of minute is invalid", "分输入错误"},
  {_MSG_TIME_SECOND_INVALID, "time of second is invalid", "秒输入错误"},
  {_MSG_DATETIME_SETFAILED, "date is invalid", "日期时间设置失败"},
};

#endif
