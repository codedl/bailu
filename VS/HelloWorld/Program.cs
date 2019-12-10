using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
#pragma warning disable 169
public class MathTest
{
    public int value;
    public int GetSquare()
    {
        return value * value;
    }
    public static int GetSquareOf(int x)
    {
        return x * x;
    }
    public static double GetPi()
    {
        return 3.14159;
    }
}

public class Money
{
    private decimal money;
    public decimal amount {
        get { return money; }
        set { money = value; }
    }
    public override string ToString()
    {
        return "$" + money.ToString();
    }
}
#pragma warning restore 169
namespace HelloWorld
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Hello from Wrox");

            var name = "Bugs Bunny";
            var age = 27;
            var isRabbit = true;
            Type nameType = name.GetType();
            Type ageType = age.GetType();
            Type isRabbitType = isRabbit.GetType();
            Console.WriteLine("name is " + nameType.ToString());
            Console.WriteLine("age is " + ageType.ToString());
            Console.WriteLine("isRabbit is " + isRabbitType.ToString());
            {
                int i = 100;
                int j = 100;
                Console.WriteLine(" {0,4}\n+{1,4}\n----\n {2,4}", i, j, i + j);
                Console.WriteLine("{0:#.000}", 1.2345);
#warning "this is a warning test" 

#line 164 "core.cs"//定义文件为core.cs，行为164

            }
            //#error "this is a error test"
            
                WeakReference weakReference = new WeakReference(new MathTest());
                MathTest math;
                if (weakReference.IsAlive)
                {
                    math = weakReference.Target as MathTest;
                    math.value = 30;
                    Console.WriteLine("math value:{0};square is:{1}",math.value,math.GetSquare());
                }
                else
                {
                    Console.WriteLine("math is not available");
                }
                GC.Collect();
                if (weakReference.IsAlive)
                {
                    math = weakReference.Target as MathTest;
                    math.value = 50;
                    Console.WriteLine("math value:{0};square is:{1}", math.value, math.GetSquare());
                }
                else
                {
                    Console.WriteLine("math is not available");
                }
            {
                Money cash = new Money();
                cash.amount = 40M;
                Console.WriteLine("ToString:" + cash.ToString());
            }
            Console.ReadLine();
            return;
        }
    }
}
