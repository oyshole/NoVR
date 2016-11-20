using System;
using System.Windows.Forms;

namespace NoVR
{
    public static class Program
    {
        [STAThread]
        static void Main()
        {
            Application.Run(new NoVRRemote());
        }
    }
}
