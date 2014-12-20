using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace IQCar
{
    public partial class GridDesigner : GridCommon
    {
        public GridDesigner()
        {
            InitializeComponent();

            DoubleBuffered = true;
        }

        public delegate void DesignCompletedEventHandler(object sender, EventArgs e);
        public event DesignCompletedEventHandler DesignCompleted;
        public event DesignCompletedEventHandler DesignCancelled;

        private Placement placement;
        public Placement Placement
        {
            get
            {
                return placement != null ? new Placement(placement) : null;
            }
            set
            {
                placement = value == null ? null : new Placement(value);
                Invalidate();
            }
        }

        public void RefreshPuzzleList()
        {
#if DEBUG
            comboBoxPuzzles.Items.Clear();
            foreach (var pair in PuzzleLibrary.Instance.EnumPuzzles())
            {
                comboBoxPuzzles.Items.Add(pair.Key);
            }
#endif
        }

        private Coord? Focusing = null;

        private bool ValidatePlacement()
        {
            return placement != null && placement.ProbeKing().Item1 != null;
        }

        private readonly Color[] PredefinedColors = new Color[]
        {
            Color.Navy,
            Color.Olive,
            Color.Brown,
            Color.DarkCyan,
            Color.Violet,
            Color.Tomato,
            Color.DarkGreen,
            Color.Tan,
            Color.YellowGreen,
            Color.SlateBlue,
            Color.Cyan,
            Color.DarkOliveGreen,
            Color.Orange,
            Color.Purple,
            Color.Green,
            Color.Yellow,
            Color.Blue,
        };
        private readonly Color PreferredKingColor = Color.Red;

        private bool Dragging = false;
        private Coord DraggingFrom;

        protected override void OnSizeChanged(EventArgs e)
        {
            base.OnSizeChanged(e);

            if (this.Size.IsEmpty)
                return;

            int button_size = this.Height / 6;
            Font button_font = new Font(buttonOK.Font.FontFamily, button_size / 6, FontStyle.Bold);
            int button_margin = 10;
            int button_x = XMargin + GridSize * Placement.Size + button_margin;

            buttonOK.Size = buttonCancel.Size = buttonAdd.Size = new Size(button_size, button_size);
            buttonOK.Font = buttonCancel.Font = buttonAdd.Font = button_font;
            buttonCancel.Location = new Point(button_x, Height - button_size - button_margin);
            buttonOK.Location = new Point(button_x, buttonCancel.Top - button_size - button_margin);
            buttonAdd.Location = new Point(button_x, buttonOK.Top - button_size - button_margin);

            comboBoxPuzzles.Size = new Size(button_size, buttonAdd.Top - button_margin * 2);
            comboBoxPuzzles.Location = new Point(button_x, button_margin);

#if !DEBUG
            comboBoxPuzzles.Visible = false;
            buttonSave.Visible = false;
#endif
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            using (Brush brush = new SolidBrush(this.BackColor))
            {
                e.Graphics.FillRectangle(brush, this.ClientRectangle);
            }

            if (placement == null)
                return;

            DrawEmptyCells(e.Graphics, placement);
            DrawCars(e.Graphics, placement);

            if (Dragging)
                DrawDragging(e.Graphics);

            if (Focusing != null)
                DrawFocusCircle(e.Graphics, placement, Focusing.Value);
        }

        private void DrawDragging(Graphics graphics)
        {
            Coord dest = GetDraggingDestination(PointToClient(Cursor.Position));
            Debug.Assert(DraggingFrom.x == dest.x || DraggingFrom.y == dest.y);

            Rectangle rect = CoordToScreen(Math.Min(DraggingFrom.x, dest.x), Math.Min(DraggingFrom.y, dest.y),
                Math.Abs(DraggingFrom.x - dest.x) + 1, Math.Abs(DraggingFrom.y - dest.y) + 1);

            using (Brush brush = new SolidBrush(Color.Gray))
            {
                graphics.FillRectangle(brush, rect.Left + CarMargin, rect.Top + CarMargin,
                    rect.Width - CarMargin * 2, rect.Height - CarMargin * 2);
            }
        }

        private Coord GetDraggingDestination(Point cursor)
        {
            Debug.Assert(Dragging);

            Coord? dst = ScreenToCoord(cursor);
            if (dst == null || (dst.Value.x == DraggingFrom.x && dst.Value.y == DraggingFrom.y))
                return DraggingFrom;

            Rectangle from_rect = CoordToScreen(DraggingFrom.x, DraggingFrom.y, 1, 1);
            Point from_center = new Point(from_rect.Left + from_rect.Width / 2, from_rect.Top + from_rect.Height / 2);

            int ox = dst.Value.x - DraggingFrom.x;
            int oy = dst.Value.y - DraggingFrom.y;
            Placement.Direction dir;
            int distance;
            int sign;

            if (Math.Abs(cursor.X - from_center.X) >= Math.Abs(cursor.Y - from_center.Y))
            {
                dir = Placement.Direction.Horizontal;
                distance = Math.Min(2, Math.Abs(ox));
                sign = Math.Sign(ox);
                oy = 0;
            }
            else
            {
                dir = Placement.Direction.Vertical;
                distance = Math.Min(2, Math.Abs(oy));
                sign = Math.Sign(oy);
                ox = 0;
            }

            Debug.Assert(placement.GetCar(DraggingFrom).Item1 == null);

            while (distance > 0)
            {
                int x = DraggingFrom.x;
                int y = DraggingFrom.y;

                if (sign < 0)
                {
                    if (dir == Placement.Direction.Horizontal)
                        x -= distance;
                    else
                        y -= distance;
                }

                if (placement.CanPlace(x, y, dir, distance + 1))
                    break;
                --distance;
            }

            if (dir == Placement.Direction.Horizontal)
                ox = sign * distance;
            else
                oy = sign * distance;

            return new Coord(DraggingFrom.x + ox, DraggingFrom.y + oy);
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);

            if (e.Button == MouseButtons.Left)
                OnLeftMouseDown(e);
            else if (e.Button == MouseButtons.Right)
                OnRightMouseDown(e);
        }

        private void OnLeftMouseDown(MouseEventArgs e)
        {
            if (placement == null || Dragging)
                return;

            Coord? coord = ScreenToCoord(e.Location);
            if (coord == null)
                return;

            var car_coord = Placement.GetCar(coord.Value);
            if (car_coord.Item1 != null)
                return;

            Dragging = true;
            DraggingFrom = car_coord.Item2;
            Invalidate();
        }

        private void OnRightMouseDown(MouseEventArgs e)
        {
            Focusing = null;

            if (placement == null)
                return;

            Coord? coord = ScreenToCoord(e.Location);
            if (coord != null)
            {
                var car_coord = placement.GetCar(coord.Value);
                if (car_coord.Item1 != null)
                    Focusing = car_coord.Item2;
            }

            editColorToolStripMenuItem.Enabled = (Focusing != null);
            removeToolStripMenuItem.Enabled = (Focusing != null);
            Invalidate();

            contextMenuStrip1.Show(this, e.Location);
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);

            if (placement != null && Dragging)
                Invalidate();
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            base.OnMouseUp(e);

            if (e.Button == MouseButtons.Left)
                OnLeftMouseUp(e);
        }

        private void OnLeftMouseUp(MouseEventArgs e)
        {
            if (placement == null || !Dragging)
                return;

            Coord dest = GetDraggingDestination(PointToClient(Cursor.Position));
            Debug.Assert(DraggingFrom.x == dest.x || DraggingFrom.y == dest.y);

            Placement.Direction dir = (DraggingFrom.x == dest.x ? Placement.Direction.Vertical : Placement.Direction.Horizontal);
            int length = 1 + (dir == Placement.Direction.Horizontal ? Math.Abs(DraggingFrom.x - dest.x) : Math.Abs(DraggingFrom.y - dest.y));
            Coord coord = new Coord(Math.Min(DraggingFrom.x, dest.x), Math.Min(DraggingFrom.y, dest.y));
            Car car = new Car(Color.Gray, dir, length);

            AssignColor(car, coord);

            placement.Place(coord.x, coord.y, car);
            Dragging = false;
            Invalidate();
        }

        private bool AssignColor(Car car, Coord coord)
        {
            HashSet<Color> usedColors = GetUsedColors();

            if (car.Direction == Placement.Direction.Horizontal && coord.y == 3 && car.Length == 2)
            {
                if (!usedColors.Contains(PreferredKingColor))
                {
                    car.Color = PreferredKingColor;
                    return true;
                }
            }

            foreach (Color color in PredefinedColors)
            {
                if (!usedColors.Contains(color))
                {
                    car.Color = color;
                    return true;
                }
            }

            return false;
        }

        private HashSet<Color> GetUsedColors()
        {
            HashSet<Color> usedColors = new HashSet<Color>();

            foreach (var v in placement.GetCars())
                usedColors.Add(v.Item1.Color);

            return usedColors;
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            if (ValidatePlacement() && DesignCompleted != null)
                DesignCompleted(this, EventArgs.Empty);
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            if (DesignCancelled != null)
                DesignCancelled(this, EventArgs.Empty);
        }

        private void buttonSave_Click(object sender, EventArgs e)
        {
            if (!ValidatePlacement())
                return;

            string name = comboBoxPuzzles.Text;
            if (name == null)
                return;

            if (PuzzleLibrary.Instance.GetPuzzle(name) != null)
            {
                if (MessageBox.Show(string.Format("Overwrite {0}?", name), ProductName, MessageBoxButtons.YesNo,
                            MessageBoxIcon.Question, MessageBoxDefaultButton.Button2) != DialogResult.Yes)
                    return;
            }

            PuzzleLibrary.Instance.SetPuzzle(name, new Placement(placement));
            comboBoxPuzzles.Items.Add(name);
        }

        private void comboBoxPuzzles_SelectedIndexChanged(object sender, EventArgs e)
        {
            string name = comboBoxPuzzles.SelectedItem.ToString();
            Placement p = PuzzleLibrary.Instance.GetPuzzle(name);
            Debug.Assert(p != null);
            this.Placement = p;
        }

        private void editColorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Debug.Assert(placement != null);
            Debug.Assert(Focusing != null);

            var car_coord = placement.GetCar(Focusing.Value);
            Car car = car_coord.Item1;
            Debug.Assert(car != null);

            ColorDialog dlg = new ColorDialog();
            dlg.Color = car.Color;
            if (dlg.ShowDialog() == DialogResult.OK)
                car.Color = dlg.Color;
            Focusing = null;
            Invalidate();
        }

        private void removeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Debug.Assert(placement != null);
            Debug.Assert(Focusing != null);

            var car_coord = placement.GetCar(Focusing.Value);
            Car car = car_coord.Item1;
            Debug.Assert(car != null);

            placement.RemoveCar(car_coord.Item2);
            Focusing = null;
            Invalidate();
        }

        private void removeAllToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Debug.Assert(placement != null);

            placement = new Placement();
            Invalidate();
        }
    }
}
