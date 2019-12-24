using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace semaphore
{
    class Program
    {
        static void Main(string[] args)
        {
            int taskCount = 6;
            int semaphoreCount = 3;
            var semaphore = new SemaphoreSlim(semaphoreCount, semaphoreCount);
            var tasks = new Task[taskCount];
            for (int i = 0; i < taskCount; i++)
                tasks[i] = Task.Run(() => TaskMain(semaphore));
            Task.WaitAll(tasks);
            Console.WriteLine("all tasks completed");
            Console.ReadLine();
        }
        static void TaskMain(SemaphoreSlim semaphore)
        {
            bool iscompleted = false;
            while (!iscompleted)
            {
                if (semaphore.Wait(600))
                {
                    try
                    {
                        Console.WriteLine("Task {0} locks", Task.CurrentId);
                        Thread.Sleep(1000);
                    }
                    finally
                    {
                        Console.WriteLine("Task {0} releases", Task.CurrentId);
                        semaphore.Release();
                        iscompleted = true;
                    }
                }
                else 
                {
                    Console.WriteLine("timeout for task{0}",Task.CurrentId);
                }
            }
        }
    }
}
