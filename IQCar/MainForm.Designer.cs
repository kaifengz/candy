namespace IQCar
{
    partial class MainForm
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
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.hiddenToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.hintToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.levelToolStripTextBox = new System.Windows.Forms.ToolStripTextBox();
            this.gridPlayer = new IQCar.GridPlayer();
            this.gridDesign = new IQCar.GridDesigner();
            this.restartToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.restartToolStripMenuItem,
            this.levelToolStripTextBox,
            this.hiddenToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(431, 37);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // hiddenToolStripMenuItem
            // 
            this.hiddenToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.hintToolStripMenuItem,
            this.editToolStripMenuItem,
            this.saveToolStripMenuItem});
            this.hiddenToolStripMenuItem.Name = "hiddenToolStripMenuItem";
            this.hiddenToolStripMenuItem.Size = new System.Drawing.Size(93, 33);
            this.hiddenToolStripMenuItem.Text = "&Hidden";
            this.hiddenToolStripMenuItem.Visible = false;
            // 
            // hintToolStripMenuItem
            // 
            this.hintToolStripMenuItem.Name = "hintToolStripMenuItem";
            this.hintToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Right)));
            this.hintToolStripMenuItem.Size = new System.Drawing.Size(233, 32);
            this.hintToolStripMenuItem.Text = "&Hint";
            this.hintToolStripMenuItem.Click += new System.EventHandler(this.hintToolStripMenuItem_Click);
            // 
            // editToolStripMenuItem
            // 
            this.editToolStripMenuItem.Name = "editToolStripMenuItem";
            this.editToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F2;
            this.editToolStripMenuItem.Size = new System.Drawing.Size(233, 32);
            this.editToolStripMenuItem.Text = "&Edit";
            this.editToolStripMenuItem.Click += new System.EventHandler(this.editToolStripMenuItem_Click);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(233, 32);
            this.saveToolStripMenuItem.Text = "&Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // levelToolStripTextBox
            // 
            this.levelToolStripTextBox.AcceptsReturn = true;
            this.levelToolStripTextBox.Name = "levelToolStripTextBox";
            this.levelToolStripTextBox.Size = new System.Drawing.Size(100, 33);
            this.levelToolStripTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.levelToolStripTextBox_KeyDown);
            // 
            // gridPlayer
            // 
            this.gridPlayer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gridPlayer.Location = new System.Drawing.Point(0, 37);
            this.gridPlayer.Name = "gridPlayer";
            this.gridPlayer.Placement = null;
            this.gridPlayer.Size = new System.Drawing.Size(431, 335);
            this.gridPlayer.TabIndex = 0;
            this.gridPlayer.PlacementChanged += new IQCar.GridPlayer.PlacementChangedEventHandler(this.gridPlayer_PlacementChanged);
            // 
            // gridDesign
            // 
            this.gridDesign.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gridDesign.Location = new System.Drawing.Point(0, 37);
            this.gridDesign.Name = "gridDesign";
            this.gridDesign.Placement = null;
            this.gridDesign.Size = new System.Drawing.Size(431, 335);
            this.gridDesign.TabIndex = 0;
            this.gridDesign.Visible = false;
            this.gridDesign.DesignCompleted += new IQCar.GridDesigner.DesignCompletedEventHandler(this.gridDesign_DesignCompleted);
            this.gridDesign.DesignCancelled += new IQCar.GridDesigner.DesignCompletedEventHandler(this.gridDesign_DesignCancelled);
            // 
            // restartToolStripMenuItem
            // 
            this.restartToolStripMenuItem.Name = "restartToolStripMenuItem";
            this.restartToolStripMenuItem.Size = new System.Drawing.Size(90, 33);
            this.restartToolStripMenuItem.Text = "&Restart";
            this.restartToolStripMenuItem.Click += new System.EventHandler(this.restartToolStripMenuItem_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(431, 372);
            this.Controls.Add(this.gridPlayer);
            this.Controls.Add(this.gridDesign);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "MainForm";
            this.Text = "IQ Car";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private IQCar.GridPlayer gridPlayer;
        private IQCar.GridDesigner gridDesign;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem hintToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem hiddenToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripTextBox levelToolStripTextBox;
        private System.Windows.Forms.ToolStripMenuItem restartToolStripMenuItem;
    }
}
