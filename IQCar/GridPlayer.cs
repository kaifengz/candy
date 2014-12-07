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
    public partial class GridPlayer : GridCommon
    {
        public GridPlayer()
        {
            InitializeComponent();

            this.SetStyle(ControlStyles.Selectable, true);
            this.DoubleBuffered = true;
        }

        public delegate void PlacementChangedEventHandler(object sender, EventArgs e);
        public event PlacementChangedEventHandler PlacementChanged;

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
                Dragging = null;
                ShowFocus = false;
                Invalidate();
            }
        }

        private Placement Dragging = null;
        private Car DraggingCar = null;
        private Point MouseDownPosition;

        private Coord focusing = new Coord(0, 0);
        private bool ShowFocus = false;

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            using (Brush brush = new SolidBrush(this.BackColor))
            {
                e.Graphics.FillRectangle(brush, this.ClientRectangle);
            }

            Placement placement = (this.Dragging ?? this.placement);
            if (placement == null)
                return;

            DrawEmptyCells(e.Graphics, placement);
            DrawCars(e.Graphics, placement);

            if (Dragging != null)
                DrawDraggingCar(e.Graphics, placement);
            else
                DrawFocusCircle(e.Graphics, placement);
        }

        private void DrawDraggingCar(Graphics graphics, Placement placement)
        {
            int offset = RestrictDragging(placement);

            Coord size = DraggingCar.Direction == Placement.Direction.Horizontal ?
                new Coord(DraggingCar.Length, 1) : new Coord(1, DraggingCar.Length);
            Rectangle rect = CoordToScreen(focusing.x, focusing.y, size.x, size.y);

            if (DraggingCar.Direction == Placement.Direction.Horizontal)
                rect.X += offset;
            else
                rect.Y -= offset;

            using (Brush brush = new SolidBrush(DraggingCar.Color))
            {
                graphics.FillRectangle(brush,
                    rect.Left + CarMargin,
                    rect.Top + CarMargin,
                    GridSize * size.x - CarMargin * 2,
                    GridSize * size.y - CarMargin * 2);
            }

            if (ShowFocus)
            {
                using (Pen pen = new Pen(Color.Gray, 2))
                {
                    int x = rect.Left + rect.Width / 2;
                    int y = rect.Top + rect.Height / 2;

                    graphics.DrawEllipse(pen,
                        x - FocusRadius, y - FocusRadius,
                        FocusRadius * 2, FocusRadius * 2);
                }
            }
        }

        private void DrawFocusCircle(Graphics graphics, Placement placement)
        {
            if (!ShowFocus)
                return;

            using (Pen pen = new Pen(Color.Gray, 2))
            {
                Car focused = placement.GetCar(focusing).Item1;
                Coord size = (focused == null ? new Coord(1, 1) :
                    (focused.Direction == IQCar.Placement.Direction.Horizontal ?
                    new Coord(focused.Length, 1) : new Coord(1, focused.Length)));
                Rectangle rect = CoordToScreen(focusing.x, focusing.y, size.x, size.y);
                int x = rect.Left + rect.Width / 2;
                int y = rect.Top + rect.Height / 2;

                graphics.DrawEllipse(pen,
                    x - FocusRadius, y - FocusRadius,
                    FocusRadius * 2, FocusRadius * 2);
            }
        }

        private int RestrictDragging(Placement placement)
        {
            Point cursor = PointToClient(Cursor.Position);

            int offset = DraggingCar.Direction == Placement.Direction.Horizontal ?
                cursor.X - MouseDownPosition.X : MouseDownPosition.Y - cursor.Y;
            int steps = (int)(Math.Sign(offset) * Math.Ceiling(1.0 * Math.Abs(offset) / GridSize));

            placement.Place(focusing.x, focusing.y, DraggingCar);
            while (!placement.CanMove(focusing, steps) && steps != 0)
                steps -= Math.Sign(steps);
            placement.RemoveCar(focusing);

            return offset > 0 ? Math.Min(offset, GridSize * steps) : Math.Max(offset, GridSize * steps);
        }

        protected override bool IsInputKey(Keys keyData)
        {
            switch (keyData & ~Keys.Shift)
            {
                case Keys.Left:
                case Keys.Right:
                case Keys.Up:
                case Keys.Down:
                    return true;
            }

            return base.IsInputKey(keyData);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if (placement != null && Dragging == null)
            {
                bool shift = (e.KeyData & Keys.Shift) != 0;
                Keys keyData = (Keys)(e.KeyData & ~Keys.Shift);
                switch (keyData)
                {
                    case Keys.Left:
                    case Keys.Right:
                    case Keys.Up:
                    case Keys.Down:
                        ShowFocus = true;
                        if (shift || !MoveCar(keyData))
                            MoveFocus(keyData);
                        break;
                }
            }

            base.OnKeyDown(e);
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);

            if (placement == null || Dragging != null)
                return;

            Coord? coord = ScreenToCoord(e.Location);
            if (coord == null)
                return;

            var car_coord = Placement.GetCar(coord.Value);
            if (car_coord.Item1 != null)
            {
                ShowFocus = false;
                focusing = car_coord.Item2;

                Dragging = new Placement(placement);
                DraggingCar = Dragging.RemoveCar(focusing);
                MouseDownPosition = e.Location;

                Invalidate();
            }
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);

            if (placement != null && Dragging != null)
                Invalidate();
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            base.OnMouseUp(e);

            if (placement == null || Dragging == null)
                return;

            int offset = RestrictDragging(Dragging);
            int steps = (int)Math.Round(1.0 * offset / GridSize);

            if (steps != 0)
            {
                Dragging.Place(focusing.x, focusing.y, DraggingCar);
                if (Dragging.GetKing().Item1 == null)
                    Dragging.SetKing(focusing.x, focusing.y);
                Dragging.Move(focusing, steps);

                placement = Dragging;
                if (DraggingCar.Direction == Placement.Direction.Horizontal)
                    focusing.x += steps;
                else
                    focusing.y += steps;
            }

            Dragging = null;
            Invalidate();

            if (PlacementChanged != null)
                PlacementChanged(this, EventArgs.Empty);
        }

        private bool MoveFocus(Keys keyData)
        {
            Debug.Assert(placement != null);

            Car focused = placement.GetCar(focusing).Item1;
            Coord focused_size = (focused == null ? new Coord(1, 1) :
                (focused.Direction == IQCar.Placement.Direction.Horizontal ?
                new Coord(focused.Length, 1) : new Coord(1, focused.Length)));

            Coord new_focus = new Coord(focusing);
            switch (keyData)
            {
                case Keys.Left:
                    new_focus.x -= 1;
                    break;

                case Keys.Right:
                    new_focus.x += focused_size.x;
                    break;

                case Keys.Up:
                    new_focus.y += focused_size.y;
                    break;

                case Keys.Down:
                    new_focus.y -= 1;
                    break;

                default:
                    return false;
            }

            if (new_focus.x < 0 || new_focus.x >= Placement.Size)
                return false;
            if (new_focus.y < 0 || new_focus.y >= Placement.Size)
                return false;

            focusing = placement.GetCar(new_focus).Item2;
            Invalidate();
            return true;
        }

        private bool MoveCar(Keys keyData)
        {
            Debug.Assert(placement != null);

            Car car = placement.GetCar(focusing).Item1;
            if (car == null)
                return false;

            switch (keyData)
            {
                case Keys.Left:
                case Keys.Right:
                    if (car.Direction != Placement.Direction.Horizontal)
                        return false;

                    int xsteps = keyData == Keys.Left ? -1 : +1;
                    if (!placement.Move(focusing, xsteps))
                        return false;
                    focusing.x += xsteps;
                    break;

                case Keys.Up:
                case Keys.Down:
                    if (car.Direction != Placement.Direction.Vertical)
                        return false;

                    int ysteps = keyData == Keys.Up ? +1 : -1;
                    if (!placement.Move(focusing, ysteps))
                        return false;
                    focusing.y += ysteps;
                    break;

                default:
                    return false;
            }

            Invalidate();

            if (PlacementChanged != null)
                PlacementChanged(this, EventArgs.Empty);

            return true;
        }
    }
}
