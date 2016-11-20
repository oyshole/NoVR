using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.IO.Pipes;
using System.Security.Principal;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;

namespace NoVR
{
    public partial class NoVRRemote : Form
    {
        [StructLayout(LayoutKind.Sequential)]
        [Serializable]
        struct TrackingMessage
        {
            public uint Checksum;
            public int TargetId;
            public double X;
            public double Y;
            public double Z;
            public double RotX;
            public double RotY;
            public double RotZ;
            public double RotW;
            public int Trigger;
        }

        private byte[] GetBytes(TrackingMessage m)
        {
            int length = Marshal.SizeOf(m);
            IntPtr ptr = Marshal.AllocHGlobal(length);
            byte[] myBuffer = new byte[length];

            Marshal.StructureToPtr(m, ptr, true);
            Marshal.Copy(ptr, myBuffer, 0, length);
            Marshal.FreeHGlobal(ptr);

            return myBuffer;
        }

        NamedPipeClientStream pipeClient;
        private Button sendButton;

        public NoVRRemote() : base()
        {
            InitializeComponent();

            pipeClient =
            new NamedPipeClientStream(".", "driver_sim",
                PipeDirection.InOut, PipeOptions.None,
                TokenImpersonationLevel.Identification);

            pipeClient.Connect();
        }

        TrackingMessage t = new TrackingMessage();

        private void sendButton_Click(object sender, EventArgs e)
        {
            if (!pipeClient.IsConnected) return;

            t.Checksum = 0xBAADF00D;

            var bytes = GetBytes(t);

            pipeClient.Write(bytes, 0, bytes.Length);

            pipeClient.Flush();
        }

        private void idBox_TextChanged(object sender, EventArgs e)
        {
            int.TryParse(idBox.Text, out t.TargetId);
        }

        private void xBox_TextChanged(object sender, EventArgs e)
        {
            double.TryParse(xBox.Text, out t.X);
        }

        private void yBox_TextChanged(object sender, EventArgs e)
        {
            double.TryParse(yBox.Text, out t.Y);
        }

        private void zBox_TextChanged(object sender, EventArgs e)
        {
            double.TryParse(zBox.Text, out t.Z);
        }
    }
}
