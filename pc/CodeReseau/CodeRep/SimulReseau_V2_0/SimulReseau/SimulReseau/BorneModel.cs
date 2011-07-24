using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Finisar.SQLite;
using System.Windows.Forms;


namespace SimulReseau
{
    class BorneModel
    {
        private BorneDTO borneDTO;
        DataGridView borneGridView;

        public BorneModel(BorneDTO borneDTO, DataGridView borneGridView)
        {
            this.borneDTO = borneDTO;
            this.borneGridView = borneGridView;
            borneGridView.DataSource = borneDTO.getCarTable();
        }
    } 
    }