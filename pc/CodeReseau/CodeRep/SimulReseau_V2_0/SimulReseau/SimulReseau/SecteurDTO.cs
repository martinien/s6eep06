using System;
using System.Data;
using Finisar.SQLite;

public class SecteurDTO
{
    private SQLiteConnection sql_con;
    private SQLiteDataAdapter DB; 
    private DataTable DT;
    private DataSet DS;
    private SQLiteCommand sql_cmd;

    public SecteurDTO(SQLiteConnection sql_con)
	{
        this.sql_con = sql_con;
        DS = new DataSet();
        DT = new DataTable();
	}

    public String getSecteur(int id)
    {
        sql_con.Open();
        sql_cmd = sql_con.CreateCommand();
        string CommandText = "SELECT nom FROM secteur WHERE id = " + id;
        DB = new SQLiteDataAdapter(CommandText, sql_con);
        DS.Reset();
        DB.Fill(DS);
        DT = DS.Tables[0];
        sql_con.Close();
        return (String)DT.Rows[0][0];
    }
}
