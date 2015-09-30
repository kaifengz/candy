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
        private ChessType userChess = ChessType.None;
        private PlayStatus status = PlayStatus.None;
        private List<Coord> history = new List<Coord>();
        private List<Coord> gameoverReasons = null;
        private bool disableComputer = false;

        private static readonly Coord[] starPoints = new Coord[]
        {
            new Coord(3,3),
            new Coord(3,11),
            new Coord(11,3),
            new Coord(11,11),
            new Coord(7,7),
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
            PaintEvaluations(graphics, gridSize, offset_x, offset_y);
        }

        private void PaintStars(Graphics graphics, float gridSize, float offset_x, float offset_y)
        {
            using (Brush blackBrush = new SolidBrush(Color.Black))
            {
                float starSize = gridSize / 6;
                foreach (Coord star in starPoints)
                {
                    if (board[star] == ChessType.None)
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
                            switch (board.Get(col, row))
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

        private void PaintEvaluations(Graphics graphics, float gridSize, float offset_x, float offset_y)
        {
#if EVALUATIONS
            if (evaluations == null)
                return;

            using (Brush darkRed = new SolidBrush(Color.DarkRed),
                         white = new SolidBrush(Color.White))
            {
                float textHeight = this.Font.GetHeight(graphics);
                float textWidth = textHeight * 2;
                foreach (var evaluation in evaluations)
                {
                    Coord coord = evaluation.Item1;
                    Score score = evaluation.Item2;

                    string text = score.Category == ScoreCategory.Estimated ? score.score.ToString("0.0") :
                        score.Category == ScoreCategory.Won ? "Win" : "Lost";
                    RectangleF rect = new RectangleF(
                            offset_x + coord.X * gridSize - textWidth / 2,
                            offset_y + coord.Y * gridSize - textHeight / 2,
                            textWidth, textHeight);
                    graphics.FillRectangle(white, rect);
                    graphics.DrawString(text, this.Font, darkRed, rect);
                }
            }
#endif
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

            if (board.TryPlace(board.NextChess, y, x))
            {
                history.Add(board.Last);
                if (board.GameOver)
                    GameOver(true);
                else
                {
                    if (!disableComputer)
                    {
                        status = PlayStatus.ComputerThinking;
                        timer1.Enabled = true;
                    }
                    Invalidate();
                }
            }
        }

        private void newGameStripMenuItem_Click(object sender, EventArgs e)
        {
            userChess = sender == blackToolStripMenuItem ? ChessType.Black : ChessType.White;

            history.Clear();

            board = new ChessBoard();
            board.Place(ChessType.Black, ChessCrawler.GetOpeningMove(board));
            history.Add(board.Last);

            if (userChess == ChessType.Black)
            {
                board.Place(ChessType.White, ChessCrawler.GetOpeningMove(board));
                history.Add(board.Last);
            }

            gameoverReasons = null;
            status = PlayStatus.WaitingUser;
            Invalidate();
        }

        private void undoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Debug.Assert(history.Count == board.ChessCount);

            if (disableComputer)
            {
                if (board.ChessCount > 1)
                {
                    history.RemoveAt(history.Count - 1);
                    board.Undo(history[history.Count - 1]);
                }
            }
            else
            {
                if (userChess == ChessType.Black && board.ChessCount <= 2)
                    return;
                else if (userChess == ChessType.White && board.ChessCount <= 1)
                    return;

                do
                {
                    history.RemoveAt(history.Count - 1);
                    board.Undo(history[history.Count - 1]);
                }
                while (board.LastChess == userChess);
            }

            gameoverReasons = null;
            status = PlayStatus.WaitingUser;
            Invalidate();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            timer1.Enabled = false;

            if (status != PlayStatus.ComputerThinking)
                return;

            ComputerThink();
        }

        private void ComputerThink()
        {
            Debug.Assert(!disableComputer);

            this.Cursor = Cursors.WaitCursor;
            List<Coord> moves = ChessCrawler.GetBestMove(board);
            this.Cursor = Cursors.Default;

            if (moves != null)
            {
                Debug.Assert(moves.Count > 0);
                board.Place(board.NextChess, moves[0].Y, moves[0].X);
                history.Add(board.Last);
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
            gameoverReasons = board.Situation.GetGameOverReasons(board);

            status = PlayStatus.None;
            Invalidate();

            if (!disableComputer)
            {
                string message = userWin ? "Congratulations, You Win!" : "You Lost!";
                MessageBox.Show(message, Application.ProductName, MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void exchangeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (status != PlayStatus.WaitingUser)
                return;

            userChess = ChessBoard.ReverseChess(userChess);
            if (!disableComputer)
            {
                status = PlayStatus.ComputerThinking;
                timer1.Enabled = true;
            }
        }

        private void disableComputerToolStripMenuItem_Click(object sender, EventArgs e)
        {
            disableComputer = !disableComputer;
            disableComputerToolStripMenuItem.Checked = disableComputer;
        }
    }
}
