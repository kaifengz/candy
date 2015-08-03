using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;

namespace Five
{
    using Coord = Point;

    public partial class MainForm : Form
    {
        enum PlayStatus
        {
            None,
            ComputerThinking,
            WaitingUser,
        }

        private ChessBoard board = new ChessBoard();
        private PlayStatus status = PlayStatus.None;
        private List<ChessBoard> history = new List<ChessBoard>();
        private List<Coord> gameoverReasons = null;

        private static readonly Point[] starPoints = new Point[]
        {
            new Point(3,3),
            new Point(3,11),
            new Point(11,3),
            new Point(11,11),
            new Point(7,7),
        };

        public MainForm()
        {
            InitializeComponent();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            PaintBoard(e.Graphics, board);
        }

        protected void PaintBoard(Graphics graphics, ChessBoard board)
        {
            float gridSize = 1.0f * Math.Min(ClientSize.Height, ClientSize.Width) / (ChessBoard.BoardSize + 1);
            float boardSize = gridSize * (ChessBoard.BoardSize - 1);
            float offset_x = (ClientSize.Width - boardSize) / 2;
            float offset_y = (ClientSize.Height - boardSize) / 2;

            PaintGrid(graphics, gridSize, boardSize, offset_x, offset_y);
            PaintChesses(graphics, gridSize, offset_x, offset_y);
            PaintStars(graphics, gridSize, offset_x, offset_y);
            PaintLastMark(graphics, gridSize, offset_x, offset_y);
            PaintGameoverReasons(graphics, gridSize, offset_x, offset_y);
        }

        private void PaintStars(Graphics graphics, float gridSize, float offset_x, float offset_y)
        {
            using (Brush blackBrush = new SolidBrush(Color.Black))
            {
                float starSize = gridSize / 6;
                foreach (Point star in starPoints)
                {
                    if (board[star.Y][star.X] == ChessType.None)
                    {
                        RectangleF rect = new RectangleF(
                                offset_x + star.X * gridSize - starSize / 2,
                                offset_y + star.Y * gridSize - starSize / 2,
                                starSize,
                                starSize);
                        graphics.FillEllipse(blackBrush, rect);
                    }
                }
            }
        }

        private void PaintGrid(Graphics graphics, float gridSize, float boardSize, float offset_x, float offset_y)
        {
            using (Pen blackPen = new Pen(Color.Black))
            {
                for (int i = 0; i < ChessBoard.BoardSize; ++i)
                {
                    graphics.DrawLine(blackPen, offset_x, offset_y + i * gridSize, offset_x + boardSize, offset_y + i * gridSize);
                    graphics.DrawLine(blackPen, offset_x + i * gridSize, offset_y, offset_x + i * gridSize, offset_y + boardSize);
                }
            }
        }

        private void PaintChesses(Graphics graphics, float gridSize, float offset_x, float offset_y)
        {
            float chessSize = gridSize * 8 / 10;
            using (Pen grayPen = new Pen(Color.DarkGray))
            {
                using (Brush grayBrush = new SolidBrush(Color.DarkGray),
                             whiteBrush = new SolidBrush(Color.White))
                {
                    for (int row = 0; row < ChessBoard.BoardSize; ++row)
                    {
                        for (int col = 0; col < ChessBoard.BoardSize; ++col)
                        {
                            RectangleF rect = new RectangleF(
                                    offset_x + col * gridSize - chessSize / 2,
                                    offset_y + row * gridSize - chessSize / 2,
                                    chessSize, chessSize);
                            switch (board[row][col])
                            {
                                case ChessType.None:
                                    break;
                                case ChessType.Black:
                                    graphics.FillEllipse(grayBrush, rect);
                                    break;
                                case ChessType.White:
                                    graphics.FillEllipse(whiteBrush, rect);
                                    graphics.DrawEllipse(grayPen, rect);
                                    break;
                            }
                        }
                    }
                }
            }
        }

        private void PaintLastMark(Graphics graphics, float gridSize, float offset_x, float offset_y)
        {
            if (!board.Last.IsEmpty)
            {
                float markSize = gridSize / 2;
                using (Pen greenPen = new Pen(Color.DarkGreen, 2))
                {
                    RectangleF rect = new RectangleF(
                            offset_x + board.Last.X * gridSize - markSize / 2,
                            offset_y + board.Last.Y * gridSize - markSize / 2,
                            markSize, markSize);
                    graphics.DrawEllipse(greenPen, rect);
                }
            }
        }

        private void PaintGameoverReasons(Graphics graphics, float gridSize, float offset_x, float offset_y)
        {
            if (gameoverReasons == null)
                return;

            float signSize = gridSize * 2 / 10;
            using (Brush redBrush = new SolidBrush(Color.Red))
            {
                foreach (Coord coord in gameoverReasons)
                {
                    RectangleF rect = new RectangleF(
                            offset_x + coord.X * gridSize - signSize / 2,
                            offset_y + coord.Y * gridSize - signSize / 2,
                            signSize, signSize);
                    graphics.FillEllipse(redBrush, rect);
                }
            }
        }

        protected override void OnResize(EventArgs e)
        {
            base.OnResizeEnd(e);
            Invalidate();
        }

        private void MainForm_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button != MouseButtons.Left)
                return;
            if (status != PlayStatus.WaitingUser)
                return;

            float gridSize = 1.0f * Math.Min(ClientSize.Height, ClientSize.Width) / (ChessBoard.BoardSize + 1);
            float boardSize = gridSize * (ChessBoard.BoardSize - 1);
            float offset_x = (ClientSize.Width - boardSize) / 2;
            float offset_y = (ClientSize.Height - boardSize) / 2;

            int x = (int)Math.Round((e.X - offset_x) / gridSize);
            int y = (int)Math.Round((e.Y - offset_y) / gridSize);

            history.Add(board.Clone());
            if (!board.TryPlace(board.NextChess, y, x))
                history.RemoveAt(history.Count - 1);
            else
            {
                if (board.GameOver)
                    GameOver(true);
                else
                {
                    status = PlayStatus.ComputerThinking;
                    timer1.Enabled = true;
                    Invalidate();
                }
            }
        }

        private void newGameStripMenuItem_Click(object sender, EventArgs e)
        {
            board = new ChessBoard();
            board.Place(ChessType.Black, ChessCrawler.GetOpeningMove(board));
            if (sender == blackToolStripMenuItem)
                board.Place(ChessType.White, ChessCrawler.GetOpeningMove(board));

            history.Clear();
            gameoverReasons = null;
            status = PlayStatus.WaitingUser;
            Invalidate();
        }

        private void undoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (history.Count > 0)
            {
                board = history.Last().Clone();
                history.RemoveAt(history.Count - 1);
                gameoverReasons = null;
                status = PlayStatus.WaitingUser;
                Invalidate();
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (status != PlayStatus.ComputerThinking)
                return;
            timer1.Enabled = false;

            this.Cursor = Cursors.WaitCursor;
            List<Point> moves = ChessCrawler.GetBestMove(board);
            this.Cursor = Cursors.Default;

            if (moves != null)
            {
                Debug.Assert(moves.Count > 0);
                board.Place(board.NextChess, moves[0].Y, moves[0].X);
                if (board.GameOver)
                {
                    GameOver(false);
                    return;
                }
            }

            status = PlayStatus.WaitingUser;
            Invalidate();
        }

        private void GameOver(bool userMoveLast)
        {
            Debug.Assert(board.GameOver);

            bool userIsBlack = (userMoveLast == (board.LastChess == ChessType.Black));
            bool userWin = (userIsBlack == board.Won);
            gameoverReasons = ChessEvaluator.GetGameOverReasons(board);

            status = PlayStatus.None;
            Invalidate();
            MessageBox.Show(userWin ? "Congratulations, You Win!" : "You Lost!");
        }
    }
}
