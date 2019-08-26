#ifndef COMMAND_H_
#define COMMAND_H_

#define MAX_GROUP                                  10
#define VALUE_MAX_GROUP                            32
#define Fs 										   200000000

//错误代码
#define SCPI_CMD_UNKNOWEN                          -1000  //未知

#define __SCPI_SUCCESS                              0     //SCPI成功
#define __SCPI_FAILED                               1     //失败
#define __SCPI_IGNORE                               2     //忽略
#define __SCPI_CANCEL                               3     //取消
#define __SCPI_UNSUPPORT                            4     //不支持
#define __SCPI_RETURN                               5     //返回

//标准指令
#define SCPI_CMD_STANDARD_CLS                       0x1000
#define SCPI_CMD_STANDARD_ESE                       0x1001
#define SCPI_CMD_STANDARD_ESE_QUERY                 0x1002
#define SCPI_CMD_STANDARD_ESR_QUERY                 0x1003
#define SCPI_CMD_STANDARD_IDN_QUERY                 0x1004
#define SCPI_CMD_STANDARD_OPC                       0x1005
#define SCPI_CMD_STANDARD_OPC_QUERY                 0x1006
#define SCPI_CMD_STANDARD_RST                       0x1007
#define SCPI_CMD_STANDARD_SRE                       0x1008
#define SCPI_CMD_STANDARD_SRE_QUERY                 0x1009
#define SCPI_CMD_STANDARD_STB_QUERY                 0x100A
#define SCPI_CMD_STANDARD_TRG                       0x100B
#define SCPI_CMD_STANDARD_WAI                       0x100C
#define SCPI_CMD_STANDARD_CAL                       0x100D
#define SCPI_CMD_STANDARD_VER                       0x100E
#define SCPI_CMD_STANDARD_REG                       0x100F
#define SCPI_CMD_STANDARD_PRECAL                    0x1010
#define SCPI_CMD_STANDARD_CAL_9KHZ_SET              0x1011
#define SCPI_CMD_STANDARD_CAL_9KHZ_GET              0x1012
#define SCPI_CMD_STANDARD_CAL_20KHZ_SET             0x1013
#define SCPI_CMD_STANDARD_CAL_20KHZ_GET             0x1014

//自定义指令
#define SCPI_CMD_UPDATE_BEGIN                        0xF000
#define SCPI_CMD_UPDATE_FILE                         0xF001
#define SCPI_CMD_UPDATE_END                          0xF002
#define SCPI_CMD_UPDATE_CANCEL                       0xF003
#define SCPI_CMD_UPDATE_CRC                          0xF004

//扩展指令
#define SCPI_CMD_FREQ_CENTER_SET                    0x1100
#define SCPI_CMD_FREQ_CENTER_GET                    0x1101
#define SCPI_CMD_FREQ_START_SET                     0x1102
#define SCPI_CMD_FREQ_START_GET                     0x1103
#define SCPI_CMD_FREQ_STOP_SET                      0x1104
#define SCPI_CMD_FREQ_STOP_GET                      0x1105
#define SCPI_CMD_FREQ_CFSTEP_SET                    0x1106
#define SCPI_CMD_FREQ_CFSTEP_GET                    0x1107
#define SCPI_CMD_FREQ_CFSTEP_AUTO_SET               0x1108
#define SCPI_CMD_FREQ_CFSTEP_AUTO_GET               0x1109
#define SCPI_CMD_FREQ_OFFSET_SET                    0x110A
//#define SCPI_CMD_FREQ_OFFSET_GET                    0x110B
#define SCPI_CMD_FREQ_REFERENCE_SET                 0x110C
#define SCPI_CMD_FREQ_REFERENCE_GET                 0x110D

//log Freq
#define SCPI_CMD_FREQ_FREQTYPE_SET                  0x110E
#define SCPI_CMD_FREQ_FREQTYPE_GET                  0x110F

#define SCPI_CMD_SPAN_SET                           0x1200
#define SCPI_CMD_SPAN_GET                           0x1201
#define SCPI_CMD_SPAN_FULL                          0x1202
#define SCPI_CMD_SPAN_ZERO                          0x1203
#define SCPI_CMD_SPAN_PREVIOUS                      0x1204

#define SCPI_CMD_AMPT_REFLEVEL_SET                  0x1300
#define SCPI_CMD_AMPT_REFLEVEL_GET                  0x1301
#define SCPI_CMD_AMPT_ATT_SET                       0x1302
#define SCPI_CMD_AMPT_ATT_GET                       0x1303
#define SCPI_CMD_AMPT_ATT_AUTO_SET                  0x1304
#define SCPI_CMD_AMPT_ATT_AUTO_GET                  0x1305
#define SCPI_CMD_AMPT_SCALEDIV_SET                  0x1306
#define SCPI_CMD_AMPT_SCALEDIV_GET                  0x1307
#define SCPI_CMD_AMPT_SCALETYPE_SET                 0x1308
#define SCPI_CMD_AMPT_SCALETYPE_GET                 0x1309
#define SCPI_CMD_AMPT_REFLEVEL_OFFSET_SET           0x130A
#define SCPI_CMD_AMPT_REFLEVEL_OFFSET_GET           0x130B
#define SCPI_CMD_AMPT_REFUNIT_SET                   0x130C
#define SCPI_CMD_AMPT_REFUNIT_GET                   0x130D
#define SCPI_CMD_AMPT_PREAMPT_SET                   0x130E
#define SCPI_CMD_AMPT_PREAMPT_GET                   0x130F

#define SCPI_CMD_TUNE_SET                           0x1400
#define SCPI_CMD_TUNE_GET                           0x1401

#define SCPI_CMD_BW_RBW_SET                         0x1500
#define SCPI_CMD_BW_RBW_GET                         0x1501
#define SCPI_CMD_BW_RBW_AUTO_SET                    0x1502
#define SCPI_CMD_BW_RBW_AUTO_GET                    0x1503
#define SCPI_CMD_BW_RBW_STEP_MODE_SET               0x1504
#define SCPI_CMD_BW_RBW_STEP_MODE_GET               0x1505
#define SCPI_CMD_BW_VBW_SET                         0x1506
#define SCPI_CMD_BW_VBW_GET                         0x1507
#define SCPI_CMD_BW_VBW_AUTO_SET                    0x1508
#define SCPI_CMD_BW_VBW_AUTO_GET                    0x1509
#define SCPI_CMD_BW_AVERAGE_COUNT_SET               0x150A
#define SCPI_CMD_BW_AVERAGE_COUNT_GET               0x150B
#define SCPI_CMD_BW_AVERAGE_STATE_SET               0x150C
#define SCPI_CMD_BW_AVERAGE_STATE_GET               0x150D

#define SCPI_CMD_BW_EMC_SET                         0x150F
#define SCPI_CMD_BW_EMC_GET                         0x1510
#define SCPI_CMD_BW_EMC_STATE_SET                   0x1511
#define SCPI_CMD_BW_EMC_STATE_GET                   0x1512

#define SCPI_CMD_SWEEP_TIME_SET                     0x1600
#define SCPI_CMD_SWEEP_TIME_GET                     0x1601
#define SCPI_CMD_SWEEP_TIME_AUTO_SET                0x1602
#define SCPI_CMD_SWEEP_TIME_AUTO_GET                0x1603
//#define SCPI_CMD_SWEEP_MODE_SET                     0x1604
//#define SCPI_CMD_SWEEP_MODE_GET                     0x1605
#define SCPI_CMD_SWEEP_POINTS_SET                   0x1606
#define SCPI_CMD_SWEEP_POINTS_GET                   0x1607

#define SCPI_CMD_TRIG_SET                           0x1700

#define SCPI_CMD_MEAS_SET_ST                        0x1800
#define SCPI_CMD_MEAS_SET_ACP                       0x1801
#define SCPI_CMD_MEAS_SET_CHP                       0x1802
#define SCPI_CMD_MEAS_SET_OBW                       0x1803
#define SCPI_CMD_MEAS_SET_DISC                      0x1804
#define SCPI_CMD_MEAS_SET_EMC                       0x1805
#define SCPI_CMD_MEAS_SET_SA                        0x1806
#define SCPI_CMD_MEAS_GET                           0x1807

#define SCPI_CMD_TRACE_MODE_SET                     0x1900
#define SCPI_CMD_TRACE_MODE_GET                     0x1901
#define SCPI_CMD_TRACE_DATA_GET                     0x1902
#define SCPI_CMD_SPECIAL_TRACE_754DATA_GET          0x1908

#define SCPI_CMD_TRACE_OPER_12EX                    0x1903
#define SCPI_CMD_TRACE_OPER_2DL2                    0x1904
#define SCPI_CMD_TRACE_OPER_23EX                    0x1905
#define SCPI_CMD_TRACE_OPER_13                      0x1906
#define SCPI_CMD_TRACE_OPER_23                      0x1907

#define SCPI_CMD_DETECTOR_SET                       0x1A00
#define SCPI_CMD_DETECTOR_GET                       0x1A01

#define SCPI_CMD_DISPLAY_FULLSCREEN_SET             0x1B00
#define SCPI_CMD_DISPLAY_FULLSCREEN_GET             0x1B01
#define SCPI_CMD_DISPLAY_FORMAT_ZOOM_SET            0x1B02
#define SCPI_CMD_DISPLAY_FORMAT_ZOOM_GET            0x1B03
#define SCPI_CMD_DISPLAY_LINE_Y_SET                 0x1B04
#define SCPI_CMD_DISPLAY_LINE_Y_GET                 0x1B05
#define SCPI_CMD_DISPLAY_LINE_Y_STATE_SET           0x1B06
#define SCPI_CMD_DISPLAY_LINE_Y_STATE_GET           0x1B07
#define SCPI_CMD_DISPLAY_Y_GAUGE_STATE_SET          0x1B08
#define SCPI_CMD_DISPLAY_Y_GAUGE_STATE_GET          0x1B09
#define SCPI_CMD_DISPLAY_GRID_STATE_SET             0x1B0A
#define SCPI_CMD_DISPLAY_GRID_STATE_GET             0x1B0B
#define SCPI_CMD_DISPLAY_LABEL_STATE_SET            0x1B0C
#define SCPI_CMD_DISPLAY_LABEL_STATE_GET            0x1B0D
#define SCPI_CMD_DISPLAY_SKIN_SET                   0x1B0E
#define SCPI_CMD_DISPLAY_SKIN_GET                   0x1B0F

#define SCPI_CMD_MEASSETUP_ACP_BANDWIDTH_INT_SET    0x1C00
#define SCPI_CMD_MEASSETUP_ACP_BANDWIDTH_INT_GET    0x1C01
#define SCPI_CMD_MEASSETUP_ACP_CHANNEL_COUNT_SET    0x1C02
#define SCPI_CMD_MEASSETUP_ACP_CHANNEL_COUNT_GET    0x1C03
#define SCPI_CMD_MEASSETUP_ACP_SPACE_SET            0x1C04
#define SCPI_CMD_MEASSETUP_ACP_SPACE_GET            0x1C05
#define SCPI_CMD_MEASSETUP_CHPOWER_FREQ_SPAN_SET    0x1C06
#define SCPI_CMD_MEASSETUP_CHPOWER_FREQ_SPAN_GET    0x1C07
#define SCPI_CMD_MEASSETUP_OBW_PERCENT_SET          0x1C08
#define SCPI_CMD_MEASSETUP_OBW_PERCENT_GET          0x1C09
#define SCPI_CMD_MEASSETUP_OBW_FREQ_SPAN_SET        0x1C0A
#define SCPI_CMD_MEASSETUP_OBW_FREQ_SPAN_GET        0x1C0B
#define SCPI_CMD_MEASSETUP_FETCH_ACP_MAIN           0x1C0C
#define SCPI_CMD_MEASSETUP_FETCH_ACP_LIST_GET       0x1C0D
#define SCPI_CMD_MEASSETUP_FETCH_ACP_LOWER          0x1C0E
#define SCPI_CMD_MEASSETUP_FETCH_ACP_UPPER          0x1C0F
#define SCPI_CMD_MEASSETUP_FETCH_CHPOWER_POWER_GET  0x1C10
#define SCPI_CMD_MEASSETUP_FETCH_OBW_BAND_GET       0x1C11

#define SCPI_CMD_MARKER_ALLOFF                      0x1D00
#define SCPI_CMD_MARKER_STATE_SET                   0x1D01
#define SCPI_CMD_MARKER_STATE_GET                   0x1D02
#define SCPI_CMD_MARKER_TRACE_SET                   0x1D03
#define SCPI_CMD_MARKER_TRACE_GET                   0x1D04
#define SCPI_CMD_MARKER_MODE_SET                    0x1D05
#define SCPI_CMD_MARKER_MODE_GET                    0x1D06
#define SCPI_CMD_MARKER_TABLE_STATE_SET             0x1D07
#define SCPI_CMD_MARKER_TABLE_STATE_GET             0x1D08
#define SCPI_CMD_MARKER_X_SET                       0x1D09
#define SCPI_CMD_MARKER_X_GET                       0x1D0A
#define SCPI_CMD_MARKER_Y_GET                       0x1D0B

#define SCPI_CMD_MARKER_FCTN_ALLOFF                 0x1E00
#define SCPI_CMD_MARKER_FCTN_FREQCOUNT_STATE_SET    0x1E01
#define SCPI_CMD_MARKER_FCTN_FREQCOUNT_STATE_GET    0x1E02
#define SCPI_CMD_MARKER_FCTN_FREQCOUNT_X_GET        0x1E03
#define SCPI_CMD_MARKER_FCTN_FREQCOUNT_RBW_SET      0x1E04
#define SCPI_CMD_MARKER_FCTN_FREQCOUNT_RBW_GET      0x1E05
#define SCPI_CMD_MARKER_FCTN_NOISE_STATE_SET        0x1E06
#define SCPI_CMD_MARKER_FCTN_NOISE_STATE_GET        0x1E07
#define SCPI_CMD_MARKER_FCTN_NOISE_Y_GET            0x1E08
#define SCPI_CMD_MARKER_FCTN_NOISE_SFREQ_SET        0x1E09
#define SCPI_CMD_MARKER_FCTN_NOISE_SFREQ_GET        0x1E0A
#define SCPI_CMD_MARKER_FCTN_NOISE_SPAN_SET         0x1E0B
#define SCPI_CMD_MARKER_FCTN_NOISE_SPAN_GET         0x1E0C
#define SCPI_CMD_MARKER_FCTN_NDB_STATE_SET          0x1E0D
#define SCPI_CMD_MARKER_FCTN_NDB_STATE_GET          0x1E0E
#define SCPI_CMD_MARKER_FCTN_NDB_NDB_SET            0x1E0F
#define SCPI_CMD_MARKER_FCTN_NDB_NDB_GET            0x1E10
#define SCPI_CMD_MARKER_FCTN_NDB_RESULT_GET         0x1E11

#define SCPI_CMD_MARKER_TO_CENTER                   0x1F00
#define SCPI_CMD_MARKER_TO_STEP                     0x1F01
#define SCPI_CMD_MARKER_TO_START                    0x1F02
#define SCPI_CMD_MARKER_TO_STOP                     0x1F03
#define SCPI_CMD_MARKER_TO_REFLEVEL                 0x1F04
#define SCPI_CMD_MARKER_DELTA_TO_SPAN               0x1F05
#define SCPI_CMD_MARKER_DELTA_TO_CENTER             0x1F06

#define SCPI_CMD_MARKER_PEAK_MAXIMUM                0x2000
#define SCPI_CMD_MARKER_PEAK_MAXIMUM_LEFT           0x2001
#define SCPI_CMD_MARKER_PEAK_MAXIMUM_RIGHT          0x2002
#define SCPI_CMD_MARKER_PEAK_MAXIMUM_NEXT           0x2003
#define SCPI_CMD_MARKER_PEAK_MINIMUM                0x2004
#define SCPI_CMD_MARKER_PEAK_CONTINUOUS_SET         0x2005
#define SCPI_CMD_MARKER_PEAK_CONTINUOUS_GET         0x2006

#define SCPI_CMD_SYSTEM_LANGUAGE_SET                0x2100
#define SCPI_CMD_SYSTEM_LANGUAGE_GET                0x2101
#define SCPI_CMD_SYSTEM_BRIGHTNESS_SET              0x2102
#define SCPI_CMD_SYSTEM_BRIGHTNESS_GET              0x2103
#define SCPI_CMD_SYSTEM_DISPLAY_ENABLE_SET          0x2104
#define SCPI_CMD_SYSTEM_DISPLAY_ENABLE_GET          0x2105
#define SCPI_CMD_SYSTEM_CAL_CHANNEL_MODE_SET        0x2106
#define SCPI_CMD_SYSTEM_CAL_CHANNEL_MODE_GET        0x2107
#define SCPI_CMD_SYSTEM_CLOCK_STATE_SET             0x2108
#define SCPI_CMD_SYSTEM_CLOCK_STATE_GET             0x2109
#define SCPI_CMD_SYSTEM_CLOCK_DATE_SET              0x210A
#define SCPI_CMD_SYSTEM_CLOCK_DATE_GET              0x210B
#define SCPI_CMD_SYSTEM_CLOCK_TIME_SET              0x210C
#define SCPI_CMD_SYSTEM_CLOCK_TIME_GET              0x210D
#define SCPI_CMD_SYSTEM_CLOCK_DATE_FORMAT_SET       0x210E
#define SCPI_CMD_SYSTEM_CLOCK_DATE_FORMAT_GET       0x210F
#define SCPI_CMD_SYSTEM_CONFIGURE_INFORMATION_GET   0x2110
#define SCPI_CMD_SYSTEM_CONFIGURE_MESSAGE_GET       0x2111
#define SCPI_CMD_SYSTEM_POWERON_TYPE_SET            0x2112
#define SCPI_CMD_SYSTEM_POWERON_TYPE_GET            0x2113
#define SCPI_CMD_SYSTEM_PRESET_TYPE_SET             0x2114
#define SCPI_CMD_SYSTEM_PRESET_TYPE_GET             0x2115
#define SCPI_CMD_SYSTEM_PRESET_USER_SAVE            0x2116
#define SCPI_CMD_USER_CALIBRATION_SET               0x2117
#define SCPI_CMD_USER_CALIBRATION_GET               0x2118
#define SCPI_CMD_USER_CALIBRATION_RESTORE           0x2119
#define SCPI_CMD_SYSTEM_COMMUNICATE_LAN_IP_SET      0x211A
#define SCPI_CMD_SYSTEM_COMMUNICATE_LAN_IP_GET      0x211B
#define SCPI_CMD_SYSTEM_COMMUNICATE_LAN_MASK_SET    0x211C
#define SCPI_CMD_SYSTEM_COMMUNICATE_LAN_MASK_GET    0x211D
#define SCPI_CMD_SYSTEM_COMMUNICATE_LAN_GATE_SET    0x211E
#define SCPI_CMD_SYSTEM_COMMUNICATE_LAN_GATE_GET    0x211F

#define SCPI_CMD_MEMORY_CATALOG                     0x2200
#define SCPI_CMD_MEMORY_DISK_INFORMATION            0x2201
#define SCPI_CMD_MEMORY_STORE_STATE                 0x2202
#define SCPI_CMD_MEMORY_STORE_TRACE                 0x2203
#define SCPI_CMD_MEMORY_STORE_SCREEN                0x2204
#define SCPI_CMD_MEMORY_LOAD_TRACE                  0x2205
#define SCPI_CMD_MEMORY_LOAD_SCREEN                 0x2206
#define SCPI_CMD_MEMORY_DELETE_TRACE                0x2207
#define SCPI_CMD_MEMORY_DELETE_SCREEN               0x2208
#define SCPI_CMD_MEMORY_DELETE_TRACE_ALL            0x2209
#define SCPI_CMD_MEMORY_DELETE_SCREEN_ALL           0x220A
#define SCPI_CMD_MEMORY_DELETE_ALL                  0x220B
#define SCPI_CMD_MEMORY_COPY_ALL                    0x220C
#define SCPI_CMD_MEMORY_COPY_TRACE_ALL              0x220D
#define SCPI_CMD_MEMORY_COPY_TRACE                  0x220E
#define SCPI_CMD_MEMORY_COPY_SCREEN_ALL             0x220F
#define SCPI_CMD_MEMORY_COPY_SCREEN                 0x2210

#define SCPI_CMD_SOURCE_TRACKGEN_STATE_SET          0x2300
#define SCPI_CMD_SOURCE_TRACKGEN_STATE_GET          0x2301
#define SCPI_CMD_SOURCE_TRACKGEN_POWER_SET          0x2302
#define SCPI_CMD_SOURCE_TRACKGEN_POWER_GET          0x2303
#define SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_STATE_SET  0x2304
#define SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_STATE_GET  0x2305
#define SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_NORMALIZE  0x2306
#define SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_RLEVEL_SET 0x2307
#define SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_RLEVEL_GET 0x2308
#define SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_POWER_SET  0x2309
#define SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_POWER_GET  0x230A
#define SCPI_CMD_SOURCE_SOURCEGEN_STATE_SET         0x230B
#define SCPI_CMD_SOURCE_SOURCEGEN_STATE_GET         0x230C
#define SCPI_CMD_SOURCE_SOURCEGEN_FREQUENCY_SET     0x230D
#define SCPI_CMD_SOURCE_SOURCEGEN_FREQUENCY_GET     0x230E
#define SCPI_CMD_SOURCE_SOURCEGEN_POWER_SET         0x230F
#define SCPI_CMD_SOURCE_SOURCEGEN_POWER_GET         0x2310

#define SCPI_CMD_PRINT_IMAGE_COLOR_SET              0x2400
#define SCPI_CMD_PRINT_IMAGE_COLOR_GET              0x2401
#define SCPI_CMD_PRINT_IMAGE_SCREEN                 0x2402
#define SCPI_CMD_PRINT_IMAGE_TRACE                  0x2403
#define SCPI_CMD_PRINT_IMAGE_ORIENTATION_SET        0x2404
#define SCPI_CMD_PRINT_IMAGE_ORIENTATION_GET        0x2405
#define SCPI_CMD_PRINT_IMAGE_SIZE_SET               0x2406
#define SCPI_CMD_PRINT_IMAGE_SIZE_GET               0x2407
#define SCPI_CMD_PRINT_IMAGE_NUMS_SET               0x2408
#define SCPI_CMD_PRINT_IMAGE_NUMS_GET               0x2409

#define SCPI_CMD_DEMOD_STATE_SET                    0x2500
#define SCPI_CMD_DEMOD_STATE_GET                    0x2501
#define SCPI_CMD_DEMOD_SOUND_SET                    0x2502
#define SCPI_CMD_DEMOD_SOUND_GET                    0x2503

#define SCPI_CMD_DEVICE_LOGO_SET                    0x3000
#define SCPI_CMD_DEVICE_LOGO_SET_BEGIN              0x3001
#define SCPI_CMD_DEVICE_LOGO_SET_END                0x3002
#define SCPI_CMD_DEVICE_LOGO_GET                    0x3003
#define SCPI_CMD_DEVICE_SERIALNO_SET                0x3004
#define SCPI_CMD_DEVICE_SERIALNO_GET                0x3005
#define SCPI_CMD_DEVICE_MAC_SET                     0x3006
#define SCPI_CMD_DEVICE_MAC_GET                     0x3007
#define SCPI_CMD_DEVICE_TYPE_SET                    0x3008
#define SCPI_CMD_DEVICE_TYPE_GET                    0x3009
#define SCPI_CMD_DEVICE_STATE_GET                   0x300A
#define SCPI_CMD_DEVICE_STATE_EVA_SET               0x300B
#define SCPI_CMD_DEVICE_STATE_FULL_SET              0x300C
#define SCPI_CMD_DEVICE_COMPANY_SET                 0x300D
#define SCPI_CMD_DEVICE_COMPANY_GET                 0x300E

#define SCPI_CMD_FILE_SEND_UPDATE_MAIN				0x300D
#define SCPI_CMD_FILE_SEND_UPDATE_SIGNLE			0x300E
#define SCPI_CMD_FILE_SEND_UPDATE_FILELIST			0x300F
#define SCPI_CMD_FILE_GET_GSINGLE					0x3010
#define SCPI_CMD_FILE_GET_GDATA						0x3011

#define SCPI_CMD_TEST_TESTS							0x5FFF

/******************************************SG1030************************************************/
#define SCPI_CMD_FREQ_SET                   0x10000001
#define SCPI_CMD_FREQ_GET                   0x10000002

#define SCPI_CMD_FREQ_REF_SET               0x10000003
#define SCPI_CMD_FREQ_REF_GET               0x10000004

#define SCPI_CMD_FREQ_REF_STATE_SET         0x10000005
#define SCPI_CMD_FREQ_REF_STATE_GET         0x10000006

#define SCPI_CMD_FREQ_OFFS_SET              0x10000007
#define SCPI_CMD_FREQ_OFFS_GET              0x10000008

#define SCPI_CMD_FREQ_STAT_SET              0x10000009
#define SCPI_CMD_FREQ_STAT_GET              0x1000000A

#define SCPI_CMD_FREQ_MULT_SET              0x1000000B
#define SCPI_CMD_FREQ_MULT_GET              0x1000000C

#define SCPI_CMD_FREQ_STEP_SET              0x1000000D
#define SCPI_CMD_FREQ_STEP_GET              0x1000000E

#define SCPI_CMD_FREQ_EXT_SET               0x1000000F
#define SCPI_CMD_FREQ_EXT_GET               0x10000010

#define SCPI_CMD_POW_SET                    0x10000011
#define SCPI_CMD_POW_GET                    0x10000012

#define SCPI_CMD_POW_REF_SET                0x10000013
#define SCPI_CMD_POW_REF_GET                0x10000014

#define SCPI_CMD_POW_REF_STAT_SET           0x10000015
#define SCPI_CMD_POW_REF_STAT_GET           0x10000016

#define SCPI_CMD_POW_OFFS_SET               0x10000017
#define SCPI_CMD_POW_OFFS_GET               0x10000018

#define SCPI_CMD_FREQ_STAR_SET              0x10000019
#define SCPI_CMD_FREQ_STAR_GET              0x1000001A

#define SCPI_CMD_FREQ_STOP_SET              0x1000001B
#define SCPI_CMD_FREQ_STOP_GET              0x1000001C

#define SCPI_CMD_SWEEP_STAT_SET				0x10000040
#define SCPI_CMD_SWEEP_STAT_GET				0x10000041

//#define SCPI_CMD_SWEEP_MODE_SET              0x1000001D
//#define SCPI_CMD_SWEEP_MODE_GET              0x1000001E

#define SCPI_CMD_POW_STAR_SET               0x1000001F
#define SCPI_CMD_POW_STAR_GET               0x10000020

#define SCPI_CMD_POW_STOP_SET               0x10000021
#define SCPI_CMD_POW_STOP_GET               0x10000022

#define SCPI_CMD_SWEEP_MODE_SET               0x10000023
#define SCPI_CMD_SWEEP_MODE_GET               0x10000024

#define SCPI_CMD_POW_ERROR_SET              0x10000100
#define SCPI_CMD_POW_ERROR_GET              0x10000101

#define SCPI_CMD_SWE_DWEL_SET               0x10000025
#define SCPI_CMD_SWE_DWEL_GET               0x10000026

#define SCPI_CMD_SWE_POIN_SET               0x10000027
#define SCPI_CMD_SWE_POIN_GET               0x10000028

#define SCPI_CMD_OPER_CONT_SET              0x10000029
#define SCPI_CMD_OPER_CONT_GET              0x1000002A

#define SCPI_CMD_OPER_TYPE_SET              0x1000002B  //
#define SCPI_CMD_OPER_TYPE_GET              0x1000002C //

#define SCPI_CMD_OPER_RETR_SET              0x1000002D
#define SCPI_CMD_OPER_RETR_GET              0x1000002E

#define SCPI_CMD_OPER_MODE_SET              0x1000002F
#define SCPI_CMD_OPER_MODE_GET              0x10000030

#define SCPI_CMD_OPER_MAN_SET               0x10000031
#define SCPI_CMD_OPER_MAN_GET               0x10000032

#define SCPI_CMD_OPER_DIR_SET               0x10000033
#define SCPI_CMD_OPER_DIR_GET               0x10000034

#define SCPI_CMD_OPER_INIT_SET              0x10000035
#define SCPI_CMD_OPER_INIT_GET              0x10000036

#define SCPI_CMD_TRIG_TYPE_SET				0x10000200
#define SCPI_CMD_TRIG_TYPE_GET				0x10000201
#define SCPI_CMD_OPER_LOAD_SET              0x30000000

//#define SCPI_CMD_OPER_FREQ_STAT_SET			0x10000040
//#define SCPI_CMD_OPER_FREQ_STAT_GET			0x10000041

#define SCPI_CMD_LIST_FREQ_SET              0x10000037
#define SCPI_CMD_LIST_FREQ_GET              0x10000038

#define SCPI_CMD_LIST_POW_SET               0x10000039
#define SCPI_CMD_LIST_POW_GET               0x1000003A

#define SCPI_CMD_LIST_DWEL_SET              0x1000003B
#define SCPI_CMD_LIST_DWEL_GET              0x1000003C

#define SCPI_CMD_LIST_POINT_SET    			0x1000003D
#define SCPI_CMD_LIST_POINT_GET    			0x1000003E

#define SCPI_CMD_LIST_DOWN					0x80000000


//AM
#define SCPI_CMD_ANALOG_SOURCE_SET			0x20000030
#define SCPI_CMD_ANALOG_SOURCE_GET			0x20000031

#define SCPI_CMD_AM_STATE_SET               0x20000000
#define SCPI_CMD_AM_STATE_GET               0x20000001
#define SCPI_CMD_AM_DEPTH_SET               0x20000002
#define SCPI_CMD_AM_DEPTH_GET               0x20000003
#define SCPI_CMD_AM_RATE_SET                0x20000004
#define SCPI_CMD_AM_RATE_GET                0x20000005

//FM
#define SCPI_CMD_FM_STATE_SET               0x20000006
#define SCPI_CMD_FM_STATE_GET               0x20000007
#define SCPI_CMD_FM_DEV_SET                 0x20000008
#define SCPI_CMD_FM_DEV_GET                 0x20000009
#define SCPI_CMD_FM_RATE_SET                0x2000000A
#define SCPI_CMD_FM_RATE_GET                0x2000000B

//PM
#define SCPI_CMD_PM_STATE_SET               0x2000000C
#define SCPI_CMD_PM_STATE_GET               0x2000000D
#define SCPI_CMD_PM_DEV_SET                 0x2000000E
#define SCPI_CMD_PM_DEV_GET                 0x2000000F
#define SCPI_CMD_PM_RATE_SET                0x20000010
#define SCPI_CMD_PM_RATE_GET                0x20000011

//ASK
#define SCPI_CMD_ASK_STATE_SET              0x20000012
#define SCPI_CMD_ASK_STATE_GET              0x20000013
#define SCPI_CMD_ASK_TYPE_SET               0x20000014
#define SCPI_CMD_ASK_TYPE_GET               0x20000015
//#define SCPI_CMD_ASK_RATE_SET               0x20000016
//#define SCPI_CMD_ASK_RATE_GET               0x20000017
#define SCPI_CMD_ASK_DEPTH_SET              0x20000100
#define SCPI_CMD_ASK_DEPTH_GET              0x20000101

//FSK
#define SCPI_CMD_FSK_STATE_SET              0x20000018
#define SCPI_CMD_FSK_STATE_GET              0x20000019
#define SCPI_CMD_FSK_TYPE_SET               0x2000001A
#define SCPI_CMD_FSK_TYPE_GET               0x2000001B
//#define SCPI_CMD_FSK_RATE_SET               0x2000001C
//#define SCPI_CMD_FSK_RATE_GET               0x2000001D
#define SCPI_CMD_FSK_OFFSET_SET               0x2000001E
#define SCPI_CMD_FSK_OFFSET_GET               0x2000001F

//PSK
#define SCPI_CMD_PSK_STATE_SET              0x20000020
#define SCPI_CMD_PSK_STATE_GET              0x20000021
#define SCPI_CMD_PSK_TYPE_SET               0x20000022
#define SCPI_CMD_PSK_TYPE_GET               0x20000023
#define SCPI_CMD_PSK_PHASE_SET				0x20000026
#define SCPI_CMD_PSK_PHASE_GET				0x20000027
//#define SCPI_CMD_PSK_RATE_SET               0x20000024
//#define SCPI_CMD_PSK_RATE_GET               0x20000025

//ASK,FSK,PSK共用的调制速率和开关
#define SCPI_CMD_AFPSK_RATE_SET				0x20000050
#define SCPI_CMD_AFPSK_RATE_GET				0x20000051
#define SCPI_CMD_AFPSK_STATE_SET            0x20000052
#define SCPI_CMD_AFPSK_STATE_GET            0x20000053
#define SCPI_CMD_AFPSK_TYPE_GET				0x20000054

//数字调制外部调制
#define SCPI_CMD_DIGITAL_SOURCE_SET			0x20000055
#define SCPI_CMD_DIGITAL_SOURCE_GET			0x20000056
#define SCPI_CMD_DIGITAL_SEQUENCE_BEGIN		0x20000057
#define SCPI_CMD_DIGITAL_SEQUENCE_CANCEL	0x20000058
#define SCPI_CMD_DIGITAL_SEQUENCE_END		0x20000059

//Pulse
#define SCPI_CMD_PLUSE_STATE_SET            0x20001020
#define SCPI_CMD_PLUSE_STATE_GET            0x20001021
#define SCPI_CMD_PLUSE_PERIOD_SET           0x20001022
#define SCPI_CMD_PLUSE_PERIOD_GET           0x20001023
#define SCPI_CMD_PLUSE_WIDTH_SET            0x20001024
#define SCPI_CMD_PLUSE_WIDTH_GET            0x20001025
#define SCPI_CMD_PLUSE_SOURCE_SET			0x20001026
#define SCPI_CMD_PLUSE_SOURCE_GET			0x20001027
#define SCPI_CMD_PLUSE_TRIGGER_SET			0x20001028
#define SCPI_CMD_PLUSE_TRIGGER_GET			0x20001029

//OSK
#define SCPI_CMD_OSK_STATE_SET              0x20002020
#define SCPI_CMD_OSK_STATE_GET              0x20002021
#define SCPI_CMD_OSK_INTERVAL_SET           0x20002022
#define SCPI_CMD_OSK_INTERVAL_GET           0x20002023
#define SCPI_CMD_OSK_STEP_SET               0x20002024
#define SCPI_CMD_OSK_STEP_GET               0x20002025


//AWM
#define SCPI_CMD_AWM_FREQ_SET               0x20000026
#define SCPI_CMD_AWM_FREQ_GET               0x20000027

//LF
#define SCPI_CMD_LF_SWITCH_SET				0x40000006
#define SCPI_CMD_LF_SWITCH_GET				0x40000007
#define SCPI_CMD_LF_TYPE_SET				0x40000000
#define SCPI_CMD_LF_TYPE_GET				0x40000001
#define SCPI_CMD_LF_FREQ_SET				0x40000002
#define SCPI_CMD_LF_FREQ_GET				0x40000003
#define SCPI_CMD_LF_AMPL_SET				0x40000004
#define SCPI_CMD_LF_AMPL_GET				0x40000005

//上变频工能
#define SCPI_CMD_CONV_SET					0x40000100
#define SCPI_CMD_CONV_GET					0x40000101
#define SCPI_CMD_CONV_FREQ_SET				0x40000102
#define SCPI_CMD_CONV_FREQ_GET				0x40000103

//DRG
#define SCPI_CMD_DRG_STATE_SET				0xA0000001
#define SCPI_CMD_DRG_STATE_GET				0xA0000002
#define SCPI_CMD_DRG_UPLIMIT_SET			0xA0000003
#define SCPI_CMD_DRG_UPLIMIT_GET			0xA0000004
#define SCPI_CMD_DRG_DOWNLIMIT_SET			0xA0000005
#define SCPI_CMD_DRG_DOWNLIMIT_GET			0xA0000006
#define SCPI_CMD_DRG_POINTS_SET				0xA0000007
#define SCPI_CMD_DRG_POINTS_GET				0xA0000008
#define SCPI_CMD_DRG_RATE_SET				0xA0000009
#define SCPI_CMD_DRG_RATE_GET				0xA000000A
#define SCPI_CMD_DRG_WORKMODE_SET			0xA000000B
#define SCPI_CMD_DRG_WORKMODE_GET			0xA000000C

//SYSTEM
#define SCPI_CMD_SYSTEM_RFOUT_SET			0x50000000
#define SCPI_CMD_SYSTEM_RFOUT_GET			0x50000001
#define SCPI_CMD_SYSTEM_MODOUT_SET			0x50000002
#define SCPI_CMD_SYSTEM_MODOUT_GET			0x50000003
//#define SCPI_CMD_SYSTEM_LFOUT_SET			0x50000004
//#define SCPI_CMD_SYSTEM_LFOUT_GET			0x50000005
#define SCPI_CMD_SYSTEM_ALC_SET				0x50000006
#define SCPI_CMD_SYSTEM_ALC_GET				0x50000007

#define SCPI_CMD_FREQ_CONVERSION_TYPE_SET   0x60000000
#define SCPI_CMD_FREQ_CONVERSION_TYPE_GET   0x60000001
#define SCPI_CMD_FREQ_CONVERSION_FREQ_SET   0x60000002
#define SCPI_CMD_FREQ_CONVERSION_FREQ_GET   0x60000003

#define SCPI_CMD_FREQ_10MHZREF_SET          0x70000000
#define SCPI_CMD_FREQ_10MHZREF_GET          0x70000001


//从底层单片机上传上来的指令
#define SCPI_RCV_CMD_EXT_IF_AMPL			0x90000000
#define SCPI_RCV_SERIAL_NUM					0x90000001
#define SCPI_RCV_MODU_SOFT_VER				0x90000002
#define SCPI_RCV_MODU_DEV_TYPE				0x90000003
#define SCPI_CMD_STANDARD_IDNN_QUERY_SET		0x90000004

//////////////////////////////////////////////////////////////////




//检索返回值结构定义
struct resultDef
{
  unsigned long int cmdCode;  //指令
  QString value;              //value
  char auxValue;              //Marker or trace index
  char auxValue2;             //Marker or trace index
};

typedef struct __NODE
{
  char data[64];
  char data_abbreviation[10];
  unsigned long funcode;
  bool isellipsis;
  struct __NODE *leftchild, *rightbrother;

  void  init()
  {
	funcode = 0;
	leftchild = NULL;
	rightbrother = NULL;
	isellipsis = false;   //可省略项
	memset(data, 0, 64);
	memset(data_abbreviation, 0, 10);
  }
} CSNode, *pCSTree;

typedef struct _SCPI_keyboard
{
  char data[64];
  bool isellipsis;
  unsigned long funcode;
} SCPI_keyboard;

struct _SCPI_keyboardCmd
{
  char data[64];
  unsigned long funcode;
};

static struct _SCPI_keyboardCmd Mainkey[]=
{
  {"*CLS",															  SCPI_CMD_STANDARD_CLS},\
  {"*IDN?",															  SCPI_CMD_STANDARD_IDN_QUERY},\
  //zpc add test
  {"*IDNN",														  	  SCPI_CMD_STANDARD_IDNN_QUERY_SET},\

  {"*RST",															  SCPI_CMD_STANDARD_RST},\
  {"*FACTCAL",													 	  SCPI_CMD_STANDARD_CAL},\
  {"*FACTPRECAL",													  SCPI_CMD_STANDARD_PRECAL},\
  {"*VER?",															  SCPI_CMD_STANDARD_VER},\
};

static struct _SCPI_keyboardCmd test[] = {
  //ROOT
  {"",						    									  0x00000000},\

  //SYSTEM
  {":SYSTem:LANGuage",                                                SCPI_CMD_SYSTEM_LANGUAGE_SET},\
  {":SYSTem:LANGuage?",                                               SCPI_CMD_SYSTEM_LANGUAGE_GET},\
  {":DISPlay:BRIGhtness",                                             SCPI_CMD_SYSTEM_BRIGHTNESS_SET},\
  {":DISPlay:BRIGhtness?",                                            SCPI_CMD_SYSTEM_BRIGHTNESS_GET},\
  {":DISPlay:ENABle",                                                 SCPI_CMD_SYSTEM_DISPLAY_ENABLE_SET},\
  {":DISPlay:ENABle?",                                                SCPI_CMD_SYSTEM_DISPLAY_ENABLE_GET},\
  {":SYSTem:[RF]:CHANnel:MODE",                                       SCPI_CMD_SYSTEM_CAL_CHANNEL_MODE_SET},\
  {":SYSTem:[RF]:CHANnel:MODE?",                                      SCPI_CMD_SYSTEM_CAL_CHANNEL_MODE_GET},\
  {":DISPlay:ANNotation:CLOCk",                                       SCPI_CMD_SYSTEM_CLOCK_STATE_SET},\
  {":DISPlay:ANNotation:CLOCk:STATe",                                 SCPI_CMD_SYSTEM_CLOCK_STATE_SET},\
  {":DISPlay:ANNotation:CLOCk?",                                      SCPI_CMD_SYSTEM_CLOCK_STATE_GET},\
  {":DISPlay:ANNotation:CLOCk:STATe?",                                SCPI_CMD_SYSTEM_CLOCK_STATE_GET},\
  {":SYSTem:DATE",                                                    SCPI_CMD_SYSTEM_CLOCK_DATE_SET},\
  {":SYSTem:DATE?",                                                   SCPI_CMD_SYSTEM_CLOCK_DATE_GET},\
  {":SYSTem:TIME",                                                    SCPI_CMD_SYSTEM_CLOCK_TIME_SET},\
  {":SYSTem:TIME?",                                                   SCPI_CMD_SYSTEM_CLOCK_TIME_GET},\
  {":DISPlay:ANNotation:CLOCk:DATE:FORMat",                           SCPI_CMD_SYSTEM_CLOCK_DATE_FORMAT_SET},\
  {":DISPlay:ANNotation:CLOCk:DATE:FORMat?",                          SCPI_CMD_SYSTEM_CLOCK_DATE_FORMAT_GET},\
  {":SYSTem:CONFigure:INFomation?",                                   SCPI_CMD_SYSTEM_CONFIGURE_INFORMATION_GET},\
  {":SYSTem:CONFigure:MESSage?",                                      SCPI_CMD_SYSTEM_CONFIGURE_MESSAGE_GET},\
  {":SYSTem:PON:TYPE",                                                SCPI_CMD_SYSTEM_POWERON_TYPE_SET},\
  {":SYSTem:PON:TYPE?",                                               SCPI_CMD_SYSTEM_POWERON_TYPE_GET},\
  {":SYSTem:PRESet:TYPE",                                             SCPI_CMD_SYSTEM_PRESET_TYPE_SET},\
  {":SYSTem:PRESet:TYPE?",                                            SCPI_CMD_SYSTEM_PRESET_TYPE_GET},\
  {":SYSTem:PRESet:[USER]:SAVE",                                      SCPI_CMD_SYSTEM_PRESET_USER_SAVE},\
  {":CALibration",                                                    SCPI_CMD_USER_CALIBRATION_SET},\
  {":CALibration:ALL",                                                SCPI_CMD_USER_CALIBRATION_SET},\
  {":CALibration?",                                                   SCPI_CMD_USER_CALIBRATION_GET},\
  {":CALibration:ALL?",                                               SCPI_CMD_USER_CALIBRATION_GET},\
  {":CALibration:RESTore",                                            SCPI_CMD_USER_CALIBRATION_RESTORE},\

  //设置IP地址
  {":SYSTem:COMMunicate:LAN:IP:ADDRess",                               SCPI_CMD_SYSTEM_COMMUNICATE_LAN_IP_SET},\
  {":SYSTem:COMMunicate:LAN:IP:ADDRess?",                    	       SCPI_CMD_SYSTEM_COMMUNICATE_LAN_IP_GET},\
  {":SYSTem:COMMunicate:LAN:MASK",                                     SCPI_CMD_SYSTEM_COMMUNICATE_LAN_MASK_SET},\
  {":SYSTem:COMMunicate:LAN:MASK?",                                    SCPI_CMD_SYSTEM_COMMUNICATE_LAN_MASK_GET},\
  {":SYSTem:COMMunicate:LAN:GATE",                                     SCPI_CMD_SYSTEM_COMMUNICATE_LAN_GATE_SET},\
  {":SYSTem:COMMunicate:LAN:GATE?",                                    SCPI_CMD_SYSTEM_COMMUNICATE_LAN_GATE_GET},\

  //update
  {":UPDATE:BEGIN",                                                   SCPI_CMD_UPDATE_BEGIN},\
  {":UPDATE:FILE",                                                    SCPI_CMD_UPDATE_FILE},\
  {":UPDATE:END",                                                     SCPI_CMD_UPDATE_END},\
  {":UPDATE:CANCEL",                                                  SCPI_CMD_UPDATE_CANCEL},\
  {":UPDATE:CRC",                                                     SCPI_CMD_UPDATE_CRC},\

  //device
  {":DEVice:SELF:LOGO",                                               SCPI_CMD_DEVICE_LOGO_SET},\
  {":DEVice:SELF:LOGOB",                                              SCPI_CMD_DEVICE_LOGO_SET_BEGIN},\
  {":DEVice:SELF:LOGOE",                                              SCPI_CMD_DEVICE_LOGO_SET_END},\
  {":DEVice:SELF:LOGO?",                                              SCPI_CMD_DEVICE_LOGO_GET},\

  {":DEVice:SELF:MAC",												  SCPI_CMD_DEVICE_MAC_SET},\
  {":DEVice:SELF:MAC?",												  SCPI_CMD_DEVICE_MAC_GET},\

  {":DEVice:SELF:EVA?",                                               SCPI_CMD_DEVICE_STATE_GET},\
  {":DEVice:SELF:EVAluation",                                         SCPI_CMD_DEVICE_STATE_EVA_SET},\
  {":DEVice:SELF:Full",                                               SCPI_CMD_DEVICE_STATE_FULL_SET},\

  {":FILE:GET:SINGLE",												  SCPI_CMD_FILE_GET_GSINGLE},\
  {":FILE:GET:GDATA",												  SCPI_CMD_FILE_GET_GDATA},\
  {":FILE:SEND:SINGLE",									  		      SCPI_CMD_FILE_SEND_UPDATE_SIGNLE},\
  {":FILE:SEND:UPDATE:MAIN",									 	  SCPI_CMD_FILE_SEND_UPDATE_MAIN},\
  {":FILE:SEND:UPDATE:LIST",									  	  SCPI_CMD_FILE_SEND_UPDATE_FILELIST},\

  //TEST
  {":[SENSe]:TEST:TESTS",   					 			      	  SCPI_CMD_TEST_TESTS},\


  /////////////////////////////////////////////SG1030/////////////////////////////////////////////////
  ////////////////////////FREQuency////////////////////////
  {":FREQuency",		            	SCPI_CMD_FREQ_SET           },\
  {":FREQuency?",		            	SCPI_CMD_FREQ_GET           },\
  {":FREQuency:REFerence",		        SCPI_CMD_FREQ_REF_SET     	},\
  {":FREQuency:REFerence?",		        SCPI_CMD_FREQ_REF_GET      	},\
  {":FREQuency:REFerence:STATe",		SCPI_CMD_FREQ_REF_STATE_SET	},\
  {":FREQuency:REFerence:STATe?",		SCPI_CMD_FREQ_REF_STATE_GET	},\
  {":FREQuency:OFFSet",		        	SCPI_CMD_FREQ_OFFS_SET    	},\
  {":FREQuency:OFFSet?",		        SCPI_CMD_FREQ_OFFS_GET    	},\
  {":FREQuency:MULTiplier",		     	SCPI_CMD_FREQ_MULT_SET    	},\
  {":FREQuency:MULTiplier?",		  	SCPI_CMD_FREQ_MULT_GET    	},\

  //////////////////////////POWer//////////////////////////
  {":POWer",		                  	SCPI_CMD_POW_SET            },\
  {":POWer?",		                  	SCPI_CMD_POW_GET            },\
  {":POWer:REFerence",		        	SCPI_CMD_POW_REF_SET     	},\
  {":POWer:REFerence?",		          	SCPI_CMD_POW_REF_GET      	},\
  {":POWer:REFerence:STATe",		 	SCPI_CMD_POW_REF_STAT_SET	},\
  {":POWer:REFerence:STATe?",			SCPI_CMD_POW_REF_STAT_GET	},\
  {":POWer:OFFSet",		        		SCPI_CMD_POW_OFFS_SET     	},\
  {":POWer:OFFSet?",		          	SCPI_CMD_POW_OFFS_GET      	},\
  {":POWer:CABLe",                 		SCPI_CMD_POW_ERROR_SET 	},\
  {":POWer:CABLe?",                		SCPI_CMD_POW_ERROR_GET 	},\

  /////////////////////////SWEEP///////////////////////////
  //STEP
  {":FREQuency:STARt",		        	SCPI_CMD_FREQ_STAR_SET	},\
  {":FREQuency:STARt?",		        	SCPI_CMD_FREQ_STAR_GET	},\
  {":FREQuency:STOP",		          	SCPI_CMD_FREQ_STOP_SET	},\
  {":FREQuency:STOP?",		          	SCPI_CMD_FREQ_STOP_GET	},\
  {":POWer:STARt",		            	SCPI_CMD_POW_STAR_SET 	},\
  {":POWer:STARt?",		            	SCPI_CMD_POW_STAR_GET  	},\
  {":POWer:STOP",		              	SCPI_CMD_POW_STOP_SET  	},\
  {":POWer:STOP?",		              	SCPI_CMD_POW_STOP_GET 	},\
  {":SWEep:STAT",						SCPI_CMD_SWEEP_STAT_SET	},\
  {":SWEep:STAT?",						SCPI_CMD_SWEEP_STAT_GET	},\
  {":SWEep:MODE",		            	SCPI_CMD_SWEEP_MODE_SET  	},\
  {":SWEep:MODE?",		            	SCPI_CMD_SWEEP_MODE_GET 	},\
  {":SWEep:DWELl",		            	SCPI_CMD_SWE_DWEL_SET  	},\
  {":SWEep:DWELl?",		            	SCPI_CMD_SWE_DWEL_GET 	},\
  {":SWEep:POINts",		            	SCPI_CMD_SWE_POIN_SET  	},\
  {":SWEep:POINts?",		            SCPI_CMD_SWE_POIN_GET  	},\

  //LIST
  {":LIST:FREQuency",		          	SCPI_CMD_LIST_FREQ_SET 	},\
  {":LIST:FREQuency?",		          	SCPI_CMD_LIST_FREQ_GET	},\
  {":LIST:POWer",		              	SCPI_CMD_LIST_POW_SET 	},\
  {":LIST:POWer?",		              	SCPI_CMD_LIST_POW_GET 	},\
  {":LIST:DWELl",		              	SCPI_CMD_LIST_DWEL_SET 	},\
  {":LIST:DWELl?",		              	SCPI_CMD_LIST_DWEL_GET 	},\
  {":LIST:POINts",	    				SCPI_CMD_LIST_POINT_SET},\
  {":LIST:POINts?",	    				SCPI_CMD_LIST_POINT_GET},\
  {":LIST:LOADdown",						SCPI_CMD_LIST_DOWN},\
  //CONFIG
  {":INIT:TRIG",						SCPI_CMD_TRIG_TYPE_SET	},\
  {":INIT:TRIG?",						SCPI_CMD_TRIG_TYPE_GET	},\
  {":LIST:TYPE",		              	SCPI_CMD_OPER_TYPE_SET  },\
  {":LIST:TYPE?",		              	SCPI_CMD_OPER_TYPE_GET  },\
  {":LIST:RETRace",		            	SCPI_CMD_OPER_RETR_SET  },\
  {":LIST:RETRace?",		            SCPI_CMD_OPER_RETR_GET  },\
  {":LIST:MODE",		              	SCPI_CMD_OPER_MODE_SET  },\
  {":LIST:MODE?",		              	SCPI_CMD_OPER_MODE_GET  },\
  {":LIST:DIRection",		          	SCPI_CMD_OPER_DIR_SET   },\
  {":LIST:DIRection?",		          	SCPI_CMD_OPER_DIR_GET   },\
  {":INIT:CONTinuous",		   			SCPI_CMD_OPER_CONT_SET  },\
  {":INIT:CONTinuous?",		   			SCPI_CMD_OPER_CONT_GET  },\
//  {":INIT:TRIGger",                 	SCPI_CMD_OPER_INIT_SET  },\
//  {":INIT:TRIGger?",                	SCPI_CMD_OPER_INIT_GET  },\
//  {":LIST:LOADdown",                	SCPI_CMD_OPER_LOAD_SET  },\
//  {":FREQ:STAT",						SCPI_CMD_OPER_FREQ_STAT_SET	},\
//  {":FREQ:STAT?",						SCPI_CMD_OPER_FREQ_STAT_GET	},\
//  {":LIST:MANual",		            	SCPI_CMD_OPER_MAN_SET   },\
//  {":LIST:MANual?",		            	SCPI_CMD_OPER_MAN_GET   },\
//  {":FREQuency:MODE",             		SCPI_CMD_FREQ_MODE_SET  },\
//  {":FREQuency:MODE?",             		SCPI_CMD_FREQ_MODE_GET  },\

  //AM
  {":ANALog:SOURce",					SCPI_CMD_ANALOG_SOURCE_SET},\
  {":ANALog:SOURce?",					SCPI_CMD_ANALOG_SOURCE_GET},\
  {":AM:STATe",                       	SCPI_CMD_AM_STATE_SET },\
  {":AM:STATe?",                       	SCPI_CMD_AM_STATE_GET },\
  {":AM:DEPTh",                       	SCPI_CMD_AM_DEPTH_SET },\
  {":AM:DEPTh?",                       	SCPI_CMD_AM_DEPTH_GET },\
  {":AM:INTernal:FUNCtion:FREQuency", 	SCPI_CMD_AM_RATE_SET  },\
  {":AM:INTernal:FUNCtion:FREQuency?", 	SCPI_CMD_AM_RATE_GET  },\
  //FM
  {":FM:STATe",                       	SCPI_CMD_FM_STATE_SET },\
  {":FM:STATe?",                       	SCPI_CMD_FM_STATE_GET },\
  {":FM:DEViation",                   	SCPI_CMD_FM_DEV_SET   },\
  {":FM:DEViation?",                   	SCPI_CMD_FM_DEV_GET   },\
  {":FM:INTernal:FUNCtion:FREQuency", 	SCPI_CMD_FM_RATE_SET  },\
  {":FM:INTernal:FUNCtion:FREQuency?", 	SCPI_CMD_FM_RATE_GET  },\
  //PM
  {":PM:STATe",                       	SCPI_CMD_PM_STATE_SET },\
  {":PM:STATe?",                       	SCPI_CMD_PM_STATE_GET },\
  {":PM:DEViation",                   	SCPI_CMD_PM_DEV_SET   },\
  {":PM:DEViation?",                   	SCPI_CMD_PM_DEV_GET   },\
  {":PM:INTernal:FUNCtion:FREQuency", 	SCPI_CMD_PM_RATE_SET  },\
  {":PM:INTernal:FUNCtion:FREQuency?", 	SCPI_CMD_PM_RATE_GET  },\

  //数字调制外部调制
  {":DIGItal:SOURce",					SCPI_CMD_DIGITAL_SOURCE_SET},
  {":DIGItal:SOURce?",					SCPI_CMD_DIGITAL_SOURCE_GET},
  {":SEQUENCE:BEGIN",					SCPI_CMD_DIGITAL_SEQUENCE_BEGIN},\
  {":SEQUENCE:CANCEL",					SCPI_CMD_DIGITAL_SEQUENCE_CANCEL},\
  {":SEQUENCE:END",						SCPI_CMD_DIGITAL_SEQUENCE_END},\


  //ASK
  {":ASK:TYPE",                       	SCPI_CMD_ASK_TYPE_SET   },\
  {":ASK:TYPE?",                       	SCPI_CMD_ASK_TYPE_GET   },\
  {":ASK:DEPTh",						SCPI_CMD_ASK_DEPTH_SET},\
  {":ASK:DEPTh?",						SCPI_CMD_ASK_DEPTH_GET},\
//  {":ASK:INT:FUNC:FREQ",				SCPI_CMD_ASK_RATE_SET   },\
//  {":ASK:INT:FUNC:FREQ?",				SCPI_CMD_ASK_RATE_GET   },\
//  {":ASK:STAT",                      	SCPI_CMD_ASK_STATE_SET  },\
//  {":ASK:STAT?",                      	SCPI_CMD_ASK_STATE_GET  },\

  //FSK
  {":FSK:TYPE",                       		SCPI_CMD_FSK_TYPE_SET   },\
  {":FSK:TYPE?",                       		SCPI_CMD_FSK_TYPE_GET   },\
  {":FSK:MIN:FREQuency:OFFSet",             SCPI_CMD_FSK_OFFSET_SET   },\
  {":FSK:MIN:FREQuency:OFFSet?",            SCPI_CMD_FSK_OFFSET_GET   },\
//  {":FSK:INTernal:FUNCtion:FREQuency",	SCPI_CMD_FSK_RATE_SET   },\
//  {":FSK:INTernal:FUNCtion:FREQuency?",	SCPI_CMD_FSK_RATE_GET   },\
//  {":FSK:STAT",                      	SCPI_CMD_FSK_STATE_SET  },\
//  {":FSK:STAT?",                      	SCPI_CMD_FSK_STATE_GET  },\

  //PSK
  {":PSK:TYPE",                       	SCPI_CMD_PSK_TYPE_SET   },\
  {":PSK:TYPE?",                       	SCPI_CMD_PSK_TYPE_GET   },\
  {":PSK:PHASe:OFFSet",					SCPI_CMD_PSK_PHASE_SET	},\
  {":PSK:PHASe:OFFSet?",				SCPI_CMD_PSK_PHASE_GET	},\
//  {":PSK:INTernal:FUNCtion:FREQuency",	SCPI_CMD_PSK_RATE_SET   },\
//  {":PSK:INTernal:FUNCtion:FREQuency?",	SCPI_CMD_PSK_RATE_GET   },\
//  {":PSK:STAT",                      	SCPI_CMD_PSK_STATE_SET  },\
//  {":PSK:STAT?",                      	SCPI_CMD_PSK_STATE_GET  },\

  //ASK,FSK,PSK调制开关和调制速率
  {":AFPSK:INTernal:FUNCtion:FREQuency",				SCPI_CMD_AFPSK_RATE_SET},\
  {":AFPSK:INTernal:FUNCtion:FREQuency?",				SCPI_CMD_AFPSK_RATE_GET},\
  {":AFPSK:STATe",										SCPI_CMD_AFPSK_STATE_SET},\
  {":AFPSK:STATe?",										SCPI_CMD_AFPSK_STATE_GET},\
  //ASK,FSK,PSK调制类型获取
  {":AFPSK:TYPE?",										SCPI_CMD_AFPSK_TYPE_GET},\

  //PLUS
  {":PLUSe:STATe",                      			SCPI_CMD_PLUSE_STATE_SET},\
  {":PLUSe:STATe?",                     			SCPI_CMD_PLUSE_STATE_GET  },\
  {":PLUSe:SOURce",					  				SCPI_CMD_PLUSE_SOURCE_SET},\
  {":PLUSe:SOURce?",				  				SCPI_CMD_PLUSE_SOURCE_GET},\
  {":PLUSe:TRIGger",								SCPI_CMD_PLUSE_TRIGGER_SET},\
  {":PLUSe:TRIGger?",								SCPI_CMD_PLUSE_TRIGGER_GET},\
  {":PLUSe:INTernal:FUNCtion:PERIod",            	SCPI_CMD_PLUSE_PERIOD_SET  },\
  {":PLUSe:INTernal:FUNCtion:PERIod?",           	SCPI_CMD_PLUSE_PERIOD_GET  },\
  {":PLUSe:INTernal:FUNCtion:WIDTh",	          	SCPI_CMD_PLUSE_WIDTH_SET   },\
  {":PLUSe:INTernal:FUNCtion:WIDTh?",	          	SCPI_CMD_PLUSE_WIDTH_GET   },\

  //OSK
  {":OSK:STATe",                      	SCPI_CMD_OSK_STATE_SET  },\
  {":OSK:STATe?",                      	SCPI_CMD_OSK_STATE_GET  },\
  {":OSK:INTErval",                    	SCPI_CMD_OSK_INTERVAL_SET   },\
  {":OSK:INTErval?",                    SCPI_CMD_OSK_INTERVAL_GET   },\
  {":OSK:STEP", 	                    SCPI_CMD_OSK_STEP_SET   },\
  {":OSK:STEP?",	                    SCPI_CMD_OSK_STEP_GET   },\

  //AWM
  {":AWM:FREQuency",                  	SCPI_CMD_AWM_FREQ_SET   },\
  {":AWM:FREQuency?",                  	SCPI_CMD_AWM_FREQ_GET   },\

  //LF
  {":LFOutput:TYPE",					SCPI_CMD_LF_TYPE_SET	},\
  {":LFOutput:TYPE?",					SCPI_CMD_LF_TYPE_GET	},\
  {":LFOutput:FREQuency",				SCPI_CMD_LF_FREQ_SET},\
  {":LFOutput:FREQuency?",				SCPI_CMD_LF_FREQ_GET},\
  {":LFOutput:AMPLitude",				SCPI_CMD_LF_AMPL_SET},\
  {":LFOutput:AMPLitude?",				SCPI_CMD_LF_AMPL_GET},\
//  {":LFOut:SWITch",						SCPI_CMD_LF_SWITCH_SET	},\
//  {":LFOut:SWITch?",					SCPI_CMD_LF_SWITCH_GET	},\

  //上变频工能
  {":FREQuency:CONVersion",							SCPI_CMD_CONV_SET		},\
  {":FREQuency:CONVersion?",						SCPI_CMD_CONV_GET		},\
  {":FREQuency:CONVersion:FREQuency",				SCPI_CMD_CONV_FREQ_SET	},\
  {":FREQuency:CONVersion:FREQuency?",				SCPI_CMD_CONV_FREQ_GET	},\

  //DRG
  {":DRG:STATe",						SCPI_CMD_DRG_STATE_SET},\
  {":DRG:STATe?",						SCPI_CMD_DRG_STATE_GET},\
  {":DRG:UPLImit",						SCPI_CMD_DRG_UPLIMIT_SET},\
  {":DRG:UPLImit?",						SCPI_CMD_DRG_UPLIMIT_GET},\
  {":DRG:DOWNlimit",					SCPI_CMD_DRG_DOWNLIMIT_SET},\
  {":DRG:DOWNlimit?",					SCPI_CMD_DRG_DOWNLIMIT_GET},\
  {":DRG:POINts",						SCPI_CMD_DRG_POINTS_SET},\
  {":DRG:POINts?",						SCPI_CMD_DRG_POINTS_GET},\
  {":DRG:RATE",							SCPI_CMD_DRG_RATE_SET},\
  {":DRG:RATE?",						SCPI_CMD_DRG_RATE_GET},\
  {":DRG:WORK:MODE",					SCPI_CMD_DRG_WORKMODE_SET},\
  {":DRG:WORK:MODE?",					SCPI_CMD_DRG_WORKMODE_GET},\

  //SYSTEM
  {":SYSTem:RFOutput",                  	SCPI_CMD_SYSTEM_RFOUT_SET   },\
  {":SYSTem:RFOutput?",                  	SCPI_CMD_SYSTEM_RFOUT_GET   },\
  {":SYSTem:MODOutput",                  	SCPI_CMD_SYSTEM_MODOUT_SET  },\
  {":SYSTem:MODOutput?",                  	SCPI_CMD_SYSTEM_MODOUT_GET  },\
  {":SYSTem:LFOutput",                  	SCPI_CMD_LF_SWITCH_SET   },\
  {":SYSTem:LFOutput?",                  	SCPI_CMD_LF_SWITCH_GET   },\
  {":ALCFUNCTION",		                  	SCPI_CMD_SYSTEM_ALC_SET   },\
  {":ALCFUNCTION?",		                  	SCPI_CMD_SYSTEM_ALC_GET   },\
  {":MMEMory:LOAD:SCReen",					SCPI_CMD_MEMORY_LOAD_SCREEN}, \

  //Exref
//  {":FREQuency:CONVersion",                  SCPI_CMD_FREQ_CONVERSION_TYPE_SET},\
//  {":FREQuency:CONVersion?",                 SCPI_CMD_FREQ_CONVERSION_TYPE_GET},\
//  {":FREQuency:CONVersion:FREQuency",        SCPI_CMD_FREQ_CONVERSION_FREQ_SET},\
//  {":FREQuency:CONVersion:FREQuency?",       SCPI_CMD_FREQ_CONVERSION_FREQ_GET},\

  {":ROSCillator:SOURce",                    SCPI_CMD_FREQ_10MHZREF_SET},\
  {":ROSCillator:SOURce?",                   SCPI_CMD_FREQ_10MHZREF_GET},\

  {":DEVice:SELF:SERIalno",                                           SCPI_CMD_DEVICE_SERIALNO_SET},\
  {":DEVice:SELF:SERIalno?",                                          SCPI_CMD_DEVICE_SERIALNO_GET},\
  {":DEVice:SELF:TYPE",                                               SCPI_CMD_DEVICE_TYPE_SET},\
  {":DEVice:SELF:TYPE?",                                              SCPI_CMD_DEVICE_TYPE_GET},\
  {":DEVice:SELF:COMPany",                                            SCPI_CMD_DEVICE_COMPANY_SET},\
  {":DEVice:SELF:COMPany?",                                           SCPI_CMD_DEVICE_COMPANY_GET},\


  //从底层单片机上传上来的命令
  {":EXT:IF:AMPL", 													  SCPI_RCV_CMD_EXT_IF_AMPL},\
  {" :MODU:SERIAL:NUM",												  SCPI_RCV_SERIAL_NUM},\
  {":MODU:SOFT:VER",												  SCPI_RCV_MODU_SOFT_VER},\
  {":MODU:DEVI:TYPE",												  SCPI_RCV_MODU_DEV_TYPE},\


};

//解析数组
static struct _SCPI_keyboard parseCmd[MAX_GROUP];

//取出指令简写
static  _SCPI_keyboard abbreviationCmd[MAX_GROUP];

//指令命令
static _SCPI_keyboardCmd funcmd[MAX_GROUP];
static _SCPI_keyboardCmd funCmd;

//数值
static _SCPI_keyboardCmd valueCmd;

static unsigned long int cmdfunCode[MAX_GROUP];

#endif
