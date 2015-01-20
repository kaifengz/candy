using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace endgamer
{
    public partial class EndGamer : Form
    {
        private ChessBoard board;

        public EndGamer()
        {
            InitializeComponent();
            this.DoubleBuffered = true;

            board = ChessSolver.init_test();
        }

        protected override void OnResize(EventArgs e)
        {
            base.OnResize(e);

            Invalidate();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            DrawBackground(e.Graphics);
            DrawBoardGrid(e.Graphics);
            DrawChesses(e.Graphics, board);
        }

        private void DrawBackground(Graphics graphics)
        {
            using (Brush brush = new SolidBrush(this.BackColor))
            {
                graphics.FillRectangle(brush, this.ClientRectangle);
            }
        }

        private void DrawBoardGrid(Graphics graphics)
        {
            int width = this.ClientSize.Width;
            int height = this.ClientSize.Height;
            int n = Math.Min(width / 10, height / 11);
            int x0 = (width - n * 8) / 2;
            int y0 = (height - n * 9) / 2;
            using (Pen pen = new Pen(Color.DarkBlue))
            {
                for (int h = 0; h < 10; ++h)
                {
                    int y = y0 + n * h;
                    graphics.DrawLine(pen, x0, y, x0 + n * 8, y);
                }

                graphics.DrawLine(pen, x0, y0, x0, y0 + n * 9);
                graphics.DrawLine(pen, x0 + n * 8, y0, x0 + n * 8, y0 + n * 9);
                for (int w = 1; w < 8; ++w)
                {
                    int x = x0 + n * w;
                    graphics.DrawLine(pen, x, y0, x, y0 + n * 4);
                    graphics.DrawLine(pen, x, y0 + n * 5, x, y0 + n * 9);
                }

                graphics.DrawLine(pen, x0 + n * 3, y0, x0 + n * 5, y0 + n * 2);
                graphics.DrawLine(pen, x0 + n * 5, y0, x0 + n * 3, y0 + n * 2);
                graphics.DrawLine(pen, x0 + n * 3, y0 + n * 7, x0 + n * 5, y0 + n * 9);
                graphics.DrawLine(pen, x0 + n * 5, y0 + n * 7, x0 + n * 3, y0 + n * 9);
            }
        }

        private void DrawChesses(Graphics graphics, ChessBoard board)
        {
            int width = this.ClientSize.Width;
            int height = this.ClientSize.Height;
            int n = Math.Min(width / 10, height / 11);
            int x0 = (width - n * 8) / 2;
            int y0 = (height - n * 9) / 2;

            using (Font font = new Font(this.Font.Name, n/2, GraphicsUnit.Pixel))
            {
                int s = (int)(font.Size / 2) + 3;
                using (Brush rbrush = new SolidBrush(Color.Red),
                    bbrush = new SolidBrush(Color.Black),
                    brush = new SolidBrush(this.BackColor))
                {
                    using (Pen rpen = new Pen(rbrush, 2),
                        bpen = new Pen(bbrush, 2))
                    {
                        foreach (var v in board.Pieces())
                        {
                            Position pos = v.Item1;
                            ChessPiece piece = v.Item2;

                            Rectangle rect = new Rectangle(
                                x0 + n * (pos.X - 1) - s, y0 + n * (10 - pos.Y) - s,
                                s * 2, s * 2);

                            graphics.FillEllipse(brush, rect);
                            graphics.DrawEllipse(piece.Color == ChessPieceColor.Red ? rpen : bpen, rect);
                            graphics.DrawString(piece.Name, font,
                                piece.Color == ChessPieceColor.Red ? rbrush : bbrush,
                                rect);
                        }
                    }
                }
            }
        }

        private ChessPieceColor next_color = ChessPieceColor.Red;
        private void searchToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ChessSolver solver = new ChessSolver();
            // solver.Visit += solver_Visit;
            // ChessBoard init = this.board;
            ChessSolver.SearchNode next = solver.Solve_Checkmate(this.board, next_color, ChessPieceColor.Red);
            if (next != null)
            {
                this.board = next.Board;
                Invalidate();
                next_color = (next_color == ChessPieceColor.Red ? ChessPieceColor.Black : ChessPieceColor.Red);
            }
        }
    }
}
