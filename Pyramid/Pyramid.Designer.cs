namespace Pyramid
{
    partial class PyramidForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.menuStripMain = new System.Windows.Forms.MenuStrip();
            this.pyramidToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.triangleStyleToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.rectangleStyleToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.hintOffToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.hintOnToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.advancedHintToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.chanllengeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newChallengeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.level1ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.level2ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.level3ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.level4ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.level5ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.level6ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.level7ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.level8ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.level9ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.solutionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.solveItToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.findAllSolutionsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.stopFindingToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveSolutionsAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.findAllChallengesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.generateAChallengePackageToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.solutionIndexToolStripTextBox = new System.Windows.Forms.ToolStripTextBox();
            this.restartChallengeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStripMain.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStripMain
            // 
            this.menuStripMain.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.pyramidToolStripMenuItem,
            this.chanllengeToolStripMenuItem,
            this.solutionToolStripMenuItem,
            this.solutionIndexToolStripTextBox});
            this.menuStripMain.Location = new System.Drawing.Point(0, 0);
            this.menuStripMain.Name = "menuStripMain";
            this.menuStripMain.Size = new System.Drawing.Size(626, 27);
            this.menuStripMain.TabIndex = 1;
            this.menuStripMain.Text = "menuStrip1";
            // 
            // pyramidToolStripMenuItem
            // 
            this.pyramidToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.triangleStyleToolStripMenuItem,
            this.rectangleStyleToolStripMenuItem,
            this.toolStripSeparator1,
            this.hintOffToolStripMenuItem,
            this.hintOnToolStripMenuItem,
            this.advancedHintToolStripMenuItem});
            this.pyramidToolStripMenuItem.Name = "pyramidToolStripMenuItem";
            this.pyramidToolStripMenuItem.Size = new System.Drawing.Size(63, 23);
            this.pyramidToolStripMenuItem.Text = "&Pyramid";
            // 
            // triangleStyleToolStripMenuItem
            // 
            this.triangleStyleToolStripMenuItem.Checked = true;
            this.triangleStyleToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
            this.triangleStyleToolStripMenuItem.Name = "triangleStyleToolStripMenuItem";
            this.triangleStyleToolStripMenuItem.Size = new System.Drawing.Size(158, 22);
            this.triangleStyleToolStripMenuItem.Text = "&Pyramid Style";
            this.triangleStyleToolStripMenuItem.Click += new System.EventHandler(this.problemStyleToolStripMenuItem_Click);
            // 
            // rectangleStyleToolStripMenuItem
            // 
            this.rectangleStyleToolStripMenuItem.Name = "rectangleStyleToolStripMenuItem";
            this.rectangleStyleToolStripMenuItem.Size = new System.Drawing.Size(158, 22);
            this.rectangleStyleToolStripMenuItem.Text = "&Landscape Style";
            this.rectangleStyleToolStripMenuItem.Click += new System.EventHandler(this.problemStyleToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(155, 6);
            // 
            // hintOffToolStripMenuItem
            // 
            this.hintOffToolStripMenuItem.Checked = true;
            this.hintOffToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
            this.hintOffToolStripMenuItem.Name = "hintOffToolStripMenuItem";
            this.hintOffToolStripMenuItem.Size = new System.Drawing.Size(158, 22);
            this.hintOffToolStripMenuItem.Text = "Hint O&ff";
            this.hintOffToolStripMenuItem.Click += new System.EventHandler(this.hintToolStripMenuItem_Click);
            // 
            // hintOnToolStripMenuItem
            // 
            this.hintOnToolStripMenuItem.Name = "hintOnToolStripMenuItem";
            this.hintOnToolStripMenuItem.Size = new System.Drawing.Size(158, 22);
            this.hintOnToolStripMenuItem.Text = "&Hint On";
            this.hintOnToolStripMenuItem.Click += new System.EventHandler(this.hintToolStripMenuItem_Click);
            // 
            // advancedHintToolStripMenuItem
            // 
            this.advancedHintToolStripMenuItem.Name = "advancedHintToolStripMenuItem";
            this.advancedHintToolStripMenuItem.Size = new System.Drawing.Size(158, 22);
            this.advancedHintToolStripMenuItem.Text = "&Advanced Hint";
            this.advancedHintToolStripMenuItem.Click += new System.EventHandler(this.hintToolStripMenuItem_Click);
            // 
            // chanllengeToolStripMenuItem
            // 
            this.chanllengeToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newChallengeToolStripMenuItem,
            this.restartChallengeToolStripMenuItem,
            this.toolStripSeparator3,
            this.level1ToolStripMenuItem,
            this.level2ToolStripMenuItem,
            this.level3ToolStripMenuItem,
            this.level4ToolStripMenuItem,
            this.level5ToolStripMenuItem,
            this.level6ToolStripMenuItem,
            this.level7ToolStripMenuItem,
            this.level8ToolStripMenuItem,
            this.level9ToolStripMenuItem});
            this.chanllengeToolStripMenuItem.Name = "chanllengeToolStripMenuItem";
            this.chanllengeToolStripMenuItem.Size = new System.Drawing.Size(72, 23);
            this.chanllengeToolStripMenuItem.Text = "&Challenge";
            // 
            // newChallengeToolStripMenuItem
            // 
            this.newChallengeToolStripMenuItem.Name = "newChallengeToolStripMenuItem";
            this.newChallengeToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F2;
            this.newChallengeToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
            this.newChallengeToolStripMenuItem.Text = "New &Challenge";
            this.newChallengeToolStripMenuItem.Click += new System.EventHandler(this.newChallengeToolStripMenuItem_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(182, 6);
            // 
            // level1ToolStripMenuItem
            // 
            this.level1ToolStripMenuItem.Checked = true;
            this.level1ToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
            this.level1ToolStripMenuItem.Name = "level1ToolStripMenuItem";
            this.level1ToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
            this.level1ToolStripMenuItem.Text = "Level &1";
            this.level1ToolStripMenuItem.Click += new System.EventHandler(this.levelToolStripMenuItem_Click);
            // 
            // level2ToolStripMenuItem
            // 
            this.level2ToolStripMenuItem.Name = "level2ToolStripMenuItem";
            this.level2ToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
            this.level2ToolStripMenuItem.Text = "Level &2";
            this.level2ToolStripMenuItem.Click += new System.EventHandler(this.levelToolStripMenuItem_Click);
            // 
            // level3ToolStripMenuItem
            // 
            this.level3ToolStripMenuItem.Name = "level3ToolStripMenuItem";
            this.level3ToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
            this.level3ToolStripMenuItem.Text = "Level &3";
            this.level3ToolStripMenuItem.Click += new System.EventHandler(this.levelToolStripMenuItem_Click);
            // 
            // level4ToolStripMenuItem
            // 
            this.level4ToolStripMenuItem.Name = "level4ToolStripMenuItem";
            this.level4ToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
            this.level4ToolStripMenuItem.Text = "Level &4";
            this.level4ToolStripMenuItem.Click += new System.EventHandler(this.levelToolStripMenuItem_Click);
            // 
            // level5ToolStripMenuItem
            // 
            this.level5ToolStripMenuItem.Name = "level5ToolStripMenuItem";
            this.level5ToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
            this.level5ToolStripMenuItem.Text = "Level &5";
            this.level5ToolStripMenuItem.Click += new System.EventHandler(this.levelToolStripMenuItem_Click);
            // 
            // level6ToolStripMenuItem
            // 
            this.level6ToolStripMenuItem.Name = "level6ToolStripMenuItem";
            this.level6ToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
            this.level6ToolStripMenuItem.Text = "Level &6";
            this.level6ToolStripMenuItem.Click += new System.EventHandler(this.levelToolStripMenuItem_Click);
            // 
            // level7ToolStripMenuItem
            // 
            this.level7ToolStripMenuItem.Name = "level7ToolStripMenuItem";
            this.level7ToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
            this.level7ToolStripMenuItem.Text = "Level &7";
            this.level7ToolStripMenuItem.Click += new System.EventHandler(this.levelToolStripMenuItem_Click);
            // 
            // level8ToolStripMenuItem
            // 
            this.level8ToolStripMenuItem.Name = "level8ToolStripMenuItem";
            this.level8ToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
            this.level8ToolStripMenuItem.Text = "Level &8";
            this.level8ToolStripMenuItem.Click += new System.EventHandler(this.levelToolStripMenuItem_Click);
            // 
            // level9ToolStripMenuItem
            // 
            this.level9ToolStripMenuItem.Name = "level9ToolStripMenuItem";
            this.level9ToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
            this.level9ToolStripMenuItem.Text = "Level &9";
            this.level9ToolStripMenuItem.Click += new System.EventHandler(this.levelToolStripMenuItem_Click);
            // 
            // solutionToolStripMenuItem
            // 
            this.solutionToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.solveItToolStripMenuItem,
            this.toolStripSeparator2,
            this.findAllSolutionsToolStripMenuItem,
            this.stopFindingToolStripMenuItem,
            this.saveSolutionsAsToolStripMenuItem,
            this.toolStripSeparator4,
            this.findAllChallengesToolStripMenuItem,
            this.generateAChallengePackageToolStripMenuItem});
            this.solutionToolStripMenuItem.Name = "solutionToolStripMenuItem";
            this.solutionToolStripMenuItem.Size = new System.Drawing.Size(63, 23);
            this.solutionToolStripMenuItem.Text = "&Solution";
            this.solutionToolStripMenuItem.Visible = false;
            // 
            // solveItToolStripMenuItem
            // 
            this.solveItToolStripMenuItem.Name = "solveItToolStripMenuItem";
            this.solveItToolStripMenuItem.Size = new System.Drawing.Size(235, 22);
            this.solveItToolStripMenuItem.Text = "&Solve It";
            this.solveItToolStripMenuItem.Click += new System.EventHandler(this.solveItToolStripMenuItem_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(232, 6);
            // 
            // findAllSolutionsToolStripMenuItem
            // 
            this.findAllSolutionsToolStripMenuItem.Name = "findAllSolutionsToolStripMenuItem";
            this.findAllSolutionsToolStripMenuItem.Size = new System.Drawing.Size(235, 22);
            this.findAllSolutionsToolStripMenuItem.Text = "&Find All Solutions";
            this.findAllSolutionsToolStripMenuItem.Click += new System.EventHandler(this.findAllSolutionsToolStripMenuItem_Click);
            // 
            // stopFindingToolStripMenuItem
            // 
            this.stopFindingToolStripMenuItem.Enabled = false;
            this.stopFindingToolStripMenuItem.Name = "stopFindingToolStripMenuItem";
            this.stopFindingToolStripMenuItem.Size = new System.Drawing.Size(235, 22);
            this.stopFindingToolStripMenuItem.Text = "S&top Finding";
            this.stopFindingToolStripMenuItem.Click += new System.EventHandler(this.stopFindingToolStripMenuItem_Click);
            // 
            // saveSolutionsAsToolStripMenuItem
            // 
            this.saveSolutionsAsToolStripMenuItem.Enabled = false;
            this.saveSolutionsAsToolStripMenuItem.Name = "saveSolutionsAsToolStripMenuItem";
            this.saveSolutionsAsToolStripMenuItem.Size = new System.Drawing.Size(235, 22);
            this.saveSolutionsAsToolStripMenuItem.Text = "S&ave Solutions As...";
            this.saveSolutionsAsToolStripMenuItem.Click += new System.EventHandler(this.saveSolutionsAsToolStripMenuItem_Click);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(232, 6);
            // 
            // findAllChallengesToolStripMenuItem
            // 
            this.findAllChallengesToolStripMenuItem.Name = "findAllChallengesToolStripMenuItem";
            this.findAllChallengesToolStripMenuItem.Size = new System.Drawing.Size(235, 22);
            this.findAllChallengesToolStripMenuItem.Text = "Find All &Challenges";
            this.findAllChallengesToolStripMenuItem.Click += new System.EventHandler(this.FindAllChallengesToolStripMenuItem_Click);
            // 
            // generateAChallengePackageToolStripMenuItem
            // 
            this.generateAChallengePackageToolStripMenuItem.Name = "generateAChallengePackageToolStripMenuItem";
            this.generateAChallengePackageToolStripMenuItem.Size = new System.Drawing.Size(235, 22);
            this.generateAChallengePackageToolStripMenuItem.Text = "&Generate A Challenge Package";
            this.generateAChallengePackageToolStripMenuItem.Click += new System.EventHandler(this.generateAChallengePackageToolStripMenuItem_Click);
            // 
            // solutionIndexToolStripTextBox
            // 
            this.solutionIndexToolStripTextBox.MaxLength = 8;
            this.solutionIndexToolStripTextBox.Name = "solutionIndexToolStripTextBox";
            this.solutionIndexToolStripTextBox.Size = new System.Drawing.Size(100, 23);
            this.solutionIndexToolStripTextBox.TextBoxTextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.solutionIndexToolStripTextBox.Visible = false;
            this.solutionIndexToolStripTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.solutionIndexToolStripTextBox_KeyDown);
            this.solutionIndexToolStripTextBox.TextChanged += new System.EventHandler(this.solutionIndexToolStripTextBox_TextChanged);
            // 
            // restartChallengeToolStripMenuItem
            // 
            this.restartChallengeToolStripMenuItem.Enabled = false;
            this.restartChallengeToolStripMenuItem.Name = "restartChallengeToolStripMenuItem";
            this.restartChallengeToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F3;
            this.restartChallengeToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
            this.restartChallengeToolStripMenuItem.Text = "&Restart Challenge";
            this.restartChallengeToolStripMenuItem.Click += new System.EventHandler(this.restartChallengeToolStripMenuItem_Click);
            // 
            // PyramidForm
            // 
            this.ClientSize = new System.Drawing.Size(626, 771);
            this.Controls.Add(this.menuStripMain);
            this.KeyPreview = true;
            this.MainMenuStrip = this.menuStripMain;
            this.MaximizeBox = false;
            this.Name = "PyramidForm";
            this.Text = "Pyramid";
            this.menuStripMain.ResumeLayout(false);
            this.menuStripMain.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStripMain;
        private System.Windows.Forms.ToolStripMenuItem pyramidToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem triangleStyleToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem rectangleStyleToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem hintOnToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem solutionToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem findAllSolutionsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveSolutionsAsToolStripMenuItem;
        private System.Windows.Forms.ToolStripTextBox solutionIndexToolStripTextBox;
        private System.Windows.Forms.ToolStripMenuItem solveItToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem stopFindingToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem hintOffToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem advancedHintToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem chanllengeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newChallengeToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripMenuItem level1ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem level2ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem level3ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem level4ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem level5ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem level6ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem level7ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem level8ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem level9ToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripMenuItem findAllChallengesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem generateAChallengePackageToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem restartChallengeToolStripMenuItem;
    }
}
