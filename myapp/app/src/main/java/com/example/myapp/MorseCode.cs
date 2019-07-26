using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;

namespace SettersLibrary
{
    /// <summary>
    /// 摩尔斯密码编码解码类
    /// </summary>
    public static class MorseCode
    {
        private static volatile string[,] CodeTable =
        {
            {"A","10111"},
            {"B","111010101"},
            {"C","11101011101"},
            {"D","1110101"},
            {"E","1"},
            {"F","101011101"},
            {"G","111011101"},
            {"H","1010101"},
            {"I","101"},
            {"J","1011101110111"},
            {"K","1110111"},
            {"L","101110101"},
            {"M","1110111"},
            {"N","11101"},
            {"O","11101110111"},
            {"P","10111011101"},
            {"Q","1110111010111"},
            {"R","1011101"},
            {"S","10101"},
            {"T","111"},
            {"U","1010111"},
            {"V","101010111"},
            {"W","101110111"},
            {"X","11101010111"},
            {"Y","1110101110111"},
            {"Z","11101110101"},
            {"0","1110111011101110111"},
            {"1","10111011101110111"},
            {"2","101011101110111"},
            {"3","1010101110111"},
            {"4","10101010111"},
            {"5","101010101"},
            {"6","11101010101"},
            {"7","1110111010101"},
            {"8","111011101110101"},
            {"9","11101110111011101"},
        };



        /// <summary>
        /// 获取Morse转后的字节数组
        /// </summary>
        /// <param name="value"></param>
        /// <param name="buf"></param>
        /// <returns></returns>
        public static int GetMorseByte(string value, ref byte[] buf)
        {
            return GetMorseSerial(Enc(value), ref buf);
        }

        /// <summary>
        /// 根据编码后的01字符串转换成byte数组
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        private static int GetMorseSerial(string value, ref byte[] buf)
        {
            int _count = value.Length;
            string tempStr = value;
            if(tempStr.Length % 8 != 0)
            {
                int _addcount = 8 - (tempStr.Length % 8);
                for(int i = 0; i < _addcount; i++)      //不是8的整数倍，补全
                {
                    tempStr += "0";
                }
            }
            byte[] result = new byte[tempStr.Length / 8];
            for(int i = 0; i < tempStr.Length; i = i+8) //转换成byte数组
            {
                string _tem = tempStr.Substring(i, 8);
                result[i / 8] = Convert.ToByte(_tem, 2);
            }
            buf = result;
            return _count;
        }

        /// <summary>
        /// 编码
        /// </summary>
        /// <param name="str">字符串</param>
        /// <returns>编码后字符串，每个编码以空格分开</returns>
        private static string Enc(string str)
        {
            int i;
            string ret = string.Empty;      
            str = Regex.Replace(str, @"\s", "");//去除所有空格
            if (str != null && (str = str.ToUpper()).Length > 0)
            {
                for(int j = 0; j < str.Length; j++)
                {
                    if ((i = Find(str[j].ToString(), 0)) > -1)
                    {
                        string temp = CodeTable[i, 1];
                        ret += temp;
                        if ((j + 1) % 4 == 0 && (j + 1) != str.Length)  //四个一组，组组间需要加入"00000"
                        {
                            ret += "00000";
                        }
                        else if ((j + 1) < str.Length)  //字符间需要加入"000"
                        {
                            ret += "000";
                        }
                    }
                }
            }
            return ret + "00000";//结束后需要加入5个0
        }



        /// <summary>
        /// 解码
        /// </summary>
        /// <param name="str">需要解码的字符串，每个编码需要空格隔开</param>
        /// <returns>解码后字符串</returns>
        public static string Dec(string str)
        {
            int i;
            string[] splits;
            string ret = string.Empty;
            if (str != null && (splits = str.Split(' ')).Length > 0)
            {
                foreach (string split in splits)
                    if ((i = Find(split, 1)) > -1)
                        ret += CodeTable[i, 0];
                return ret;
            }
            return "{#}";
        }

        /// <summary>
        /// 查表
        /// </summary>
        /// <param name="str">需要查找字符串</param>
        /// <param name="cols">查表方式 0：编码 1：解码</param>
        /// <returns></returns>
        private static int Find(string str, int cols)
        {
            int i = 0, len = CodeTable.Length / 2; // len / rank  
            while (i < len)
            {
                if (CodeTable[i, cols] == str)
                    return i;
                i++;
            };
            return -1;
        }

        /// <summary>
        /// 调用系统dll文件实现蜂鸣器播放
        /// </summary>
        /// <param name="frequency">声音频率</param>
        /// <param name="duration">播放时间</param>
        /// <returns></returns>
        [DllImport("Kernel32.dll")]
        public static extern bool Beep(int frequency, int duration);

        /// <summary>
        /// 使用电脑蜂鸣器播放声音
        /// </summary>
        /// <param name="_str">需要播放的摩尔斯密码</param>
        public static void Play(string _str)
        {
            foreach (char c in _str)
            {
                switch (c)
                {
                    case '-':
                        Beep(3000, 300);        //3000HZ蜂鸣器，时间300ms
                        break;
                    case '.':
                        Beep(3000, 100);        //3000HZ蜂鸣器，时间100ms
                        Thread.Sleep(100);      
                        break;
                    case ' ':
                        Thread.Sleep(300);      
                        break;
                }
            }
        }
    }
}
