using System;
using System.Data;
using Finisar.SQLite;


namespace SimulReseau
{
    class BorneDTO
    {
        private SQLiteConnection sql_con;
        private SQLiteDataAdapter DB;
        private DataTable DT;
        private DataSet DS;
        private SQLiteCommand sql_cmd;

        public BorneDTO(SQLiteConnection sql_con)
        {
            this.sql_con = sql_con;
            DS = new DataSet();
            DT = new DataTable();
        }

        public DataTable getCarTable()
        {
            sql_con.Open();
            sql_cmd = sql_con.CreateCommand();
            string CommandText = "SELECT * FROM borne";
            DB = new SQLiteDataAdapter(CommandText, sql_con);
            DS.Reset();
            DB.Fill(DS);
            DT = DS.Tables[0];
            sql_con.Close();
            return DT;
        }

    }
}
