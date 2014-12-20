using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.IO.Compression;
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

#if DEBUG
            LoadFromDesktopFile();
#endif

            LoadFromEmbeddedResource();

            Placement init;
            if (PuzzleLibrary.Instance.Count == 0)
            {
                init = Placement.Generate();
                levelToolStripTextBox.Enabled = false;
            }
            else
            {
                var first = PuzzleLibrary.Instance.First.Value;
                levelToolStripTextBox.Text = first.Key;
                init = first.Value;

                gridDesign.RefreshPuzzleList();
            }

            gridPlayer.Placement = placement = level_start = init;
            gridDesign.Placement = new Placement();

#if DEBUG
            hiddenToolStripMenuItem.Visible = true;
#endif
        }

        private Placement placement;
        private Placement level_start;
        private List<Placement> solution;
        private int index;
        private const string puzzles_xml = "puzzles.xml";

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

        private void restartToolStripMenuItem_Click(object sender, EventArgs e)
        {
            gridPlayer.Placement = placement = level_start;
            solution = null;
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
#if DEBUG
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.FileName = puzzles_xml;
            if (dlg.ShowDialog() != DialogResult.OK)
                return;
            PuzzleLibrary.Instance.SavePuzzles(dlg.FileName);
#endif
        }

        private void levelToolStripTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Up || e.KeyCode == Keys.Down)
            {
                string level = levelToolStripTextBox.Text;
                var new_level = (e.KeyCode == Keys.Up ?
                    PuzzleLibrary.Instance.Previous(level) : PuzzleLibrary.Instance.Next(level));
                if (new_level != null)
                {
                    levelToolStripTextBox.Text = new_level.Value.Key;
                    e.Handled = true;
                    ApplyPuzzle();
                }
            }
            else if (e.KeyCode == Keys.Enter && ApplyPuzzle())
            {
                e.Handled = true;
                e.SuppressKeyPress = true;
            }
        }

        private bool ApplyPuzzle()
        {
            string name = levelToolStripTextBox.Text;
            Placement placement = PuzzleLibrary.Instance.GetPuzzle(name);
            if (placement == null)
                return false;

            gridPlayer.Placement = this.placement = level_start = placement;
            solution = null;
            return true;
        }

        private void LoadFromEmbeddedResource()
        {
            try
            {
                byte[] puzzles_xml_gz = IQCar.Properties.Resources.puzzles_xml_gz;
                using (Stream stream_gz = new MemoryStream(puzzles_xml_gz))
                {
                    using (Stream stream = new GZipStream(stream_gz, CompressionMode.Decompress))
                    {
                        PuzzleLibrary.Instance.LoadPuzzles(stream);
                    }
                }
            }
            catch
            {
            }
        }

#if DEBUG
        private void LoadFromDesktopFile()
        {
            string desktop = Environment.GetFolderPath(Environment.SpecialFolder.DesktopDirectory);
            string filename = Path.Combine(desktop, puzzles_xml);
            try
            {
                if (File.Exists(filename))
                    PuzzleLibrary.Instance.LoadPuzzles(filename);
            }
            catch
            {
            }
        }
#endif
    }
}
