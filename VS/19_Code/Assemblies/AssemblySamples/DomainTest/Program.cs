using System;
using System.Reflection;
using System.Threading;

namespace Wrox.ProCSharp.Assemblies
{
    class Program
    {
        static void Main()
        {
            AppDomain currentDomain = AppDomain.CurrentDomain;
            Console.WriteLine(currentDomain.FriendlyName);
            Thread.Sleep(1000);
            AppDomain secondDomain = AppDomain.CreateDomain("New AppDomain");
            //secondDomain.ExecuteAssembly("AssemblyA.exe");
            secondDomain.CreateInstance("AssemblyA", "Wrox.ProCSharp.Assemblies.Demo", true,
             BindingFlags.CreateInstance, null, new object[] { 7, 3 }, null, null);


        }
    }
}

