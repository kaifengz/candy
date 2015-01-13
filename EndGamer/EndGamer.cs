using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace endgamer
{
    public partial class EndGamer : Form
    {
        public EndGamer()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            ChessSolver s = new ChessSolver();
            s.test();
        }
    }
}
