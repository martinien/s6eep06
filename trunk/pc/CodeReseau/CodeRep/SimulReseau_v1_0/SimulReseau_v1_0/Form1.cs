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

namespace SimulReseau_v1_0
{
    public partial class Form1 : Form
    {
        int stackLimit = 10;
        int[,] receptions = new int[50, 3];
        int receptionsPointer = 0;

        public Form1()
        {
            InitializeComponent();

            serialPort1.DataReceived += new SerialDataReceivedEventHandler(serialPort1_DataReceived);
            serialPort1.Open();
            textBox1.AppendText("Ready/n");

            for(int i = 0 ; i<50 ; i++)
            {
                for(int j = 0 ; j<3 ; j++)
                    receptions[i,j] = 999;
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            //Console.WriteLine("serialPort1.BytesToRead = " + serialPort1.BytesToRead);
            if (serialPort1.BytesToRead >= 24)
            {
                this.Invoke(new EventHandler(ProcessTransmission));
            }
        }

        private void ProcessTransmission(object s, EventArgs e)
        {     
            string message = "";

            /*try
            {*/
                message = serialPort1.ReadExisting();

                serialPort1.DiscardInBuffer();
                textBox1.Text = message;
                Debug.WriteLine("");
                Debug.WriteLine("Entry: " + message);
                Debug.WriteLine("");
                Debug.WriteLine("ID?: " + message.Contains("ID"));
                Debug.WriteLine("NBBAT? : " + message.Contains("NBBAT"));
                Debug.WriteLine("QUEUE?: " + message.Contains("QUEUE"));

                if (message.Contains("ID") && message.Contains("NBBAT") && message.Contains("QUEUE"))
                {
                    if (receptionsPointer + 1 == stackLimit)
                    {
                        receptionsPointer = 0;
                        Debug.WriteLine("");
                        Debug.WriteLine("Table overflow at: " + message);
                        Debug.WriteLine("");
                        Debug.WriteLine("Writing last message at pointer = 0");
                        Debug.WriteLine("");
                    }

                    int maxCount = message.Count(f => f == 'I');
                    for (int count = 1; count < maxCount; count++)
                    {
                        int lastIDtag = message.LastIndexOf("ID");
                        string part = message.Substring(lastIDtag);
                        message = message.Remove(lastIDtag);

                        if (part.Count() > 3)
                        {
                            receptions[receptionsPointer, 0] = Int32.Parse(part.Substring(3, 2));
                        }
                        if (part.Count() > 12)
                        {
                            receptions[receptionsPointer, 0] = Int32.Parse(part.Substring(12, 2));
                        }
                        if (part.Count() > 21)
                        {
                            receptions[receptionsPointer, 0] = Int32.Parse(part.Substring(21, 2));
                        }
                    }

                    receptionsPointer = receptionsPointer + 1;
                    Debug.WriteLine("receptionsPointer= " + receptionsPointer);
                }
                else
                {
                    Debug.WriteLine("last transmission received is invalid/n");
                }

            //}
           // catch (System.Exception ex){}
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
    
    }
}
