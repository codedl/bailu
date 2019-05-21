#ifndef COMMAND_H_
#define COMMAND_H_

#define MAX_GROUP                                  10
#define VALUE_MAX_GROUP                            32

//错误代码
#define SCPI_CMD_UNKNOWEN                          -1000  //未知

#define __SCPI_SUCCESS                              0     //SCPI成功
#define __SCPI_FAILED                               1     //失败
#define __SCPI_IGNORE                               2     //忽略
#define __SCPI_CANCEL                               3     //取消
#define __SCPI_UNSUPPORT                            4     //不支持
#define __SCPI_RETURN                               5     //返回
#define __SCPI_SERVICELOCK                          6     //系统服务上锁

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
#define SCPI_CMD_STANDARD_CLEAR                     0x1015
#define SCPI_CMD_STANDARD_ZCCAL                     0x1016

//自定义指令
#define SCPI_CMD_UPDATE_BEGIN                       0xF000
#define SCPI_CMD_UPDATE_FILE                        0xF001
#define SCPI_CMD_UPDATE_END                         0xF002
#define SCPI_CMD_UPDATE_CANCEL                      0xF003
#define SCPI_CMD_UPDATE_CRC                         0xF004

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
#define SCPI_CMD_FREQ_OFFSET_GET                    0x110B
#define SCPI_CMD_FREQ_REFERENCE_SET                 0x110C
#define SCPI_CMD_FREQ_REFERENCE_GET                 0x110D
#define SCPI_CMD_FREQ_SCALETYPE_SET					0x110E
#define SCPI_CMD_FREQ_SCALETYPE_GET					0x110F

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
#define SCPI_CMD_SWEEP_MODE_SET                     0x1604
#define SCPI_CMD_SWEEP_MODE_GET                     0x1605
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
#define SCPI_CMD_TRACE_OPER_12EX                    0x1903
#define SCPI_CMD_TRACE_OPER_2DL2                    0x1904
#define SCPI_CMD_TRACE_OPER_23EX                    0x1905
#define SCPI_CMD_TRACE_OPER_13                      0x1906
#define SCPI_CMD_TRACE_OPER_23                      0x1907
#define SCPI_CMD_SPECIAL_TRACE_754DATA_GET          0x1908


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
#define SCPI_CMD_MEMORY_LOAD_STATE					0x2211

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
#define SCPI_CMD_DEMOD_MODE_SET                     0x2504
#define SCPI_CMD_DEMOD_MODE_GET                     0x2505
#define SCPI_CMD_DEMOD_FREQ_SET               	    0x2506
#define SCPI_CMD_DEMOD_FREQ_GET               	    0x2507

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
#define SCPI_CMD_DEVICE_SERVICE_SET                 0x3012

#define SCPI_CMD_FILE_SEND_UPDATE_MAIN				0x300D
#define SCPI_CMD_FILE_SEND_UPDATE_SIGNLE			0x300E
#define SCPI_CMD_FILE_SEND_UPDATE_FILELIST			0x300F
#define SCPI_CMD_FILE_GET_GSINGLE					0x3010
#define SCPI_CMD_FILE_GET_GDATA						0x3011

//FFT指令
#define SCPI_CMD_OPTIONS_FFT_STATE_SET              0xB000
#define SCPI_CMD_OPTIONS_FFT_STATE_GET              0xB001
#define SCPI_CMD_OPTIONS_FFT_IDATA_GET              0xB002
#define SCPI_CMD_OPTIONS_FFT_QDATA_GET              0xB003
#define SCPI_CMD_OPTIONS_FFT_IQDATA_GET             0xB004
#define SCPI_CMD_OPTIONS_FFT_IQTESTDATA_GET         0xB005

//DEMOD指令
#define SCPI_CMD_OPTIONS_DEMOD_AM_STATE_SET         0xB020
#define SCPI_CMD_OPTIONS_DEMOD_AM_STATE_GET         0xB021
#define SCPI_CMD_OPTIONS_DEMOD_AM_DATA_GET          0xB022
#define SCPI_CMD_OPTIONS_DEMOD_AM_FFTDATA_GET       0xB023

//选件配置
#define SCPI_CMD_OPTIONS_ON_SET                     0xC000
#define SCPI_CMD_OPTIONS_OFF_SET                    0xC001
#define SCPI_CMD_OPTIONS_ONOFF_GET                  0xC002

//仪器配置
#define SCPI_CMD_DEVICE_ON_SET                      0xD000
#define SCPI_CMD_DEVICE_OFF_SET                     0xD001
#define SCPI_CMD_DEVICE_ONOFF_GET                   0xD002

//触发
#define SCPI_CMD_TRIG_MODE_SET                      0xE000
#define SCPI_CMD_TRIG_MODE_GET                      0xE001
#define SCPI_CMD_TRIG_MODE_VIDEO_POWER_SET          0xE003
#define SCPI_CMD_TRIG_MODE_VIDEO_POWER_GET          0xE004
//test
#define SCPI_CMD_TEST                               0xE005

#define	SCPI_CMD_FRONTEND_FREQ        				0x4000
#define	SCPI_CMD_FRONTEND_PREAMPT_ON  				0x4001
#define	SCPI_CMD_FRONTEND_PE43704     				0x4002
#define	SCPI_CMD_FRONTEND_HMC1018     				0x4003
#define SCPI_CMD_FRONTEND_IF_ATT      				0x4004
#define	SCPI_CMD_FRONTEND_AD5320                  	0x4005
#define SCPI_CMD_FRONTEND_REFSEL                  	0x4006
#define SCPI_CMD_GPMC_SET						  	0x4007
#define SCPI_CMD_GPMC_CLR						  	0x4008
#define SCPI_CMD_FRONTEND_HMC704				    0x4009
#define SCPI_CMD_READ_TEST							0x400a
#define SCPI_CMD_FRONTDET							0x400b
#define SCPI_CMD_DATA_START							0x400c
#define SCPI_CMD_DATA_SAVE							0x400d
#define SCPI_CMD_WRITE_TEST							0x400e
#define SCPI_CMD_UDP_REMOTE_IP_SET			        0x4100
#define SCPI_CMD_UDP_REMOTE_IP_GET			        0x4101
#define SCPI_CMD_UDP_REMOTE_PORT_SET		        0x4102
#define SCPI_CMD_UDP_REMOTE_PORT_GET		        0x4103
#define SCPI_CMD_FRONTEND_TEST                      0x4104

#define SCPI_CMD_POWERMETER_FREQ_SET	 	        0x4200
#define SCPI_CMD_POWERMETER_FREQ_GET		        0x4201
#define SCPI_CMD_POWERMETER_AMPT_GET		        0x4202
#define SCPI_CMD_CALAFREQ							0x4203
#define SCPI_CMD_CLEAR								0x4204
#define SCPI_CMD_TEMP								0x4205


  //检索返回值结构定义
struct resultDef
{
  unsigned long int cmdCode;  //指令
  QString value;              //value
  char auxValue;              //Marker index
  char auxValue2;             //trace index
  char auxValue3;             //demof freq index
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
  {"*CLEAR",														  SCPI_CMD_STANDARD_CLEAR},\
  {"*IDN?",															  SCPI_CMD_STANDARD_IDN_QUERY},\
  {"*RST",															  SCPI_CMD_STANDARD_RST},\
  {"*VER?",															  SCPI_CMD_STANDARD_VER},\
};
//:trace? trace1;
static struct _SCPI_keyboardCmd test[] = {
  //ROOT
  {"",						    									  0x00000000},\
  //CAL
  {":FACTCAL",													 	  SCPI_CMD_STANDARD_CAL},\
  {":ZCCAL",														  SCPI_CMD_STANDARD_ZCCAL},\
  {":FACTPRECAL",													  SCPI_CMD_STANDARD_PRECAL},\
  //FREQ
  {":[SENSe]:FREQuency:CENTer",	       								  SCPI_CMD_FREQ_CENTER_SET},\
  {":[SENSe]:FREQuency:CENTer?",	       							  SCPI_CMD_FREQ_CENTER_GET},\
  {":[SENSe]:FREQuency:STARt",						    			  SCPI_CMD_FREQ_START_SET},\
  {":[SENSe]:FREQuency:STARt?",						     			  SCPI_CMD_FREQ_START_GET},\
  {":[SENSe]:FREQuency:STOP",								 		  SCPI_CMD_FREQ_STOP_SET},\
  {":[SENSe]:FREQuency:STOP?",							 			  SCPI_CMD_FREQ_STOP_GET},\
  {":[SENSe]:FREQuency:CENTer:STEP",						 		  SCPI_CMD_FREQ_CFSTEP_SET},\
  {":[SENSe]:FREQuency:CENTer:STEP:[INCRement]",			 		  SCPI_CMD_FREQ_CFSTEP_SET},\
  {":[SENSe]:FREQuency:CENTer:STEP?", 					 			  SCPI_CMD_FREQ_CFSTEP_GET},\
  {":[SENSe]:FREQuency:CENTer:STEP:[INCRement]?", 		 			  SCPI_CMD_FREQ_CFSTEP_GET},\
  {":[SENSe]:FREQuency:CENTer:STEP:AUTO",						 	  SCPI_CMD_FREQ_CFSTEP_AUTO_SET},\
  {":[SENSe]:FREQuency:CENTer:STEP:AUTO?",						 	  SCPI_CMD_FREQ_CFSTEP_AUTO_GET},\
  {":DISPlay:WINdow:TRACe:X:[SCALe]:OFFSet",                          SCPI_CMD_FREQ_OFFSET_SET},\
  {":DISPlay:WINdow:TRACe:X:[SCALe]:OFFSet?",                         SCPI_CMD_FREQ_OFFSET_GET},\
  {":[SENSe]:FREQuency:REFerence",                                    SCPI_CMD_FREQ_REFERENCE_SET},\
  {":[SENSe]:FREQuency:REFerence?",                                   SCPI_CMD_FREQ_REFERENCE_GET},\
  {":DISPlay:WINdow:TRACe:X:[SCALe]:SPACing",                         SCPI_CMD_FREQ_SCALETYPE_SET},\
  {":DISPlay:WINdow:TRACe:X:[SCALe]:SPACing?",                  	    SCPI_CMD_FREQ_SCALETYPE_GET},\

  //SPAN
  {":[SENSe]:FREQuency:SPAN",                                         SCPI_CMD_SPAN_SET},\
  {":[SENSe]:FREQuency:SPAN?",                                        SCPI_CMD_SPAN_GET},\
  {":[SENSe]:FREQuency:SPAN:FULL",                                    SCPI_CMD_SPAN_FULL},\
  {":[SENSe]:FREQuency:SPAN:ZERO",                                    SCPI_CMD_SPAN_ZERO},\
  {":[SENSe]:FREQuency:SPAN:PREVious",                                SCPI_CMD_SPAN_PREVIOUS},\

  //AMPT
  {":DISPlay:WINdow:TRACe:Y:[SCALe]:RLEVel",                          SCPI_CMD_AMPT_REFLEVEL_SET},\
  {":DISPlay:WINdow:TRACe:Y:[SCALe]:RLEVel?",                         SCPI_CMD_AMPT_REFLEVEL_GET},\
  {":[SENSe]:POWer:[RF]:ATTenuation",                                 SCPI_CMD_AMPT_ATT_SET},\
  {":[SENSe]:POWer:[RF]:ATTenuation?",                                SCPI_CMD_AMPT_ATT_GET},\
  {":[SENSe]:POWer:[RF]:ATTenuation:AUTO",                            SCPI_CMD_AMPT_ATT_AUTO_SET},\
  {":[SENSe]:POWer:[RF]:ATTenuation:AUTO?",                           SCPI_CMD_AMPT_ATT_AUTO_GET},\
  {":DISPlay:WINdow:TRACe:Y:[SCALe]:PDIVision",                       SCPI_CMD_AMPT_SCALEDIV_SET},\
  {":DISPlay:WINdow:TRACe:Y:[SCALe]:PDIVision?",                      SCPI_CMD_AMPT_SCALEDIV_GET},\
  {":DISPlay:WINdow:TRACe:Y:[SCALe]:SPACing",                         SCPI_CMD_AMPT_SCALETYPE_SET},\
  {":DISPlay:WINdow:TRACe:Y:[SCALe]:SPACing?",                        SCPI_CMD_AMPT_SCALETYPE_GET},\
  {":DISPlay:WINdow:TRACe:Y:[SCALe]:RLEVel:OFFSet",                   SCPI_CMD_AMPT_REFLEVEL_OFFSET_SET},\
  {":DISPlay:WINdow:TRACe:Y:[SCALe]:RLEVel:OFFSet?",                  SCPI_CMD_AMPT_REFLEVEL_OFFSET_GET},\
  {":UNIT:POWer",                                                     SCPI_CMD_AMPT_REFUNIT_SET},\
  {":UNIT:POWer?",                                                    SCPI_CMD_AMPT_REFUNIT_GET},\

  {":[SENSe]:POWer:[RF]:GAIN:AUTO",                                   SCPI_CMD_AMPT_PREAMPT_SET},\
  {":[SENSe]:POWer:[RF]:GAIN:AUTO?",                                  SCPI_CMD_AMPT_PREAMPT_GET},\

  //TUNE
//  {":CALCulate:MARKer:TRCKing",                                       SCPI_CMD_TUNE_SET},\
//  {":CALCulate:MARKer:TRCKing:STATe",                                 SCPI_CMD_TUNE_SET},\
//  {":CALCulate:MARKer:TRCKing?",                                      SCPI_CMD_TUNE_GET},\
//  {":CALCulate:MARKer:TRCKing:STATe?",                                SCPI_CMD_TUNE_GET},
  {":CALCulate:TUNE:AUTO",                                            SCPI_CMD_TUNE_SET},\
  {":CALCulate:TUNE:AUTO?",                                           SCPI_CMD_TUNE_GET},\

  //BW
  {":[SENSe]:BANDwidth",                                              SCPI_CMD_BW_RBW_SET},\
  {":[SENSe]:BWIDth",                                                 SCPI_CMD_BW_RBW_SET},\
  {":[SENSe]:BANDwidth?",                                             SCPI_CMD_BW_RBW_GET},\
  {":[SENSe]:BWIDth?",                                                SCPI_CMD_BW_RBW_GET},\
  {":[SENSe]:BANDwidth:[RESolution]",                                 SCPI_CMD_BW_RBW_SET},\
  {":[SENSe]:BWIDth:[RESolution]",                                    SCPI_CMD_BW_RBW_SET},\
  {":[SENSe]:BANDwidth:[RESolution]?",                                SCPI_CMD_BW_RBW_GET},\
  {":[SENSe]:BWIDth:[RESolution]?",                                   SCPI_CMD_BW_RBW_GET},\
  {":[SENSe]:BANDwidth:[RESolution]:AUTO",                            SCPI_CMD_BW_RBW_AUTO_SET},\
  {":[SENSe]:BWIDth:[RESolution]:AUTO",                               SCPI_CMD_BW_RBW_AUTO_SET},\
  {":[SENSe]:BANDwidth:[RESolution]:AUTO?",                           SCPI_CMD_BW_RBW_AUTO_GET},\
  {":[SENSe]:BWIDth:[RESolution]:AUTO?",                              SCPI_CMD_BW_RBW_AUTO_GET},\
  {":[SENSe]:BANDwidth:[RESolution]:STEP:MODE",                       SCPI_CMD_BW_RBW_STEP_MODE_SET},\
  {":[SENSe]:BWIDth:[RESolution]:STEP:MODE",                          SCPI_CMD_BW_RBW_STEP_MODE_SET},\
  {":[SENSe]:BANDwidth:[RESolution]:STEP:MODE?",                      SCPI_CMD_BW_RBW_STEP_MODE_GET},\
  {":[SENSe]:BWIDth:[RESolution]:STEP:MODE?",                         SCPI_CMD_BW_RBW_STEP_MODE_GET},\
  {":[SENSe]:BANDwidth:VIDeo",                                        SCPI_CMD_BW_VBW_SET},\
  {":[SENSe]:BWIDth:VIDeo",                                           SCPI_CMD_BW_VBW_SET},\
  {":[SENSe]:BANDwidth:VIDeo?",                                       SCPI_CMD_BW_VBW_GET},\
  {":[SENSe]:BWIDth:VIDeo?",                                          SCPI_CMD_BW_VBW_GET},\
  {":[SENSe]:BANDwidth:VIDeo:AUTO",                                   SCPI_CMD_BW_VBW_AUTO_SET},\
  {":[SENSe]:BWIDth:VIDeo:AUTO",                                      SCPI_CMD_BW_VBW_AUTO_SET},\
  {":[SENSe]:BANDwidth:VIDeo:AUTO?",                                  SCPI_CMD_BW_VBW_AUTO_GET},\
  {":[SENSe]:BWIDth:VIDeo:AUTO?",                                     SCPI_CMD_BW_VBW_AUTO_GET},\
  {":[SENSe]:AVERage",                                                SCPI_CMD_BW_AVERAGE_STATE_SET},\
  {":[SENSe]:AVERage:[STATe]",                                        SCPI_CMD_BW_AVERAGE_STATE_SET},\
  {":[SENSe]:AVERage?",                                               SCPI_CMD_BW_AVERAGE_STATE_GET},\
  {":[SENSe]:AVERage:[STATe]?",                                       SCPI_CMD_BW_AVERAGE_STATE_GET},\
  {":[SENSe]:AVERage:COUNt",                                          SCPI_CMD_BW_AVERAGE_COUNT_SET},\
  {":[SENSe]:AVERage:COUNt?",                                         SCPI_CMD_BW_AVERAGE_COUNT_GET},\

  //EMI BW
  {":[SENSe]:BANDwidth:EMC",						 				  										SCPI_CMD_BW_EMC_SET},\
  {":[SENSe]:BWIDth:EMC",						 					  											SCPI_CMD_BW_EMC_SET},\
  {":[SENSe]:BANDwidth:EMC?", 					 			          							SCPI_CMD_BW_EMC_GET},\
  {":[SENSe]:BWIDth:EMC?",   					 			          								SCPI_CMD_BW_EMC_GET},\
  {":[SENSe]:BANDwidth:EMC:STATe",				 			          						SCPI_CMD_BW_EMC_STATE_SET},\
  {":[SENSe]:BWIDth:EMC:STATe",				 						  									SCPI_CMD_BW_EMC_STATE_SET},\
  {":[SENSe]:BANDwidth:EMC:STATe?",				 			          						SCPI_CMD_BW_EMC_STATE_GET},\
  {":[SENSe]:BWIDth:EMC:STATe?",                                      SCPI_CMD_BW_EMC_STATE_GET},\

  //SWEEP
  {":[SENSe]:SWEep:TIME",                                             SCPI_CMD_SWEEP_TIME_SET},\
  {":[SENSe]:SWEep:TIME?",                                            SCPI_CMD_SWEEP_TIME_GET},\
  {":[SENSe]:SWEep:TIME:AUTO",                                        SCPI_CMD_SWEEP_TIME_AUTO_SET},\
  {":[SENSe]:SWEep:TIME:AUTO?",                                       SCPI_CMD_SWEEP_TIME_AUTO_GET},\
  {":INITiate:CONTinuous",                                            SCPI_CMD_SWEEP_MODE_SET},\
  {":INITiate:CONTinuous?",                                           SCPI_CMD_SWEEP_MODE_GET},\
  {":[SENSe]:SWEep:POINts",                                           SCPI_CMD_SWEEP_POINTS_SET},\
  {":[SENSe]:SWEep:POINts?",                                          SCPI_CMD_SWEEP_POINTS_GET},\

  //TRACE
  {":TRACe?",                                                         SCPI_CMD_TRACE_DATA_GET},\
  {":TRACe:[DATA]?",                                                  SCPI_CMD_TRACE_DATA_GET},\
  //socket
  {":TRACe:SOCKdata?",					 							  											SCPI_CMD_SPECIAL_TRACE_754DATA_GET},\
  {":TRACe:MODE",                                                     SCPI_CMD_TRACE_MODE_SET},\
  {":TRACe:MODE?",                                                    SCPI_CMD_TRACE_MODE_GET},\


  //DETECTOR
  {":[SENSe]:DETector",                                               SCPI_CMD_DETECTOR_SET},\
  {":[SENSe]:DETector:[FUNCtion]",                                    SCPI_CMD_DETECTOR_SET},\
  {":[SENSe]:DETector?",                                              SCPI_CMD_DETECTOR_GET},\
  {":[SENSe]:DETector:[FUNCtion]?",                                   SCPI_CMD_DETECTOR_GET},\

  //DISPLAY
  {":DISPlay:MENU:STATe",                                             SCPI_CMD_DISPLAY_FULLSCREEN_SET},\
  {":DISPlay:MENU:STATe?",                                            SCPI_CMD_DISPLAY_FULLSCREEN_GET},\
  {":DISPlay:FORMat:ZOOM",                                            SCPI_CMD_DISPLAY_FORMAT_ZOOM_SET},\
  {":DISPlay:FORMat:ZOOM?",                                           SCPI_CMD_DISPLAY_FORMAT_ZOOM_GET},\
  {":DISPlay:WINdow:TRACe:Y:DLINe",                                   SCPI_CMD_DISPLAY_LINE_Y_SET},\
  {":DISPlay:WINdow:TRACe:Y:DLINe?",                                  SCPI_CMD_DISPLAY_LINE_Y_GET},\
  {":DISPlay:WINdow:TRACe:Y:DLINe:STATe",                             SCPI_CMD_DISPLAY_LINE_Y_STATE_SET},\
  {":DISPlay:WINdow:TRACe:Y:DLINe:STATe?",                            SCPI_CMD_DISPLAY_LINE_Y_STATE_GET},\
  {":DISPlay:WINdow:TRACe:Y:[SCALe]:GAUge",                           SCPI_CMD_DISPLAY_Y_GAUGE_STATE_SET},\
  {":DISPlay:WINdow:TRACe:Y:[SCALe]:GAUge?",                          SCPI_CMD_DISPLAY_Y_GAUGE_STATE_GET},\
  {":DISPlay:WINdow:GRID",                                            SCPI_CMD_DISPLAY_GRID_STATE_SET},\
  {":DISPlay:WINdow:GRID?",                                           SCPI_CMD_DISPLAY_GRID_STATE_GET},\
  {":DISPlay:WINdow:LABEl",                                           SCPI_CMD_DISPLAY_LABEL_STATE_SET},\
  {":DISPlay:WINdow:LABEl?",                                          SCPI_CMD_DISPLAY_LABEL_STATE_GET},\
  {":DISPlay:SKIN",                                                   SCPI_CMD_DISPLAY_SKIN_SET},\
  {":DISPlay:SKIN?",                                                  SCPI_CMD_DISPLAY_SKIN_GET},\

  //MEAS
  {":CONFigure:SATime",                                               SCPI_CMD_MEAS_SET_ST},\
  {":CONFigure:ACPower",                                              SCPI_CMD_MEAS_SET_ACP},\
  {":CONFigure:CHPower",                                              SCPI_CMD_MEAS_SET_CHP},\
  {":CONFigure:OBWidth",                                              SCPI_CMD_MEAS_SET_OBW},\
  {":CONFigure:DISC",                                                 SCPI_CMD_MEAS_SET_DISC},\
  {":CONFigure:EMC",                                                  SCPI_CMD_MEAS_SET_EMC},\
  {":CONFigure:SANalyzer",                                            SCPI_CMD_MEAS_SET_SA},\
  {":CONFigure?",                                                     SCPI_CMD_MEAS_GET},\

  //MEASSETUP
  {":[SENSe]:ACPower:BANDwidth:INTegration",                          SCPI_CMD_MEASSETUP_ACP_BANDWIDTH_INT_SET},\
  {":[SENSe]:ACPower:BANDwidth:INTegration?",                         SCPI_CMD_MEASSETUP_ACP_BANDWIDTH_INT_GET},\
  {":[SENSe]:ACPower:BANDwidth:ACHannel:COUNt",                       SCPI_CMD_MEASSETUP_ACP_CHANNEL_COUNT_SET},\
  {":[SENSe]:ACPower:BANDwidth:ACHannel:COUNt?",                      SCPI_CMD_MEASSETUP_ACP_CHANNEL_COUNT_GET},\
  {":[SENSe]:ACPower:CSPacing",                                       SCPI_CMD_MEASSETUP_ACP_SPACE_SET},\
  {":[SENSe]:ACPower:CSPacing?",                                      SCPI_CMD_MEASSETUP_ACP_SPACE_GET},\
  {":[SENSe]:CHPower:FREQuency:SPAN",                                 SCPI_CMD_MEASSETUP_CHPOWER_FREQ_SPAN_SET},\
  {":[SENSe]:CHPower:FREQuency:SPAN?",                                SCPI_CMD_MEASSETUP_CHPOWER_FREQ_SPAN_GET},\
  {":[SENSe]:OBWidth:PERCent",                                        SCPI_CMD_MEASSETUP_OBW_PERCENT_SET},\
  {":[SENSe]:OBWidth:PERCent?",                                       SCPI_CMD_MEASSETUP_OBW_PERCENT_GET},\
  {":[SENSe]:OBWidth:FREQuency:SPAN",                                 SCPI_CMD_MEASSETUP_OBW_FREQ_SPAN_SET},\
  {":[SENSe]:OBWidth:FREQuency:SPAN?",                                SCPI_CMD_MEASSETUP_OBW_FREQ_SPAN_GET},\
  {":FETCh:ACPower:MAIN?",                                            SCPI_CMD_MEASSETUP_FETCH_ACP_MAIN},\
  {":FETCh:ACPower?",                                                 SCPI_CMD_MEASSETUP_FETCH_ACP_LIST_GET},\
  {":FETCh:ACPower:LOWer?",                                           SCPI_CMD_MEASSETUP_FETCH_ACP_LOWER},\
  {":FETCh:ACPower:UPPer?",                                           SCPI_CMD_MEASSETUP_FETCH_ACP_UPPER},\
  {":FETCh:CHPower:POWer?",                                           SCPI_CMD_MEASSETUP_FETCH_CHPOWER_POWER_GET},\
  {":FETCh:OBWidth:BANDwidth?",                                       SCPI_CMD_MEASSETUP_FETCH_OBW_BAND_GET},\

  //MARKER
  {":CALCulate:MARKer:AOFF",                                          SCPI_CMD_MARKER_ALLOFF},\
  {":CALCulate:MARKer:STATe",                                         SCPI_CMD_MARKER_STATE_SET},\
  {":CALCulate:MARKer:STATe?",                                        SCPI_CMD_MARKER_STATE_GET},\
  {":CALCulate:MARKer:TRACe?",                                        SCPI_CMD_MARKER_TRACE_GET},\
  {":CALCulate:MARKer:TRACe",                                         SCPI_CMD_MARKER_TRACE_SET},\
  {":CALCulate:MARKer:MODE",                                          SCPI_CMD_MARKER_MODE_SET},\
  {":CALCulate:MARKer:MODE?",                                         SCPI_CMD_MARKER_MODE_GET},\
  {":CALCulate:MARKer:TABLe:STATe",                                   SCPI_CMD_MARKER_TABLE_STATE_SET},\
  {":CALCulate:MARKer:TABLe:STATe?",                                  SCPI_CMD_MARKER_TABLE_STATE_GET},\
  {":CALCulate:MARKer:X",                                             SCPI_CMD_MARKER_X_SET},\
  {":CALCulate:MARKer:X?",                                            SCPI_CMD_MARKER_X_GET},\
  {":CALCulate:MARKer:Y?",                                            SCPI_CMD_MARKER_Y_GET},\

  //MARKER FUNCTION
  {":CALCulate:MARKer:FUNCtion:AOFF",                                 SCPI_CMD_MARKER_FCTN_ALLOFF},\
  {":CALCulate:MARKer:FCOunt",                                        SCPI_CMD_MARKER_FCTN_FREQCOUNT_STATE_SET},\
  {":CALCulate:MARKer:FCOunt:[STATe]",                                SCPI_CMD_MARKER_FCTN_FREQCOUNT_STATE_SET},\
  {":CALCulate:MARKer:FCOunt?",                                       SCPI_CMD_MARKER_FCTN_FREQCOUNT_STATE_GET},\
  {":CALCulate:MARKer:FCOunt:[STATe]?",                               SCPI_CMD_MARKER_FCTN_FREQCOUNT_STATE_GET},\
  {":CALCulate:MARKer:FCOunt:X?",                                     SCPI_CMD_MARKER_FCTN_FREQCOUNT_X_GET},
  {":CALCulate:MARKer:FCOunt:RESolution",                             SCPI_CMD_MARKER_FCTN_FREQCOUNT_RBW_SET},
  {":CALCulate:MARKer:FCOunt:RESolution?",                            SCPI_CMD_MARKER_FCTN_FREQCOUNT_RBW_GET},
  {":CALCulate:MARKer:PHNoise",                                       SCPI_CMD_MARKER_FCTN_NOISE_STATE_SET},\
  {":CALCulate:MARKer:PHNoise:[STATe]",                               SCPI_CMD_MARKER_FCTN_NOISE_STATE_SET},\
  {":CALCulate:MARKer:PHNoise?",                                      SCPI_CMD_MARKER_FCTN_NOISE_STATE_GET},\
  {":CALCulate:MARKer:PHNoise:[STATe]?",                              SCPI_CMD_MARKER_FCTN_NOISE_STATE_GET},\
  {":CALCulate:MARKer:PHNoise:Y?",                                    SCPI_CMD_MARKER_FCTN_NOISE_Y_GET},\
  {":CALCulate:MARKer:PHNoise:OFFSet",                                SCPI_CMD_MARKER_FCTN_NOISE_SPAN_SET},\
  {":CALCulate:MARKer:PHNoise:OFFSet?",                               SCPI_CMD_MARKER_FCTN_NOISE_SPAN_GET},\
  {":CALCulate:MARKer:PHNoise:OFFSet:FREQuency",                      SCPI_CMD_MARKER_FCTN_NOISE_SFREQ_SET},\
  {":CALCulate:MARKer:PHNoise:OFFSet:FREQuency?",                     SCPI_CMD_MARKER_FCTN_NOISE_SFREQ_GET},\
  {":CALCulate:BWIDth",                                               SCPI_CMD_MARKER_FCTN_NDB_STATE_SET},\
  {":CALCulate:BWIDth:[STATe]",                                       SCPI_CMD_MARKER_FCTN_NDB_STATE_SET},\
  {":CALCulate:BANDwidth",                                            SCPI_CMD_MARKER_FCTN_NDB_STATE_SET},\
  {":CALCulate:BANDwidth:[STATe]",                                    SCPI_CMD_MARKER_FCTN_NDB_STATE_SET},\
  {":CALCulate:BWIDth?",                                              SCPI_CMD_MARKER_FCTN_NDB_STATE_GET},\
  {":CALCulate:BWIDth:[STATe]?",                                      SCPI_CMD_MARKER_FCTN_NDB_STATE_GET},\
  {":CALCulate:BANDwidth?",                                           SCPI_CMD_MARKER_FCTN_NDB_STATE_GET},\
  {":CALCulate:BANDwidth:[STATe]?",                                   SCPI_CMD_MARKER_FCTN_NDB_STATE_GET},\
  {":CALCulate:BWIDth:NDB",                                           SCPI_CMD_MARKER_FCTN_NDB_NDB_SET},\
  {":CALCulate:BANDwidth:NDB",                                        SCPI_CMD_MARKER_FCTN_NDB_NDB_SET},\
  {":CALCulate:BWIDth:NDB?",                                          SCPI_CMD_MARKER_FCTN_NDB_NDB_GET},\
  {":CALCulate:BANDwidth:NDB?",                                       SCPI_CMD_MARKER_FCTN_NDB_NDB_GET},\
  {":CALCulate:BWIDth:RESult?",                                       SCPI_CMD_MARKER_FCTN_NDB_RESULT_GET},\
  {":CALCulate:BANDwidth:RESult?",                                    SCPI_CMD_MARKER_FCTN_NDB_RESULT_GET},\

  //MARKERTO
  {":CALCulate:MARKer:[SET]:CENTer",                                  SCPI_CMD_MARKER_TO_CENTER},\
  {":CALCulate:MARKer:[SET]:STEP",                                    SCPI_CMD_MARKER_TO_STEP},\
  {":CALCulate:MARKer:[SET]:STARt",                                   SCPI_CMD_MARKER_TO_START},\
  {":CALCulate:MARKer:[SET]:STOP",                                    SCPI_CMD_MARKER_TO_STOP},\
  {":CALCulate:MARKer:[SET]:RLEVel",                                  SCPI_CMD_MARKER_TO_REFLEVEL},\
  {":CALCulate:MARKer:DELTa:[SET]:SPAN",                              SCPI_CMD_MARKER_DELTA_TO_SPAN},\
  {":CALCulate:MARKer:DELTa:[SET]:CENTer",                            SCPI_CMD_MARKER_DELTA_TO_CENTER},\

  //PEAK
  {":CALCulate:MARKer:MAXimum",                                       SCPI_CMD_MARKER_PEAK_MAXIMUM},\
  {":CALCulate:MARKer:MAXimum:LEFT",                                  SCPI_CMD_MARKER_PEAK_MAXIMUM_LEFT},\
  {":CALCulate:MARKer:MAXimum:RIGHt",                                 SCPI_CMD_MARKER_PEAK_MAXIMUM_RIGHT},\
  {":CALCulate:MARKer:MAXimum:NEXT",                                  SCPI_CMD_MARKER_PEAK_MAXIMUM_NEXT},\
  {":CALCulate:MARKer:MINimum",                                       SCPI_CMD_MARKER_PEAK_MINIMUM},\
  {":CALCulate:MARKer:CPEak",                                         SCPI_CMD_MARKER_PEAK_CONTINUOUS_SET},\
  {":CALCulate:MARKer:CPEak:[STATe]",                                 SCPI_CMD_MARKER_PEAK_CONTINUOUS_SET},\
  {":CALCulate:MARKer:CPEak?",                                        SCPI_CMD_MARKER_PEAK_CONTINUOUS_GET},\
  {":CALCulate:MARKer:CPEak:[STATe]?",                                SCPI_CMD_MARKER_PEAK_CONTINUOUS_GET},\

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
  {":DISPlay:ANNotation:CLOCk:[STATe]",                               SCPI_CMD_SYSTEM_CLOCK_STATE_SET},\
  {":DISPlay:ANNotation:CLOCk?",                                      SCPI_CMD_SYSTEM_CLOCK_STATE_GET},\
  {":DISPlay:ANNotation:CLOCk:[STATe]?",                              SCPI_CMD_SYSTEM_CLOCK_STATE_GET},\
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
  {":SYSTem:COMMunicate:LAN:IP:ADDRess",                              SCPI_CMD_SYSTEM_COMMUNICATE_LAN_IP_SET},\
  {":SYSTem:COMMunicate:LAN:IP:ADDRess?",                    	      SCPI_CMD_SYSTEM_COMMUNICATE_LAN_IP_GET},\
  {":SYSTem:COMMunicate:LAN:MASK",                                    SCPI_CMD_SYSTEM_COMMUNICATE_LAN_MASK_SET},\
  {":SYSTem:COMMunicate:LAN:MASK?",                                   SCPI_CMD_SYSTEM_COMMUNICATE_LAN_MASK_GET},\
  {":SYSTem:COMMunicate:LAN:GATE",                                    SCPI_CMD_SYSTEM_COMMUNICATE_LAN_GATE_SET},\
  {":SYSTem:COMMunicate:LAN:GATE?",                                   SCPI_CMD_SYSTEM_COMMUNICATE_LAN_GATE_GET},\

  //补偿
  {":CALibration:NINE",                                               SCPI_CMD_STANDARD_CAL_9KHZ_SET},\
  {":CALibration:NINE?",                                              SCPI_CMD_STANDARD_CAL_9KHZ_GET},\
  {":CALibration:TWENTY",                                             SCPI_CMD_STANDARD_CAL_20KHZ_SET},\
  {":CALibration:TWENTY?",                                            SCPI_CMD_STANDARD_CAL_20KHZ_GET},\

  //file && save
  {":MMEMory:CATalog?",                                               SCPI_CMD_MEMORY_CATALOG},\
  {":MMEMory:DISK:INFormation?",                                      SCPI_CMD_MEMORY_DISK_INFORMATION},\
  {":MMEMory:STORe:STATe",                                            SCPI_CMD_MEMORY_STORE_STATE},\
  {":MMEMory:STORe:TRACe",                                            SCPI_CMD_MEMORY_STORE_TRACE},\
  {":MMEMory:STORe:SCReen",                                           SCPI_CMD_MEMORY_STORE_SCREEN},\
  {":MMEMory:LOAD:STATe",                                             SCPI_CMD_MEMORY_LOAD_STATE},\
  {":MMEMory:LOAD:TRACe",                                             SCPI_CMD_MEMORY_LOAD_TRACE},\
  {":MMEMory:LOAD:SCReen",                                            SCPI_CMD_MEMORY_LOAD_SCREEN},\
  {":MMEMory:DELete:TRACe",                                           SCPI_CMD_MEMORY_DELETE_TRACE},\
  {":MMEMory:DELete:SCReen",                                          SCPI_CMD_MEMORY_DELETE_SCREEN},\
  {":MMEMory:DELete:TRACe:ALL",                                       SCPI_CMD_MEMORY_DELETE_TRACE_ALL},\
  {":MMEMory:DELete:SCReen:ALL",                                      SCPI_CMD_MEMORY_DELETE_SCREEN_ALL},\
  {":MMEMory:DELete:ALL",                                             SCPI_CMD_MEMORY_DELETE_ALL},\
  {":MMEMory:COPY:ALL",                                               SCPI_CMD_MEMORY_COPY_ALL},\
  {":MMEMory:COPY:TRACe",                                             SCPI_CMD_MEMORY_COPY_TRACE},\
  {":MMEMory:COPY:TRACe:ALL",                                         SCPI_CMD_MEMORY_COPY_TRACE_ALL},\
  {":MMEMory:COPY:SCReen",                                            SCPI_CMD_MEMORY_COPY_SCREEN},\
  {":MMEMory:COPY:SCReen:ALL",                                        SCPI_CMD_MEMORY_COPY_SCREEN_ALL},\

  //source
  {":OUTPut",                                                         SCPI_CMD_SOURCE_SOURCEGEN_STATE_SET},\
  {":OUTPut?",                                                        SCPI_CMD_SOURCE_SOURCEGEN_STATE_GET},\
  {":OUTPut:[STATe]",                                                 SCPI_CMD_SOURCE_SOURCEGEN_STATE_SET},\
  {":OUTPut:[STATe]?",                                                SCPI_CMD_SOURCE_SOURCEGEN_STATE_GET},\
  {":SOURce:OUTPut:SIGNal",                                           SCPI_CMD_SOURCE_SOURCEGEN_STATE_SET},\
  {":SOURce:OUTPut:SIGNal?",                                          SCPI_CMD_SOURCE_SOURCEGEN_STATE_GET},\
  {":SOURce:OUTPut:SIGNal:[STATe]",                                   SCPI_CMD_SOURCE_SOURCEGEN_STATE_SET},\
  {":SOURce:OUTPut:SIGNal:[STATe]?",                                  SCPI_CMD_SOURCE_SOURCEGEN_STATE_GET},\

  {":OUTPut:POWer",                                                   SCPI_CMD_SOURCE_SOURCEGEN_POWER_SET},\
  {":OUTPut:POWer?",                                                  SCPI_CMD_SOURCE_SOURCEGEN_POWER_GET},\
  {":SOURce:OUTPut:POWer",                                            SCPI_CMD_SOURCE_SOURCEGEN_POWER_SET},\
  {":SOURce:OUTPut:POWer?",                                           SCPI_CMD_SOURCE_SOURCEGEN_POWER_GET},\

  {":OUTPut:FREQuency",                                               SCPI_CMD_SOURCE_SOURCEGEN_FREQUENCY_SET},\
  {":OUTPut:FREQuency?",                                              SCPI_CMD_SOURCE_SOURCEGEN_FREQUENCY_GET},\
  {":SOURce:FREQuency",                                               SCPI_CMD_SOURCE_SOURCEGEN_FREQUENCY_SET},\
  {":SOURce:FREQuency?",                                              SCPI_CMD_SOURCE_SOURCEGEN_FREQUENCY_GET},\

  //Gen
  {":OUTPut:TRACk",                                                   SCPI_CMD_SOURCE_TRACKGEN_STATE_SET},\
  {":OUTPut:TRACk?",                                                  SCPI_CMD_SOURCE_TRACKGEN_STATE_GET},\
  {":OUTPut:TRACk:[STATe]",                                           SCPI_CMD_SOURCE_TRACKGEN_STATE_SET},\
  {":OUTPut:TRACk:[STATe]?",                                          SCPI_CMD_SOURCE_TRACKGEN_STATE_GET},\
  {":SOURce:OUTPut:TRACk",                                            SCPI_CMD_SOURCE_TRACKGEN_STATE_SET},\
  {":SOURce:OUTPut:TRACk?",                                           SCPI_CMD_SOURCE_TRACKGEN_STATE_GET},\
  {":SOURce:OUTPut:TRACk:[STATe]",                                    SCPI_CMD_SOURCE_TRACKGEN_STATE_SET},\
  {":SOURce:OUTPut:TRACk:[STATe]?",                                   SCPI_CMD_SOURCE_TRACKGEN_STATE_GET},\

  {":SOURce:POWer:TRACk",                                             SCPI_CMD_SOURCE_TRACKGEN_POWER_SET},\
  {":SOURce:POWer:TRACk?",                                            SCPI_CMD_SOURCE_TRACKGEN_POWER_GET},\
  {":SOURce:POWer:TRACk:[POWer]",                                     SCPI_CMD_SOURCE_TRACKGEN_POWER_SET},\
  {":SOURce:POWer:TRACk:[POWer]?",                                    SCPI_CMD_SOURCE_TRACKGEN_POWER_GET},\

  {":CALCulate:NETMeasure",                                           SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_STATE_SET},\
  {":CALCulate:NETMeasure:[STATe]",                                   SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_STATE_SET},\
  {":CALCulate:NETMeasure?",                                          SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_STATE_GET},\
  {":CALCulate:NETMeasure:[STATe]?",                                  SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_STATE_GET},\
  {":CALCulate:NTData:NORMalize",                                     SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_NORMALIZE},\
  {":CALCulate:NETMeasure:RLEVel",                                    SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_RLEVEL_SET},\
  {":CALCulate:NETMeasure:RLEVel?",                                   SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_RLEVEL_GET},\
  {":CALCulate:NETMeasure:POWer",                                     SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_POWER_SET},\
  {":CALCulate:NETMeasure:POWer?",                                    SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_POWER_GET},\

  //print
  {":HCOPy:IMAGe:COLor",                                              SCPI_CMD_PRINT_IMAGE_COLOR_SET},\
  {":HCOPy:IMAGe:COLor:[STATe]",                                      SCPI_CMD_PRINT_IMAGE_COLOR_SET},\
  {":HCOPy:IMAGe:COLor?",                                             SCPI_CMD_PRINT_IMAGE_COLOR_GET},\
  {":HCOPy:IMAGe:COLor:[STATe]?",                                     SCPI_CMD_PRINT_IMAGE_COLOR_GET},\
  {":HCOPy:SCReen",                                                   SCPI_CMD_PRINT_IMAGE_SCREEN},\
  {":HCOPy:TRACe",                                                    SCPI_CMD_PRINT_IMAGE_TRACE},\
  {":HCOPy:PAGE:ORIentation",                                         SCPI_CMD_PRINT_IMAGE_ORIENTATION_SET},\
  {":HCOPy:PAGE:ORIentation?",                                        SCPI_CMD_PRINT_IMAGE_ORIENTATION_GET},\
  {":HCOPy:PAGE:SIZE",                                                SCPI_CMD_PRINT_IMAGE_SIZE_SET},\
  {":HCOPy:PAGE:SIZE?",                                               SCPI_CMD_PRINT_IMAGE_SIZE_GET},\
  {":HCOPy:PAGE:PRINts",                                              SCPI_CMD_PRINT_IMAGE_NUMS_SET},\
  {":HCOPy:PAGE:PRINts?",                                             SCPI_CMD_PRINT_IMAGE_NUMS_GET},\

  //demod
  {":[SENSe]:DEMod:STATe",                                            SCPI_CMD_DEMOD_STATE_SET},\
  {":[SENSe]:DEMod:STATe?",                                           SCPI_CMD_DEMOD_STATE_GET},\
  {":SYSTem:SPEaker:VOLume",                                          SCPI_CMD_DEMOD_SOUND_SET},\
  {":SYSTem:SPEaker:VOLume?",                                         SCPI_CMD_DEMOD_SOUND_GET},\
  {":[SENSe]:DEMod:MODE",                                             SCPI_CMD_DEMOD_MODE_SET},\
  {":[SENSe]:DEMod:MODE?",                                            SCPI_CMD_DEMOD_MODE_GET},\
  {":[SENSe]:DEMod:FREQuency",                                        SCPI_CMD_DEMOD_FREQ_SET},\
  {":[SENSe]:DEMod:FREQuency?",                                       SCPI_CMD_DEMOD_FREQ_GET},\

  //reg
  {":REGister",                                                       SCPI_CMD_STANDARD_REG},\

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
  {":DEVice:SELF:SERIalno",                                           SCPI_CMD_DEVICE_SERIALNO_SET},\
  {":DEVice:SELF:SERIalno?",                                          SCPI_CMD_DEVICE_SERIALNO_GET},\
  {":DEVice:SELF:MAC",                                                SCPI_CMD_DEVICE_MAC_SET},\
  {":DEVice:SELF:MAC?",                                               SCPI_CMD_DEVICE_MAC_GET},\
  {":DEVice:SELF:TYPE",                                               SCPI_CMD_DEVICE_TYPE_SET},\
  {":DEVice:SELF:TYPE?",                                              SCPI_CMD_DEVICE_TYPE_GET},\
  {":DEVice:SELF:EVA?",                                               SCPI_CMD_DEVICE_STATE_GET},\
  {":DEVice:SELF:EVAluation",                                         SCPI_CMD_DEVICE_STATE_EVA_SET},\
  {":DEVice:SELF:Full",                                               SCPI_CMD_DEVICE_STATE_FULL_SET},\
  {":DEVice:SELF:SERVice",                                            SCPI_CMD_DEVICE_SERVICE_SET},\

  {":FILE:GET:SINGLE",												  											SCPI_CMD_FILE_GET_GSINGLE},\
  {":FILE:GET:GDATA",												  												SCPI_CMD_FILE_GET_GDATA},\
  {":FILE:SEND:SINGLE",									  		      									SCPI_CMD_FILE_SEND_UPDATE_SIGNLE},\
  {":FILE:SEND:UPDATE:MAIN",									 	  										SCPI_CMD_FILE_SEND_UPDATE_MAIN},\
  {":FILE:SEND:UPDATE:LIST",									  	  									SCPI_CMD_FILE_SEND_UPDATE_FILELIST},\

  //option
  {":FFT:DATA?",   					 			                      							SCPI_CMD_OPTIONS_FFT_IQDATA_GET},\
  {":FFT:TESTDATA?",   					 			                  							SCPI_CMD_OPTIONS_FFT_IQTESTDATA_GET},\

  {":OPTion:ON",   					 			                    SCPI_CMD_OPTIONS_ON_SET},\
  {":OPTion:OFF",   					 			                  SCPI_CMD_OPTIONS_OFF_SET},\
  {":OPTion:STATe?",   					 			                SCPI_CMD_OPTIONS_ONOFF_GET},\

  //器件配置
  {":DEVice:SELF:ON",   					 			              SCPI_CMD_DEVICE_ON_SET},\
  {":DEVice:SELF:OFF",   					 			              SCPI_CMD_DEVICE_OFF_SET},\
  {":DEVice:SELF:STATe?",   					 			          SCPI_CMD_DEVICE_ONOFF_GET},\

  //触发控制
  {":TRIG:MODE",   	     				 			                SCPI_CMD_TRIG_MODE_SET},\
  {":TRIG:MODE?",   					 			                  SCPI_CMD_TRIG_MODE_GET},\
  {":TRIG:MODE:VIDeo",   					 			              SCPI_CMD_TRIG_MODE_VIDEO_POWER_SET},\
  {":TRIG:MODE:VIDeo?",   					 			            SCPI_CMD_TRIG_MODE_VIDEO_POWER_GET},\

  {":TEST?",                                          SCPI_CMD_TEST},\

  //射频前端
  {":FRON:TEST",																			SCPI_CMD_FRONTEND_TEST},\
  {":FROntend:FREQ", 															    SCPI_CMD_FRONTEND_FREQ},\
  {":FROntend:PREAmpt:ON",														SCPI_CMD_FRONTEND_PREAMPT_ON},\
  {":FROntend:PE43704", 															SCPI_CMD_FRONTEND_PE43704},\
  {":FROntend:HMC1018",																SCPI_CMD_FRONTEND_HMC1018},\
  {":FROntend:IF:ATT",																SCPI_CMD_FRONTEND_IF_ATT},\
  {":FROntend:REFVolt",																SCPI_CMD_FRONTEND_AD5320},\
  {":FROntend:REFSEL",																SCPI_CMD_FRONTEND_REFSEL},\
  {":GPMC:TEST:SET",																	SCPI_CMD_GPMC_SET},\
  {":GPMC:TEST:CLR",																	SCPI_CMD_GPMC_CLR},\
  {":FROntend:TEST:HMC704",														SCPI_CMD_FRONTEND_HMC704},\
  {":FPGA:TEST:READ", 																SCPI_CMD_READ_TEST},\
  {":IFCONTROL:DATA:START",														SCPI_CMD_DATA_START},\
  {":IFCONTROL:DATA:SAVE",														SCPI_CMD_DATA_SAVE},\
  {":FTOntend:INTR:DET",															SCPI_CMD_FRONTDET},\
  {":FPGA:TEST:WRITE",																SCPI_CMD_WRITE_TEST},\
  {":UDP:REMOte:IP",																	SCPI_CMD_UDP_REMOTE_IP_SET},
  {":UDP:REMOte:IP?",																	SCPI_CMD_UDP_REMOTE_IP_GET},
  {":UDP:REMOte:PORT",																SCPI_CMD_UDP_REMOTE_PORT_SET},
  {":UDP:REMOte:PORT?",																SCPI_CMD_UDP_REMOTE_PORT_GET},

  //usb Power meter
  {":USBPOWERmeter:FREQ",									SCPI_CMD_POWERMETER_FREQ_SET},
  {":USBPOWERmeter:FREQ?",									SCPI_CMD_POWERMETER_FREQ_GET},
  {":USBPOWERmeter:AMPT?",									SCPI_CMD_POWERMETER_AMPT_GET},
  {":CALAFREQ:FREQ",										SCPI_CMD_CALAFREQ			},
  {":CLEAR",												SCPI_CMD_CLEAR			},
  {":SYSTem:TEMP?",											SCPI_CMD_TEMP}, 
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
