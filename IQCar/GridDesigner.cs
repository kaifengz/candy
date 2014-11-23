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
        }

        private void DrawDragging(Graphics graphics)
        {
        }

        private Coord GetDraggingDestination(Point cursor)
        {
            Debug.Assert(Dragging);

            Coord? dst = ScreenToCoord(cursor);
            if (dst == null)
                return DraggingFrom;

            int ox = dst.Value.x - DraggingFrom.x;
            int oy = dst.Value.y - DraggingFrom.y;
            Placement.Direction dir;
            int length;
            int sign;

            if (Math.Abs(ox) >= Math.Abs(oy))
            {
                dir = Placement.Direction.Horizontal;
                length = Math.Min(3, Math.Abs(ox));
                sign = Math.Sign(ox);
                oy = 0;
            }
            else
            {
                dir = Placement.Direction.Vertical;
                length = Math.Min(3, Math.Abs(oy));
                sign = Math.Sign(oy);
                ox = 0;
            }

            Debug.Assert(placement.GetCar(DraggingFrom).Item1 == null);

            while (length > 1)
            {
                int x = DraggingFrom.x;
                int y = DraggingFrom.y;

                if (sign < 0)
                {
                    if (dir == Placement.Direction.Horizontal)
                        x -= length;
                    else
                        y -= length;
                }

                if (placement.CanPlace(x, y, dir, length))
                    break;
            }

            if (dir == Placement.Direction.Horizontal)
                ox = sign * length;
            else
                oy = sign * length;

            return new Coord(DraggingFrom.x + ox, DraggingFrom.y + oy);
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);

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

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);

            if (placement != null && Dragging)
                Invalidate();
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            base.OnMouseUp(e);

            if (placement == null || !Dragging)
                return;

            Dragging = false;
            Invalidate();
        }
    }
}
