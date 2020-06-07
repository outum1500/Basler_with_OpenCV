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
using MySql.Data.MySqlClient;

namespace database_connection_test
{

    public partial class Form1 : Form
    {
        MySqlConnection connection;
        string tableName = "dworld_table";

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // ------------------------------ List view Setting ----------------------------------

            listView1.Columns.Add("PK", 40);
            listView1.Columns.Add("DATE", 100);
            listView1.Columns.Add("TIME", 80);
            listView1.Columns.Add("NUM", 40);
            listView1.Columns.Add("SIZE", 60);
            listView1.Columns.Add("ANGLE", 90);
            listView1.Columns.Add("ERR", 50);
            listView1.Columns.Add("IMAGE", 200);

            listView1.View = View.Details;

            listView1.FullRowSelect = true;
            listView1.GridLines = true;

            // ------------------------------ List view Setting end ----------------------------------

            Button bt_restart = new Button();
            bt_restart.Size = new Size(66, 18);
            bt_restart.Location = new Point(1029, 214);
            bt_restart.Text = "pi 재시작";
            bt_restart.Click += new EventHandler(button5_Click);
            this.Controls.Add(bt_restart);




            // 테스트할때 귀찮아서 해놓은 설정들
            textBox2.Text = "192.168.0.120";
            textBox3.Text = "root";
            textBox4.Text = "1234";
            textBox5.Text = "dworld_rpi_connection";

        }




        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            // 선택모드일 경우
            if (radioButton2.Checked)
            {
                // ListView 초기화
                listView1.Items.Clear();

                MySqlDataAdapter clickAdapt;
                DataTable clickDataTable = new DataTable();
                int checkCount = -1;



                // ListView에 클릭한 제품 결함좌표만 출력
                checkCount = listBox1.SelectedIndex;

                if (checkCount != -1)
                {
                    string query = "select * from " + tableName + " where u_date=\"" + listBox1.Items[checkCount].ToString() + "\";";
                    clickAdapt = new MySqlDataAdapter(query, connection);
                    clickAdapt.Fill(clickDataTable);
                }

                for (int i = 0; i < clickDataTable.Rows.Count; i++)
                {
                    DataRow clickDr = clickDataTable.Rows[i];
                    ListViewItem listitem = new ListViewItem(clickDr["pk"].ToString());
                    listitem.SubItems.Add(clickDr["u_date"].ToString());
                    listitem.SubItems.Add(clickDr["u_time"].ToString());
                    listitem.SubItems.Add(clickDr["u_product_num"].ToString());
                    listitem.SubItems.Add(clickDr["u_size"].ToString());
                    listitem.SubItems.Add(clickDr["u_angle"].ToString());
                    listitem.SubItems.Add(clickDr["u_error"].ToString());
                    listitem.SubItems.Add(clickDr["u_image"].ToString());
                    listView1.Items.Add(listitem);
                }
            }
        }




        private void radioButton1_CheckedChanged(object sender, EventArgs e)
        {
            if (radioButton1.Checked)
            {
                listView1.Items.Clear();

                MySqlDataAdapter adapt = new MySqlDataAdapter("SELECT * FROM " + tableName, connection);
                DataTable dt = new DataTable();
                adapt.Fill(dt);

                for (int i = 0; i < dt.Rows.Count; i++)
                {
                    // ListView에 전체 결함 좌표 출력
                    DataRow dr = dt.Rows[i];
                    ListViewItem listitem = new ListViewItem(dr["pk"].ToString());
                    listitem.SubItems.Add(dr["u_date"].ToString());
                    listitem.SubItems.Add(dr["u_time"].ToString());
                    listitem.SubItems.Add(dr["u_product_num"].ToString());
                    listitem.SubItems.Add(dr["u_size"].ToString());
                    listitem.SubItems.Add(dr["u_angle"].ToString());
                    listitem.SubItems.Add(dr["u_error"].ToString());
                    listitem.SubItems.Add(dr["u_image"].ToString());
                    listView1.Items.Add(listitem);
                }

            }
        }

        private void radioButton2_CheckedChanged(object sender, EventArgs e)
        {
            listView1.Items.Clear();
        }


        private void button1_Click(object sender, EventArgs e)
        {
            if (radioButton2.Checked)
            {
                listView1.Items.Clear();

                MySqlDataAdapter TextAdapt;
                DataTable TextDataTable = new DataTable();


                if (textBox1.Text == "")
                {
                    string query = "SELECT * FROM " + tableName;
                    TextAdapt = new MySqlDataAdapter(query, connection);
                    TextAdapt.Fill(TextDataTable);
                    printRows(TextDataTable);
                }
                else
                {
                    string query = "SELECT * FROM " + tableName + " where u_date=\"" + textBox1.Text + "\";";
                    TextAdapt = new MySqlDataAdapter(query, connection);
                    TextAdapt.Fill(TextDataTable);
                    printRows(TextDataTable);
                }

            }
            else
            {
                MessageBox.Show("선택모드가 아닙니다!");
            }

        }




        void printRows(DataTable table)
        {
            for (int i = 0; i < table.Rows.Count; i++)
            {
                DataRow clickDr = table.Rows[i];
                ListViewItem listitem = new ListViewItem(clickDr["pk"].ToString());
                listitem.SubItems.Add(clickDr["u_date"].ToString());
                listitem.SubItems.Add(clickDr["u_time"].ToString());
                listitem.SubItems.Add(clickDr["u_product_num"].ToString());
                listitem.SubItems.Add(clickDr["u_size"].ToString());
                listitem.SubItems.Add(clickDr["u_angle"].ToString());
                listitem.SubItems.Add(clickDr["u_error"].ToString());
                listitem.SubItems.Add(clickDr["u_image"].ToString());
                listView1.Items.Add(listitem);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            // ------------------------------ Database Connection ----------------------------------
            var connectServer = "server=" + textBox2.Text + ";";
            var connectID = "user=" + textBox3.Text + ";";
            var connectDB = "database=" + textBox5.Text + ";";
            var connectPW = "password=" + textBox4.Text + ";";

            var connectionQuery = connectServer + connectID + connectDB + connectPW;
            connection = new MySqlConnection(connectionQuery);
            string newtestid;
            string testid = null;

            try
            {
                connection.Open();
                MessageBox.Show("MySQL 연결 성공");

                textBox2.Text = "";
                textBox3.Text = "";
                textBox4.Text = "";
                textBox5.Text = "";
            }
            catch
            {
                connection.Close();
                MessageBox.Show("MySQL 연결 실패");
                Application.OpenForms["MainForm"].Close();      // 실패시 폼을 닫아준다.
            }

            // ------------------------------ Database Connection end ----------------------------------




            // Database의 값을 모두 받아온다 (ListView에 출력하기 위함)
            MySqlDataAdapter adapt = new MySqlDataAdapter("SELECT * FROM " + tableName, connection);
            DataTable dt = new DataTable();
            adapt.Fill(dt);


            for (int i = 0; i < dt.Rows.Count; i++)
            {
                // ListView에 결함 좌표 출력
                DataRow dr = dt.Rows[i];
                ListViewItem listitem = new ListViewItem(dr["pk"].ToString());
                listitem.SubItems.Add(dr["u_date"].ToString());
                listitem.SubItems.Add(dr["u_time"].ToString());
                listitem.SubItems.Add(dr["u_product_num"].ToString());
                listitem.SubItems.Add(dr["u_size"].ToString());
                listitem.SubItems.Add(dr["u_angle"].ToString());
                listitem.SubItems.Add(dr["u_error"].ToString());
                listitem.SubItems.Add(dr["u_image"].ToString());
                listView1.Items.Add(listitem);



                // 제품 ID 리스트 추출 (같은 제품 ID는 중복이므로 생략)
                newtestid = dr["u_date"].ToString();

                if (testid == null || testid != newtestid)
                {
                    testid = newtestid;
                    listBox1.Items.Add(newtestid);
                }
            }

            radioButton1.Checked = true;

        }

        private void button3_Click(object sender, EventArgs e)
        {
            // ListView에 클릭한 제품 인덱스 받아오기
            if (textBox6.Text != "")
            {
                string query = "DELETE FROM " + tableName + " WHERE pk=" + textBox6.Text + ";";
                MySqlCommand cmd = new MySqlCommand(query, connection);
                MySqlDataReader rdr = cmd.ExecuteReader();
                MessageBox.Show(textBox6.Text + "번째 항목을 삭제했습니다!");
                rdr.Close();

                listView1.Items.Clear();
                MySqlDataAdapter TextAdapt;
                DataTable TextDataTable = new DataTable();

                string delete_create_query = "SELECT * FROM " + tableName;
                TextAdapt = new MySqlDataAdapter(delete_create_query, connection);
                TextAdapt.Fill(TextDataTable);
                printRows(TextDataTable);
            }
            else
            {
                // 선택된 항목이 없다면
                MessageBox.Show("선택된 항목이 없습니다.");
            }
        }

        Button[] btn = new Button[10];

        private void listView1_SelectedIndexChanged(object sender, EventArgs e)
        {
            // 선택된 아이템이 있을 경우
            if (listView1.SelectedItems.Count > 0)
            {
                // 삭제할 index 값 미리보기
                string index_text = listView1.SelectedItems[0].SubItems[0].Text;
                textBox6.Text = index_text;

                // 이미지가 있다면 띄워주기
                if (listView1.SelectedItems[0].SubItems[7].Text != "")
                    pictureBox1.ImageLocation = listView1.SelectedItems[0].SubItems[7].Text;
                else
                    pictureBox1.ImageLocation = "";
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            string newtestid;
            string testid = null;

            listView1.Items.Clear();
            listBox1.Items.Clear();

            // Database의 값을 모두 받아온다 (ListView에 출력하기 위함)
            MySqlDataAdapter adapt = new MySqlDataAdapter("SELECT * FROM " + tableName, connection);
            DataTable dt = new DataTable();
            adapt.Fill(dt);


            for (int i = 0; i < dt.Rows.Count; i++)
            {
                // ListView에 Database 데이터 출력
                DataRow dr = dt.Rows[i];
                ListViewItem listitem = new ListViewItem(dr["pk"].ToString());
                listitem.SubItems.Add(dr["u_date"].ToString());
                listitem.SubItems.Add(dr["u_time"].ToString());
                listitem.SubItems.Add(dr["u_product_num"].ToString());
                listitem.SubItems.Add(dr["u_size"].ToString());
                listitem.SubItems.Add(dr["u_angle"].ToString());
                listitem.SubItems.Add(dr["u_error"].ToString());
                listitem.SubItems.Add(dr["u_image"].ToString());
                listView1.Items.Add(listitem);

                // 제품 ID 리스트 추출 (같은 제품 ID는 중복이므로 생략)
                newtestid = dr["u_date"].ToString();

                if (testid == null || testid != newtestid)
                {
                    testid = newtestid;
                    listBox1.Items.Add(newtestid);
                }
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {

            Socket client = null;
            try
            {
                IPEndPoint ipep = new IPEndPoint(IPAddress.Parse(textBox7.Text), 8081);
                client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                client.Connect(ipep);

                Console.WriteLine("Socket connect");

                byte[] data = new byte[1024];
                data = Encoding.UTF8.GetBytes(textBox2.Text);
                Array.Resize(ref data, 1024);
                client.Send(data, 1024, 0);

            }
            catch (Exception)
            {
                MessageBox.Show("Socket Timeout", "Error");
            }

            client.Close();

        }

        private void textBox7_TextChanged(object sender, EventArgs e)
        {

        }
    }
}