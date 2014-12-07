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
            this.gameToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.restartLevelToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.hiddenToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.hintToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.gridPlayer = new IQCar.GridPlayer();
            this.gridDesign = new IQCar.GridDesigner();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.gameToolStripMenuItem,
            this.hiddenToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(431, 35);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // gameToolStripMenuItem
            // 
            this.gameToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.restartLevelToolStripMenuItem,
            this.toolStripSeparator1});
            this.gameToolStripMenuItem.Name = "gameToolStripMenuItem";
            this.gameToolStripMenuItem.Size = new System.Drawing.Size(80, 31);
            this.gameToolStripMenuItem.Text = "&Game";
            // 
            // restartLevelToolStripMenuItem
            // 
            this.restartLevelToolStripMenuItem.Name = "restartLevelToolStripMenuItem";
            this.restartLevelToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F3;
            this.restartLevelToolStripMenuItem.Size = new System.Drawing.Size(239, 32);
            this.restartLevelToolStripMenuItem.Text = "&Restart Level";
            this.restartLevelToolStripMenuItem.Click += new System.EventHandler(this.restartLevelToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(236, 6);
            // 
            // hiddenToolStripMenuItem
            // 
            this.hiddenToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.hintToolStripMenuItem,
            this.editToolStripMenuItem,
            this.saveToolStripMenuItem});
            this.hiddenToolStripMenuItem.Name = "hiddenToolStripMenuItem";
            this.hiddenToolStripMenuItem.Size = new System.Drawing.Size(93, 31);
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
            // gridPlayer
            // 
            this.gridPlayer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gridPlayer.Location = new System.Drawing.Point(0, 35);
            this.gridPlayer.Name = "gridPlayer";
            this.gridPlayer.Placement = null;
            this.gridPlayer.Size = new System.Drawing.Size(431, 337);
            this.gridPlayer.TabIndex = 0;
            this.gridPlayer.PlacementChanged += new IQCar.GridPlayer.PlacementChangedEventHandler(this.gridPlayer_PlacementChanged);
            // 
            // gridDesign
            // 
            this.gridDesign.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gridDesign.Location = new System.Drawing.Point(0, 35);
            this.gridDesign.Name = "gridDesign";
            this.gridDesign.Placement = null;
            this.gridDesign.Size = new System.Drawing.Size(431, 337);
            this.gridDesign.TabIndex = 0;
            this.gridDesign.Visible = false;
            this.gridDesign.DesignCompleted += new IQCar.GridDesigner.DesignCompletedEventHandler(this.gridDesign_DesignCompleted);
            this.gridDesign.DesignCancelled += new IQCar.GridDesigner.DesignCompletedEventHandler(this.gridDesign_DesignCancelled);
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
        private System.Windows.Forms.ToolStripMenuItem gameToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem hintToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem hiddenToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem restartLevelToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
    }
}
