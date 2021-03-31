using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using PasswordHelper.Cripto;

namespace PasswordHelper
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            DriveInfo sdDriver = GetDriver();

            var files = sdDriver.RootDirectory.GetFiles().ToList();

            var deviceId = ReadString(() => Console.WriteLine("Enter device unique Id (10 charaters):"), 10, 10).ToUpper();

            int pin = GetPin(sdDriver, files, deviceId);

            SetPassword(sdDriver, pin);
        }

        private static void SetPassword(DriveInfo sdDriver, int pin)
        {
            var finished = false;
            do
            {
                var response = GetPasswordToChange();

                if (response != -1)
                {
                    using (StreamWriter passFile = new StreamWriter(Path.Combine(sdDriver.RootDirectory.FullName, $"C{response}.TXT"), false, System.Text.Encoding.ASCII))
                    {
                        var password = ReadString(() => Console.WriteLine($"New password on position {response}"), 1, 30);
                        passFile.WriteLine(SimpleCripto.Cripto(pin, password));
                    }
                }
                if (ReadString(() => Console.WriteLine("Finished? (y/n)"), "y", "n")
                    .Equals("y", StringComparison.InvariantCultureIgnoreCase))
                {
                    finished = true;
                }
            } while (!finished);
        }

        private static int GetPasswordToChange()
        {
            int response = -1;
            do
            {
                response = ReadInt(() =>
                {
                    Console.WriteLine("Select Password to set:");
                    for (int i = 0; i < 10; i++)
                    {
                        Console.WriteLine($"{i + 1} - Password {i + 1}");
                    }
                    Console.WriteLine("0 - Exit");
                }, 0, 10);
            } while (response == -1);
            return response;
        }

        private static int GetPin(DriveInfo sdDriver, List<FileInfo> files, string deviceId)
        {
            var lockOk = false;
            var lockFileExists = files.Any(f => f.Name.Equals("LOCK"));
            var pin = 0;
            do
            {
                if (lockFileExists)
                {
                    pin = ReadInt(() => Console.WriteLine("Enter PIN code:"), 1, 9999);
                    using (StreamReader lockFile = new StreamReader(Path.Combine(sdDriver.RootDirectory.FullName, "LOCK.TXT")))
                    {
                        var readId = lockFile.ReadLine();
                        if (SimpleCripto.Decripto(pin, readId).Equals(deviceId, StringComparison.InvariantCultureIgnoreCase))
                        {
                            lockOk = true;
                        }
                        else
                        {
                            lockFileExists = ResetDevice(sdDriver, lockFileExists);
                        }
                    }
                }
                else
                {
                    pin = ReadInt(() => Console.WriteLine("Create new PIN code (4 digits):"), 1, 9999);
                    using (StreamWriter lockFile = new StreamWriter(Path.Combine(sdDriver.RootDirectory.FullName, "LOCK.TXT"), false, System.Text.Encoding.ASCII))
                    {
                        lockFile.WriteLine(SimpleCripto.Cripto(pin, deviceId));
                        lockOk = true;
                    }
                }
            } while (!lockOk);
            return pin;
        }

        private static bool ResetDevice(DriveInfo sdDriver, bool lockFileExists)
        {
            if (ReadString(() =>
            {
                Console.WriteLine("Invalid Pin for this device.");
                Console.WriteLine("Reset device? (y/n)");
            }, "y", "n").Equals("y", StringComparison.InvariantCultureIgnoreCase))
            {
                lockFileExists = false;
                File.Delete(Path.Combine(sdDriver.RootDirectory.FullName, "LOCK.TXT"));
                for (int i = 0; i < 10; i++)
                {
                    if (File.Exists(Path.Combine(sdDriver.RootDirectory.FullName, $"C{i + 1}")))
                    {
                        File.Delete(Path.Combine(sdDriver.RootDirectory.FullName, "LOCK"));
                    }
                }
            }

            return lockFileExists;
        }

        private static DriveInfo GetDriver()
        {
            var drivers = DriveInfo.GetDrives().ToList();

            int driver = ReadInt(() =>
            {
                Console.WriteLine("Select SD card drive:");
                drivers.ForEach(d => Console.WriteLine($"{drivers.IndexOf(d) + 1} - {d.Name} - {d.VolumeLabel}"));
            }, 1, drivers.Count);

            var sdDriver = drivers[driver - 1];
            return sdDriver;
        }

        private static string ReadString(Action messageBuilder, int min, int max)
        {
            var ok = false;
            string result;
            do
            {
                messageBuilder.Invoke();
                result = Console.ReadLine();
                if (result.Length >= min && result.Length <= max)
                {
                    ok = true;
                }
                if (!ok) Console.WriteLine("Wrong value, try again");
            } while (!ok);
            return result;
        }

        private static string ReadString(Action messageBuilder, params string[] valid)
        {
            var ok = false;
            string result;
            do
            {
                messageBuilder.Invoke();
                result = Console.ReadLine();
                if (valid.Any(v => v.Equals(result, StringComparison.InvariantCultureIgnoreCase)))
                {
                    ok = true;
                }
                if (!ok) Console.WriteLine("Wrong value, try again");
            } while (!ok);
            return result;
        }

        private static int ReadInt(Action messageBuilder, int from, int to)
        {
            var ok = false;
            int result;
            do
            {
                messageBuilder.Invoke();
                var value = Console.ReadLine();
                if (int.TryParse(value, out result))
                {
                    if (result >= from && result <= to)
                    {
                        ok = true;
                    }
                }
                if (!ok) Console.WriteLine("Wrong value, try again");
            } while (!ok);
            return result;
        }
    }
}