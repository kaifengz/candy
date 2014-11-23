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
    public partial class GridCommon : UserControl
    {
        public GridCommon()
        {
            InitializeComponent();
        }

        protected int GridSize { get; private set; }
        protected int XMargin { get; private set; }
        protected int YMargin { get; private set; }
        protected int CarMargin { get; private set; }
        protected int CellMargin { get; private set; }

        protected override void OnSizeChanged(EventArgs e)
        {
            base.OnSizeChanged(e);

            GridSize = Math.Min(Size.Width, Size.Height) / Placement.Size;
            XMargin = (Size.Width - GridSize * Placement.Size) / 2;
            YMargin = (Size.Height - GridSize * Placement.Size) / 2;
            CarMargin = GridSize / 20;
            CellMargin = CarMargin * 2;
        }

        protected void DrawBackground(Graphics graphics)
        {
            using (Brush brush = new SolidBrush(this.BackColor))
            {
                graphics.FillRectangle(brush, this.ClientRectangle);
            }
        }
        
        protected void DrawEmptyCells(Graphics graphics, Placement placement)
        {
            if (placement == null)
                return;

            using (Brush brush = new SolidBrush(Color.LightGray))
            {
                for (int x = 0; x < Placement.Size; ++x)
                {
                    for (int y = 0; y < Placement.Size; ++y)
                    {
                        if (placement.GetCar(x, y).Item1 == null)
                        {
                            Rectangle rect = CoordToScreen(x, y, 1, 1);
                            graphics.FillRectangle(brush,
                                rect.Left + CellMargin, rect.Top + CellMargin,
                                rect.Width - CellMargin * 2, rect.Height - CellMargin * 2);
                        }
                    }
                }
            }
        }

        protected void DrawCars(Graphics graphics, Placement placement)
        {
            if (placement == null)
                return;

            foreach (var v in placement.GetCars())
            {
                Car car = v.Item1;
                Coord topleft = v.Item2;

                Coord size = car.Direction == Placement.Direction.Horizontal ?
                    new Coord(car.Length, 1) : new Coord(1, car.Length);
                Rectangle rect = CoordToScreen(topleft.x, topleft.y, size.x, size.y);

                using (Brush brush = new SolidBrush(car.Color))
                {
                    graphics.FillRectangle(brush,
                        rect.Left + CarMargin, rect.Top + CarMargin,
                        rect.Width - CarMargin * 2, rect.Height - CarMargin * 2);
                }
            }
        }

        protected Point CoordToScreen(Coord coord)
        {
            return CoordToScreen(coord.x, coord.y);
        }

        protected Point CoordToScreen(int x, int y)
        {
            Debug.Assert(x >= 0 && x <= Placement.Size);
            Debug.Assert(y >= 0 && y <= Placement.Size);

            return new Point(
                XMargin + x * GridSize,
                YMargin + (Placement.Size - y) * GridSize);
        }

        protected Rectangle CoordToScreen(Rectangle rect)
        {
            return CoordToScreen(rect.Left, rect.Top, rect.Width, rect.Height);
        }

        protected Rectangle CoordToScreen(int left, int top, int width, int height)
        {
            return new Rectangle(
                CoordToScreen(left, top + height),
                new Size(GridSize * width, GridSize * height));
        }

        protected Coord? ScreenToCoord(Point pt)
        {
            return ScreenToCoord(pt.X, pt.Y);
        }

        protected Coord? ScreenToCoord(int x, int y)
        {
            x = (x - XMargin) / GridSize;
            y = (YMargin + GridSize * Placement.Size - y) / GridSize;

            if (x >= 0 && x < Placement.Size && y >= 0 && y < Placement.Size)
                return new Coord(x, y);
            else
                return null;
        }
    }
}
