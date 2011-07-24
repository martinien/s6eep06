using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using Finisar.SQLite;

namespace SimulReseau
{
    public static class GlobalVar
    { 
        private static DataSet DS = new DataSet();
        public static int stackLimit = 10;
        public static int[,] receptions = new int[50, 3];
        public static int[] defCars = new int[17];
        public static int[] cars = new int[17];
        public static Double[] defCoeff = new Double[17];
        public static Double[] coeff = new Double[17];
        public static Double[] defRebate = new Double[17];
        public static Double[] rebate = new Double[17];
        public static int receptionsPointer = 0;
        public static bool start = false;
        public static bool pause = false;
        public static bool stop = false;
        public static int[,] mvtTable = {
            { 10, 11, 99, 99 },
            { 9, 8, 3, 2 },
            { 8, 1, 3, 11 },
            { 1, 2, 13, 7 },
            { 11, 16, 15, 99 },
            { 12, 15, 13, 14 },
            { 14, 7, 99, 99 },
            { 6, 3, 9, 99 },
            { 1, 9, 99, 99 },
            { 8, 1, 3, 7 },
            { 0, 99, 99, 99 },
            { 0, 2, 16, 4 },
            { 5, 15, 13, 14 },
            { 3, 14, 12, 5 },
            { 13, 6, 3, 99 },
            { 12, 5, 4, 16 },
            { 15, 4, 11, 4 }
            };

        static public void tbox_to_default(SQLiteConnection sql_con)
        {
            sql_con.Open();
            SQLiteCommand sql_cmd = sql_con.CreateCommand();
            string CommandText = "SELECT * FROM settings";
            SQLiteDataAdapter DB = new SQLiteDataAdapter(CommandText, sql_con);
            GlobalVar.DS.Reset();
            DB.Fill(DS);
            DataTable DT = DS.Tables[0];

            for (int i = 0; i < 16; i++)
            {
                GlobalVar.defCars[i] = Convert.ToInt32(DT.Rows[i][0]);
                GlobalVar.defCoeff[i] = Convert.ToInt32(DT.Rows[i][1]);
                GlobalVar.defRebate[i] = Convert.ToInt32(DT.Rows[i][2]);
            }
        }

       static public void compute()
        {
            double qty;
            Random random = new Random();

            for (int i = 0; i < 17; i++)
            {
                if (GlobalVar.cars[i] > 0)
                {
                    qty = random.Next() % (GlobalVar.coeff[i] * 100);
                    qty = (qty / 10000 - GlobalVar.rebate[i] / 100) * GlobalVar.cars[i];
                    if (qty < 0)
                    {
                        qty = 0;
                    }
                    //Debug.WriteLine(GlobalVar.cars[i] - qty);
                    if (GlobalVar.cars[i] - qty < 0)
                    {
                        qty = GlobalVar.cars[i];
                        GlobalVar.cars[i] = 0;
                    }
                    else
                    {
                        GlobalVar.cars[i] = Convert.ToInt32(GlobalVar.cars[i] - qty);
                    }

                    int loopCounter = 0;
                    while (loopCounter < 4)
                    {
                        if (GlobalVar.mvtTable[i, loopCounter] == 99)
                        {
                            break;
                        }
                        else
                        {
                            loopCounter++;
                        }
                    }

                    if (loopCounter > 0)
                    {
                        double destination = 0;
                        int pointer = 0;
                        for (int a = 0; a < loopCounter; a++)
                        {
                            double tempDestination = Math.Max(destination, random.Next(100) / 100 + GlobalVar.rebate[GlobalVar.mvtTable[i, a]] / 100);
                            if (tempDestination > destination)
                            {
                                pointer = a;
                                destination = tempDestination;
                            }
                        }
                        GlobalVar.cars[GlobalVar.mvtTable[i, pointer]] += Convert.ToInt32(qty);
                    }
                }
            }
        }

    }


}
