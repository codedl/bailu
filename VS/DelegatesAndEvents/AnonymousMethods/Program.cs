using System;
using System.Collections.Generic;

namespace Wrox.ProCSharp.Delegates
{
    class Program
    {
        static void Main()
        {
            string mid = ", middle part,";

            Func<string, string> anonDel = delegate (string param)
            {
                param += mid;
                param += " and this was added to the string.";
                return param;
            };
            Console.WriteLine(anonDel("Start of string"));

            var values = new List<int>() { 10, 20, 30 };
            var funcs = new List<Func<int>>();
            foreach (var val in values)
                funcs.Add(() => val);
            foreach (var f in funcs)
                Console.WriteLine(f());
            Console.Read();

        }
    }

}
