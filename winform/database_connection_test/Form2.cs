using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace database_connection_test
{
    public partial class Form2 : Form
    {

        TextBox tb1;

        public Form2()
        {
            InitializeComponent();
        }

        private void Form2_Load(object sender, EventArgs e)
        {
            tb1 = new TextBox();
            tb1.Size = new Size(100, 50);
            tb1.Location = new Point(70, 10);
            this.Controls.Add(tb1);

            Button btn1 = new Button();
            btn1.Size = new Size(200, 50);
            btn1.Location = new Point(25, 100);
            btn1.Text = "라즈베리파리 프로그램 재시작";
            btn1.Click += new EventHandler(btn1_Click);
            this.Controls.Add(btn1);

            Label label1 = new Label();
            label1.Text = "DB IP : ";
            label1.Size = new Size(150, 50);
            label1.Location = new Point(20, 15);
            this.Controls.Add(label1);
        }

        private void btn1_Click(object sender, EventArgs e)
        {
            Socket client = null;
            try
            {
                IPEndPoint ipep = new IPEndPoint(IPAddress.Parse("192.168.0.117"), 8081);
                client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                client.Connect(ipep);

                Console.WriteLine("Socket connect");

                byte[] data = new byte[1024];
                data = Encoding.UTF8.GetBytes(tb1.Text);
                Array.Resize(ref data, 1024);
                client.Send(data, 1024, 0);

            }
            catch (Exception)
            {
                MessageBox.Show("Socket Timeout", "Error");
            }

            client.Close();

            this.Close();
        }
    }
}