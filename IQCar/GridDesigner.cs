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

        public Placement ValidatePlacement()
        {
            if (placement == null)
                return null;

            Coord? king = null;
            foreach (var car_coord in placement.GetCars())
            {
                Car car = car_coord.Item1;
                Coord coord = car_coord.Item2;
                if (car.Direction == Placement.Direction.Horizontal && car.Length == 2 && coord.y == 3)
                {
                    king = coord;
                    break;
                }
            }

            if (king == null)
                return null;

            placement.SetKing(king.Value.x, king.Value.y);
            return new Placement(placement);
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
            if (placement == null)
                return;

            Coord? coord = ScreenToCoord(e.Location);
            if (coord == null)
                return;

            var car_coord = Placement.GetCar(coord.Value);
            Car car = car_coord.Item1;
            if (car == null)
                return;

            ColorDialog dlg = new ColorDialog();
            dlg.Color = car.Color;
            if (dlg.ShowDialog() != DialogResult.OK)
                return;

            car.Color = dlg.Color;
            Invalidate();
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
    }
}
