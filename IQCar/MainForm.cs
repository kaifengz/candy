using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace IQCar
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();

            gridPlayer.Placement = placement;
            gridDesign.Placement = new Placement();
        }

        private Placement placement = Placement.Generate();
        private List<Placement> solution;
        private int index;

        private void hintToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (solution == null)
                solvePlacement();
            if (solution != null)
                SolutionMove(+1);
        }

        private void gridPlayer_PlacementChanged(object sender, EventArgs args)
        {
            placement = gridPlayer.Placement;
            solution = null;
        }

        private bool solvePlacement()
        {
            Solver.Node succeed = Solver.Solve(placement);
            if (succeed == null)
                return false;

            solution = new List<Placement>();
            while (succeed != null)
            {
                solution.Add(succeed.Placement);
                succeed = succeed.Parent;
            }

            solution.Reverse();
            index = 0;
            return true;
        }

        private bool SolutionMove(int steps)
        {
            Debug.Assert(solution != null);
            if (index + steps < 0 || index + steps >= solution.Count)
                return false;

            index += steps;
            gridPlayer.Placement = solution[index];
            return true;
        }

        private void editToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Debug.Assert(gridPlayer.Visible != gridDesign.Visible);

            if (gridDesign.Visible)
            {
                Placement p = gridDesign.ValidatePlacement();
                if (p == null)
                    return;

                gridPlayer.Placement = placement = p;
                solution = null;
                gridDesign.Visible = false;
                gridPlayer.Visible = true;
                this.Text = this.ProductName;
            }
            else
            {
                gridDesign.Placement = new Placement();
                gridPlayer.Visible = false;
                gridDesign.Visible = true;
                this.Text = this.ProductName + " - Designing";
            }
        }
    }
}
