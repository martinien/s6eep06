using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Finisar.SQLite;
using System.Windows.Forms;


namespace SimulReseau
{
    class CarModel
    {
        private CarDTO carDTO;
        DataGridView carGridView;

        public CarModel(CarDTO carDTO, DataGridView carGridView)
        {
            this.carDTO = carDTO;
            this.carGridView = carGridView;
            carGridView.DataSource = carDTO.getCarTable();
        }


    }
}
