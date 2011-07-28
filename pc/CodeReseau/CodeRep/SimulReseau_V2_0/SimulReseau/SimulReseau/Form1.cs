using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.Diagnostics;
using Finisar.SQLite;

namespace SimulReseau
{
    public partial class Form1 : Form
    {
        private SQLiteConnection sql_con;
        private SecteurDTO sectDTO;
        private BorneDTO borneDTO;
        private BorneModel borneModel;

        public Form1()
        {
            InitializeComponent();
            SetConnection();
            sectDTO = new SecteurDTO(sql_con);
            borneDTO = new BorneDTO(sql_con);
            borneModel = new BorneModel(borneDTO, borneGridView);
            serialPort1.DataReceived += new SerialDataReceivedEventHandler(serialPort1_DataReceived);
            serialPort1.Open();
            tbox_to_default();
        }

        private void quitterToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void SetConnection()
        {
            sql_con = new SQLiteConnection
                ("Data Source=smartus.db;Version=3;New=False;Compress=True;");
        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            string message = serialPort1.ReadExisting();
            int a = message.Length;
            serialPort1.DiscardInBuffer();

           if (message.Length >= 24)
            {
                ProcessTransmission(message);
            }
        }

        void ProcessTransmission(string message)
        {
            Debug.WriteLine("");
            message = message.Substring(message.IndexOf("D")-1);
            Debug.WriteLine("transmission: " + message);
            textBox1.Text = message;

            if (message.Contains("ID") && message.Contains("NBBAT") && message.Contains("QUEUE"))
            {
                Debug.WriteLine("Received status transmission");

                if (GlobalVar.statePointer + 1 == GlobalVar.stackLimit)
                {
                    GlobalVar.statePointer = 0;
                }

                if (message.Count() > 3)
                {
                    try
                    {
                        GlobalVar.receptionsState[GlobalVar.statePointer, 0] = Int32.Parse(message.Substring(3, 2));//part instead of message
                    }
                    catch
                    {
                        GlobalVar.receptionsState[GlobalVar.statePointer, 0] = 99;
                    }
                }
                if (message.Count() > 12)
                {
                    try
                    {
                        GlobalVar.receptionsState[GlobalVar.statePointer, 1] = Int32.Parse(message.Substring(12, 2));//part instead of message
                    }
                    catch
                    {
                        GlobalVar.receptionsState[GlobalVar.statePointer, 1] = 99;
                    }
                }
                if (message.Count() > 21)
                {
                    try
                    {
                        GlobalVar.receptionsState[GlobalVar.statePointer, 2] = Int32.Parse(message.Substring(21, 2));//part instead of message
                    }
                    catch
                    {
                        GlobalVar.receptionsState[GlobalVar.statePointer, 2] = 99;
                    }
                }

                Debug.WriteLine("ID: " + GlobalVar.receptionsState[GlobalVar.statePointer, 0] + " NBBAT: " + GlobalVar.receptionsState[GlobalVar.statePointer, 1] + " QUEUE: " + GlobalVar.receptionsState[GlobalVar.statePointer, 2]);
                GlobalVar.statePointer = GlobalVar.statePointer + 1;
            }
            else if (message.Contains("ID") && message.Contains("OUT") && message.Contains("IN"))
            {
                Debug.WriteLine("Received check-in/check-out transmission");

                if (GlobalVar.checkIOPointer + 1 == GlobalVar.stackLimit)
                {
                    GlobalVar.checkIOPointer = 0;
                }

                if (message.Count() > 3)
                {
                    try
                    {
                        GlobalVar.receptionsIO[GlobalVar.checkIOPointer, 0] = Int32.Parse(message.Substring(3, 2));
                    }
                    catch
                    {
                        GlobalVar.receptionsIO[GlobalVar.checkIOPointer, 0] = 99;
                    }
                }
                if (message.Count() > 12)
                {
                    try
                    {
                        GlobalVar.receptionsIO[GlobalVar.checkIOPointer, 1] = Int32.Parse(message.Substring(10, 4));
                    }
                    catch
                    {
                        GlobalVar.receptionsIO[GlobalVar.checkIOPointer, 1] = 99;
                    }
                }
                if (message.Count() > 20)
                {
                    try
                    {
                        GlobalVar.receptionsIO[GlobalVar.checkIOPointer, 2] = Int32.Parse(message.Substring(18, 4));
                    }
                    catch
                    {
                        GlobalVar.receptionsIO[GlobalVar.checkIOPointer, 2] = 99;
                    }
                }

                Debug.WriteLine("ID: " + GlobalVar.receptionsIO[GlobalVar.checkIOPointer, 0] + " OUT: " + GlobalVar.receptionsIO[GlobalVar.checkIOPointer, 1] + " IN: " + GlobalVar.receptionsIO[GlobalVar.checkIOPointer, 2]);
                GlobalVar.checkIOPointer = GlobalVar.checkIOPointer + 1;
            }

            else
                serialPort1.DiscardInBuffer();
         }

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if ((GlobalVar.start == true) && (GlobalVar.pause == false) && (GlobalVar.stop == false))
            {
                GlobalVar.stop = false;
                GlobalVar.pause = false;

                //reads contents of "car nb" column
                GlobalVar.cars[0] = Int32.Parse(textBox2.Text);
                GlobalVar.cars[1] = Int32.Parse(textBox3.Text);
                GlobalVar.cars[2] = Int32.Parse(textBox4.Text);
                GlobalVar.cars[3] = Int32.Parse(textBox5.Text);
                GlobalVar.cars[4] = Int32.Parse(textBox6.Text);
                GlobalVar.cars[5] = Int32.Parse(textBox7.Text);
                GlobalVar.cars[6] = Int32.Parse(textBox8.Text);
                GlobalVar.cars[7] = Int32.Parse(textBox9.Text);
                GlobalVar.cars[8] = Int32.Parse(textBox10.Text);
                GlobalVar.cars[9] = Int32.Parse(textBox11.Text);
                GlobalVar.cars[10] = Int32.Parse(textBox12.Text);
                GlobalVar.cars[11] = Int32.Parse(textBox13.Text);
                GlobalVar.cars[12] = Int32.Parse(textBox14.Text);
                GlobalVar.cars[13] = Int32.Parse(textBox15.Text);
                GlobalVar.cars[14] = Int32.Parse(textBox16.Text);
                GlobalVar.cars[15] = Int32.Parse(textBox17.Text);
                GlobalVar.cars[16] = Int32.Parse(textBox18.Text);
                

                //reads contents of "coeff" column
                GlobalVar.coeff[0] = Double.Parse(textBox35.Text);
                GlobalVar.coeff[1] = Double.Parse(textBox34.Text);
                GlobalVar.coeff[2] = Double.Parse(textBox33.Text);
                GlobalVar.coeff[3] = Double.Parse(textBox32.Text);
                GlobalVar.coeff[4] = Double.Parse(textBox31.Text);
                GlobalVar.coeff[5] = Double.Parse(textBox30.Text);
                GlobalVar.coeff[6] = Double.Parse(textBox29.Text);
                GlobalVar.coeff[7] = Double.Parse(textBox28.Text);
                GlobalVar.coeff[8] = Double.Parse(textBox27.Text);
                GlobalVar.coeff[9] = Double.Parse(textBox26.Text);
                GlobalVar.coeff[10] = Double.Parse(textBox25.Text);
                GlobalVar.coeff[11] = Double.Parse(textBox24.Text);
                GlobalVar.coeff[12] = Double.Parse(textBox23.Text);
                GlobalVar.coeff[13] = Double.Parse(textBox22.Text);
                GlobalVar.coeff[14] = Double.Parse(textBox21.Text);
                GlobalVar.coeff[15] = Double.Parse(textBox20.Text);
                GlobalVar.coeff[16] = Double.Parse(textBox19.Text);

                //reads contents of "rebate" column
                GlobalVar.rebate[0] = Double.Parse(textBox52.Text);
                GlobalVar.rebate[1] = Double.Parse(textBox51.Text);
                GlobalVar.rebate[2] = Double.Parse(textBox50.Text);
                GlobalVar.rebate[3] = Double.Parse(textBox49.Text);
                GlobalVar.rebate[4] = Double.Parse(textBox48.Text);
                GlobalVar.rebate[5] = Double.Parse(textBox47.Text);
                GlobalVar.rebate[6] = Double.Parse(textBox46.Text);
                GlobalVar.rebate[7] = Double.Parse(textBox45.Text);
                GlobalVar.rebate[8] = Double.Parse(textBox44.Text);
                GlobalVar.rebate[9] = Double.Parse(textBox43.Text);
                GlobalVar.rebate[10] = Double.Parse(textBox42.Text);
                GlobalVar.rebate[11] = Double.Parse(textBox41.Text);
                GlobalVar.rebate[12] = Double.Parse(textBox40.Text);
                GlobalVar.rebate[13] = Double.Parse(textBox39.Text);
                GlobalVar.rebate[14] = Double.Parse(textBox38.Text);
                GlobalVar.rebate[15] = Double.Parse(textBox37.Text);
                GlobalVar.rebate[16] = Double.Parse(textBox36.Text);

                GlobalVar.compute();
                updateTxtBox(GlobalVar.cars, GlobalVar.coeff, GlobalVar.rebate);
            }

            else if ((GlobalVar.pause == true) && (GlobalVar.start == true))
            {
                GlobalVar.start = false;
                GlobalVar.pause = false;
            }

            else if (GlobalVar.stop == true)
            {
                GlobalVar.start = false;
                GlobalVar.pause = false;
                GlobalVar.stop = false;
                tbox_to_default();
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            GlobalVar.start = true;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            GlobalVar.pause = true;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            GlobalVar.stop = true;
        }

        void tbox_to_default()
        {
            GlobalVar.tbox_to_default(sql_con);
            updateTxtBox(GlobalVar.defCars, GlobalVar.defCoeff, GlobalVar.defRebate);
        }

        void updateTxtBox(int[] carsArray, double[] coeffArray, double[] rebateArray)
        {
            //Reset to default textboxs
            textBox2.ResetText();
            textBox2.AppendText(Convert.ToString(carsArray[0]));
            textBox3.ResetText();
            textBox3.AppendText(Convert.ToString(carsArray[1]));
            textBox4.ResetText();
            textBox4.AppendText(Convert.ToString(carsArray[2]));
            textBox5.ResetText();
            textBox5.AppendText(Convert.ToString(carsArray[3]));
            textBox6.ResetText();
            textBox6.AppendText(Convert.ToString(carsArray[4]));
            textBox7.ResetText();
            textBox7.AppendText(Convert.ToString(carsArray[5]));
            textBox8.ResetText();
            textBox8.AppendText(Convert.ToString(carsArray[6]));
            textBox9.ResetText();
            textBox9.AppendText(Convert.ToString(carsArray[7]));
            textBox10.ResetText();
            textBox10.AppendText(Convert.ToString(carsArray[8]));
            textBox11.ResetText();
            textBox11.AppendText(Convert.ToString(carsArray[9]));
            textBox12.ResetText();
            textBox12.AppendText(Convert.ToString(carsArray[10]));
            textBox13.ResetText();
            textBox13.AppendText(Convert.ToString(carsArray[11]));
            textBox14.ResetText();
            textBox14.AppendText(Convert.ToString(carsArray[12]));
            textBox15.ResetText();
            textBox15.AppendText(Convert.ToString(carsArray[13]));
            textBox16.ResetText();
            textBox16.AppendText(Convert.ToString(carsArray[14]));
            textBox17.ResetText();
            textBox17.AppendText(Convert.ToString(carsArray[15]));
            textBox18.ResetText();
            textBox18.AppendText(Convert.ToString(carsArray[16]));

            textBox35.ResetText();
            textBox35.AppendText(Convert.ToString(coeffArray[0]));
            textBox34.ResetText();
            textBox34.AppendText(Convert.ToString(coeffArray[1]));
            textBox33.ResetText();
            textBox33.AppendText(Convert.ToString(coeffArray[2]));
            textBox32.ResetText();
            textBox32.AppendText(Convert.ToString(coeffArray[3]));
            textBox31.ResetText();
            textBox31.AppendText(Convert.ToString(coeffArray[4]));
            textBox30.ResetText();
            textBox30.AppendText(Convert.ToString(coeffArray[5]));
            textBox29.ResetText();
            textBox29.AppendText(Convert.ToString(coeffArray[6]));
            textBox28.ResetText();
            textBox28.AppendText(Convert.ToString(coeffArray[7]));
            textBox27.ResetText();
            textBox27.AppendText(Convert.ToString(coeffArray[8]));
            textBox26.ResetText();
            textBox26.AppendText(Convert.ToString(coeffArray[9]));
            textBox25.ResetText();
            textBox25.AppendText(Convert.ToString(coeffArray[10]));
            textBox24.ResetText();
            textBox24.AppendText(Convert.ToString(coeffArray[11]));
            textBox23.ResetText();
            textBox23.AppendText(Convert.ToString(coeffArray[12]));
            textBox22.ResetText();
            textBox22.AppendText(Convert.ToString(coeffArray[13]));
            textBox21.ResetText();
            textBox21.AppendText(Convert.ToString(coeffArray[14]));
            textBox20.ResetText();
            textBox20.AppendText(Convert.ToString(coeffArray[15]));
            textBox19.ResetText();
            textBox19.AppendText(Convert.ToString(coeffArray[16]));

            textBox52.ResetText();
            textBox52.AppendText(Convert.ToString(rebateArray[0]));
            textBox51.ResetText();
            textBox51.AppendText(Convert.ToString(rebateArray[1]));
            textBox50.ResetText();
            textBox50.AppendText(Convert.ToString(rebateArray[2]));
            textBox49.ResetText();
            textBox49.AppendText(Convert.ToString(rebateArray[3]));
            textBox48.ResetText();
            textBox48.AppendText(Convert.ToString(rebateArray[4]));
            textBox47.ResetText();
            textBox47.AppendText(Convert.ToString(rebateArray[5]));
            textBox46.ResetText();
            textBox46.AppendText(Convert.ToString(rebateArray[6]));
            textBox45.ResetText();
            textBox45.AppendText(Convert.ToString(rebateArray[7]));
            textBox44.ResetText();
            textBox44.AppendText(Convert.ToString(rebateArray[8]));
            textBox43.ResetText();
            textBox43.AppendText(Convert.ToString(rebateArray[9]));
            textBox42.ResetText();
            textBox42.AppendText(Convert.ToString(rebateArray[10]));
            textBox41.ResetText();
            textBox41.AppendText(Convert.ToString(rebateArray[11]));
            textBox40.ResetText();
            textBox40.AppendText(Convert.ToString(rebateArray[12]));
            textBox39.ResetText();
            textBox39.AppendText(Convert.ToString(rebateArray[13]));
            textBox38.ResetText();
            textBox38.AppendText(Convert.ToString(rebateArray[14]));
            textBox37.ResetText();
            textBox37.AppendText(Convert.ToString(rebateArray[15]));
            textBox36.ResetText();
            textBox36.AppendText(Convert.ToString(rebateArray[16]));
        }

        private void label1_Click_1(object sender, EventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
