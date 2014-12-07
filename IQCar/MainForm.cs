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

            placement = level_start = Placement.Generate();
            gridPlayer.Placement = placement;
            gridDesign.Placement = new Placement();
        }

        private Placement placement;
        private Placement level_start;
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
            Debug.Assert(gridPlayer.Visible == editToolStripMenuItem.Enabled);

            if (gridPlayer.Visible)
            {
                gridDesign.Placement = new Placement();
                gridPlayer.Visible = false;
                gridDesign.Visible = true;
                editToolStripMenuItem.Enabled = false;
                this.Text = this.ProductName + " - Designing";
            }
        }

        private void gridDesign_DesignCompleted(object sender, EventArgs args)
        {
            DesignCompleted(true);
        }

        private void gridDesign_DesignCancelled(object sender, EventArgs args)
        {
            DesignCompleted(false);
        }

        private void DesignCompleted(bool completed)
        {
            Debug.Assert(gridPlayer.Visible != gridDesign.Visible);
            Debug.Assert(gridPlayer.Visible == editToolStripMenuItem.Enabled);

            if (gridDesign.Visible)
            {
                if (completed)
                {
                    gridPlayer.Placement = placement = level_start = gridDesign.Placement;
                    solution = null;
                }
                gridPlayer.Visible = true;
                gridDesign.Visible = false;
                editToolStripMenuItem.Enabled = true;
                this.Text = this.ProductName;
            }
        }

        private void restartLevelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            gridPlayer.Placement = placement = level_start;
            solution = null;
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }
    }
}
