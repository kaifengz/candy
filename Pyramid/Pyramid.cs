using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.IO.Compression;
using System.Reflection;
using System.Threading;
using System.Windows.Forms;

namespace Pyramid
{
    public partial class PyramidForm : Form
    {
        #region Constants
        private const int xmargin = 20;
        private const int ymargin = 36;
        private const int radius = 12;
        private const int offset = 32;
        private const int tie_w = 10;
        private const int tie_h = 4;
        private const int shade_xoffset = 1;
        private const int shade_yoffset = 3;
        private static readonly Color shade_color = Color.DarkGray;
        private static readonly Color board_color = Color.LightGray;
        #endregion

        #region Privates
        enum ProblemType
        {
            Triangle,
            Rectangle,
        }

        enum HintType
        {
            NoHint,
            NormalHint,
            AdvancedHint,
        }

        private Problem problem;
        private Dictionary<char, Color> colors = new Dictionary<char, Color>();
        private Dictionary<Piece, Point> positions = new Dictionary<Piece, Point>();
        private List<Piece> unplaced_stack = new List<Piece>();
        private Piece moving_piece = null;
        private Point moving_offset;

        private double problem_rotation;
        private double problem_rotation_sin;
        private double problem_rotation_cos;
        private Point problem_origin;
        private int slot_columns;
        private int slot_rows;
        private int slot_width;
        private int slot_height;
        private Point slots_origin;
        private Point[] problem_outline;

        private HintType hint_type = HintType.NoHint;
        private Problem hint_problem = null;
        private Thread hint_thread = null;
        private Mutex hint_mutex = new Mutex();

        private List<string> all_solutions = new List<string>();
        private bool finding_all_solutions = false;
        private bool stop_finding = false;

        private List<string> embedded_solutions = new List<string>();

        private List<List<Challenge>> embedded_challenges = null;
        private int challenge_level = 1;
        private int max_challenge_level = 9;
        private Challenge current_challenge = null;

        Random random = new Random();
        #endregion

        public PyramidForm()
        {
            InitializeComponent();
            DoubleBuffered = true;

            InitProblem(ProblemType.Triangle);
            InitColors();

            loadSolutionsFromResource();
        }

        private void InitProblem(ProblemType type)
        {
            switch (type)
            {
                case ProblemType.Triangle:
                    int board_size = (int)Math.Round(offset * 11.5 / Math.Sqrt(2));
                    problem = new TriangleProblem();
                    problem_rotation = Math.PI / 4.0;
                    problem_origin = new Point(xmargin + board_size, ymargin + offset);
                    problem_outline = new Point[3];
                    problem_outline[0].X = xmargin;
                    problem_outline[0].Y = ymargin + board_size;
                    problem_outline[1].X = xmargin + board_size;
                    problem_outline[1].Y = ymargin;
                    problem_outline[2].X = xmargin + board_size * 2;
                    problem_outline[2].Y = ymargin + board_size;
                    slot_columns = 4;
                    slot_rows = 3;
                    slot_width = (int)Math.Round(board_size * 2.0 / slot_columns);
                    slot_height = slot_width;
                    slots_origin = new Point(xmargin + (int)Math.Round(slot_width / Math.Sqrt(2)), ymargin + board_size + 50);
                    break;

                case ProblemType.Rectangle:
                    int board_width = offset * 12;
                    int board_height = offset * 6;
                    problem = new RectangleProblem();
                    problem_rotation = Math.PI / 2.0;
                    problem_origin = new Point(xmargin + offset * 11, ymargin + offset);
                    problem_outline = new Point[4];
                    problem_outline[0].X = xmargin;
                    problem_outline[0].Y = ymargin;
                    problem_outline[1].X = xmargin + board_width;
                    problem_outline[1].Y = ymargin;
                    problem_outline[2].X = xmargin + board_width;
                    problem_outline[2].Y = ymargin + board_height;
                    problem_outline[3].X = xmargin;
                    problem_outline[3].Y = ymargin + board_height;
                    slot_columns = 3;
                    slot_rows = 4;
                    slot_width = (int)Math.Round(1.0 * board_width / slot_columns);
                    slot_height = slot_width;
                    slots_origin = new Point(xmargin + offset * 4, ymargin + board_height + 50);
                    break;
            }

            Debug.Assert(slot_columns * slot_rows == 12);
            problem_rotation_sin = Math.Sin(problem_rotation);
            problem_rotation_cos = Math.Cos(problem_rotation);

#if !DEBUG
#   if PROFILING
            problem.Setup(false);
#   else
            problem.Shuffle();
#   endif
#endif

            InitPiecePositions();
            OnHint();
        }

        private void InitPiecePositions()
        {
            unplaced_stack.Clear();
            for (int i = 0; i < problem.Pieces.Length; ++i)
            {
                Piece piece = problem.Pieces[i];
                if (piece.Placed)
                {
                    int x = piece.X * offset, y = piece.Y * offset;
                    BoardCoordToScreen(ref x, ref y);
                    positions[piece] = new Point(problem_origin.X + x, problem_origin.Y + y);
                }
                else
                {
                    positions[piece] = new Point(
                        slots_origin.X + unplaced_stack.Count % slot_columns * slot_width,
                        slots_origin.Y + unplaced_stack.Count / slot_columns * slot_height);
                    unplaced_stack.Add(piece);
                }
            }
        }

        private void InitColors()
        {
            colors['A'] = Color.FromArgb(64, 64, 196);
            colors['B'] = Color.FromArgb(255, 96, 96);
            colors['C'] = Color.FromArgb(128, 0, 255);
            colors['D'] = Color.FromArgb(64, 196, 64);
            colors['E'] = Color.FromArgb(224, 224, 0);
            colors['F'] = Color.FromArgb(255, 160, 128);
            colors['G'] = Color.FromArgb(32, 128, 32);
            colors['H'] = Color.FromArgb(0, 128, 255);
            colors['I'] = Color.FromArgb(255, 16, 16);
            colors['J'] = Color.FromArgb(224, 64, 32);
            colors['K'] = Color.FromArgb(96, 102, 96);
            colors['L'] = Color.FromArgb(0, 196, 196);
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            if (!finding_all_solutions)
            {
                if (keyData == Keys.F12)
                {
                    OnAutoComplete();
                    return true;
                }
                else if (keyData == Keys.F11)
                {
                    OnEnableSolutionMenu();
                    return true;
                }
            }

            return base.ProcessCmdKey(ref msg, keyData);
        }

        private void OnAutoComplete()
        {
            if (unplaced_stack.Count == 0)
            {

                problem.Reset();

#if !DEBUG
#   if PROFILING
                problem.Setup(false);
#   else
                problem.Shuffle();
#   endif
#endif

                InitPiecePositions();
                Invalidate();
                return;
            }

            Cursor.Current = Cursors.WaitCursor;

#if !DEBUG && PROFILING
            Solver solver = new Solver(problem);
            for (int i = 0; i < 1000; ++i)
            {
                problem.Reset();
                problem.Setup(false);
                foreach (bool result in solver.Solve())
                {
                    if (result)
                        break;
                }
            }
#else
            foreach (bool result in new Solver(problem).Solve())
            {
                if (result)
                    break;
            }
#endif

            InitPiecePositions();
            Cursor.Current = Cursors.Default;
            Invalidate();
        }

        private void OnEnableSolutionMenu()
        {
            solutionToolStripMenuItem.Visible = true;
        }

        protected override void OnPaint(PaintEventArgs arg)
        {
            base.OnPaint(arg);

            Graphics dc = arg.Graphics;

            // fill the background to white
            dc.FillRectangle(Brushes.White, ClientRectangle);

            // draw the board and empty cells
            DrawBoardAndEmptyCells(dc);

            // draw the pieces
            foreach (Piece piece in EnumPiecesByStackReverseOrder())
            {
                Point pos = positions[piece];
                DrawPieceWithShade(dc, piece, pos.X, pos.Y, piece == moving_piece ? true : false);
            }
        }

        private void DrawBoardAndEmptyCells(Graphics dc)
        {
            Pen black_pen = new Pen(Color.Black);
            Brush board_brush = new SolidBrush(board_color);
            Brush cell_brush = Brushes.White;

            dc.FillPolygon(board_brush, problem_outline);
            dc.DrawPolygon(black_pen, problem_outline);

            // draw the cells
            char[][] board = problem.Board;
            for (int h = 0; h < board.Length; ++h)
            {
                char[] line = board[h];
                for (int w = 0; w < line.Length; ++w)
                {
                    if (line[w] == ' ')
                    {
                        Rectangle rect = GetCellRectangle(problem_origin.X, problem_origin.Y, h, w);
                        dc.FillEllipse(cell_brush, rect);
                        dc.DrawEllipse(black_pen, rect);
                    }
                }
            }
        }

        private void DrawPieceWithShade(Graphics dc, Piece piece, int x0, int y0, bool shade = true)
        {
            if (shade)
            {
                DrawPiece(dc, piece, x0, y0, shade_color);
                DrawPiece(dc, piece, x0 - shade_xoffset, y0 - shade_yoffset, colors[piece.Color]);
            }
            else
            {
                DrawPiece(dc, piece, x0, y0, colors[piece.Color]);
            }
        }

        private void DrawPiece(Graphics dc, Piece piece, int x0, int y0, Color color)
        {
            Brush brush = new SolidBrush(color);

            char[][] shape = piece.Shape;
            for (int h = 0; h < shape.Length; ++h)
            {
                char[] line = shape[h];
                for (int w = 0; w < line.Length; ++w)
                {
                    if (line[w] != ' ')
                    {
                        Rectangle rect = GetCellRectangle(x0, y0, h, w);
                        dc.FillEllipse(brush, rect);
                    }
                }
            }

            Point[] points = new Point[4];
            for (int h = 0; h < shape.Length; ++h)
            {
                char[] line = shape[h];
                for (int w = 1; w < line.Length; ++w)
                {
                    if (line[w] != ' ' && line[w - 1] != ' ')
                    {
                        int ch = h * offset;
                        int cw = (w * 2 - 1) * offset / 2;
                        GetTiePolygon(x0, y0, cw, ch, tie_w, tie_h, points);
                        dc.FillPolygon(brush, points);
                    }
                }
            }
            for (int h = 1; h < shape.Length; ++h)
            {
                char[] line = shape[h];
                for (int w = 0; w < line.Length; ++w)
                {
                    if (shape[h][w] != ' ' && shape[h - 1][w] != ' ')
                    {
                        int ch = (h * 2 - 1) * offset / 2;
                        int cw = w * offset;
                        GetTiePolygon(x0, y0, cw, ch, tie_h, tie_w, points);
                        dc.FillPolygon(brush, points);
                    }
                }
            }
        }

        private void BoardCoordToScreen(ref int x, ref int y)
        {
            int newx = (int)Math.Round(x * problem_rotation_cos - y * problem_rotation_sin);
            int newy = (int)Math.Round(x * problem_rotation_sin + y * problem_rotation_cos);
            x = newx;
            y = newy;
        }

        private void ScreenCoordToBoard(ref int x, ref int y)
        {
            int newx = (int)Math.Round(y * problem_rotation_sin + x * problem_rotation_cos);
            int newy = (int)Math.Round(y * problem_rotation_cos - x * problem_rotation_sin);
            x = newx;
            y = newy;
        }

        private Rectangle GetCellRectangle(int x0, int y0, int h, int w)
        {
            int x = w * offset, y = h * offset;
            BoardCoordToScreen(ref x, ref y);
            return new Rectangle(x0 + x - radius, y0 + y - radius, radius * 2, radius * 2);
        }

        private void GetTiePolygon(int x0, int y0, int cx, int cy, int tie_width, int tie_height, Point[] points)
        {
            Debug.Assert(points.Length == 4);
            points[0].X = cx - tie_width;
            points[0].Y = cy - tie_height;
            points[1].X = cx + tie_width;
            points[1].Y = cy - tie_height;
            points[2].X = cx + tie_width;
            points[2].Y = cy + tie_height;
            points[3].X = cx - tie_width;
            points[3].Y = cy + tie_height;
            for (int i=0; i<points.Length; ++i)
            {
                int x = points[i].X, y = points[i].Y;
                BoardCoordToScreen(ref x, ref y);
                points[i].X = x0 + x;
                points[i].Y = y0 + y;
            }
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);

            if (finding_all_solutions)
                return;

            foreach (Piece piece in EnumPiecesByStackOrder())
            {
                Point pos = positions[piece];
                if (HitTest(piece, pos, e.Location))
                {
                    switch (e.Button)
                    {
                        case MouseButtons.Left:
                            OnPieceLButtonDown(piece, pos, e.Location);
                            break;

                        case MouseButtons.Right:
                            OnPieceRButtonDown(piece, pos, e.Location);
                            break;

                        case MouseButtons.Middle:
                            OnPieceMButtonDown(piece, pos, e.Location);
                            break;
                    }

                    break;
                }
            }
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);

            if (finding_all_solutions)
                return;

            if (moving_piece != null)
            {
                positions[moving_piece] = new Point(
                    e.Location.X - moving_offset.X,
                    e.Location.Y - moving_offset.Y);
                Invalidate();
            }
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            base.OnMouseUp(e);

            if (finding_all_solutions)
                return;

            if (e.Button == MouseButtons.Left)
            {
                if (moving_piece != null)
                {
                    Point pos = new Point(
                        e.Location.X - moving_offset.X,
                        e.Location.Y - moving_offset.Y);
                    positions[moving_piece] = pos;

                    OnDropPiece(moving_piece, pos);
                    moving_piece = null;
                    Invalidate();
                }
            }
        }

        private void OnPieceLButtonDown(Piece piece, Point piecePos, Point mousePos)
        {
            if (piece.Placed)
            {
                problem.Remove(piece);
                OnHint();
            }

            moving_piece = piece;
            moving_offset = new Point(mousePos.X - piecePos.X, mousePos.Y - piecePos.Y);
            BringToStackTop(piece);
            Invalidate();

            Rectangle clip = RectangleToScreen(ClientRectangle);
            clip.Y += menuStripMain.Height;
            clip.Height -= menuStripMain.Height;
            Cursor.Clip = clip;
        }

        private void OnPieceRButtonDown(Piece piece, Point piecePos, Point mousePos)
        {
            if (piece.Placed)
                problem.Remove(piece);
            if (moving_piece != null)
                Debug.Assert(moving_piece == piece);

            Point current_offset = new Point(mousePos.X - piecePos.X, mousePos.Y - piecePos.Y);
            int height = (piece.Height - 1) * offset;
            Point new_offset = new Point(
                (int)(height * problem_rotation_cos - current_offset.Y),
                (int)(height * problem_rotation_sin + current_offset.X));
            Point new_pos = new Point(
                mousePos.X - new_offset.X,
                mousePos.Y - new_offset.Y);

            positions[piece] = new_pos;
            piece.Rotate();

            if (moving_piece == piece)
                moving_offset = new_offset;
            else
                OnDropPiece(piece, new_pos);

            Invalidate();
        }

        private void OnPieceMButtonDown(Piece piece, Point piecePos, Point mousePos)
        {
            if (piece.Placed)
                problem.Remove(piece);
            if (moving_piece != null)
                Debug.Assert(moving_piece == piece);

            Point new_offset, new_pos;
            Point offset = new Point(mousePos.X - piecePos.X, mousePos.Y - piecePos.Y);

            if (piecePos.Equals(mousePos))
            {
                new_offset = offset;
                new_pos = piecePos;
            }
            else
            {
                double distance = Math.Sqrt(offset.X * offset.X + offset.Y * offset.Y);
                double current_sin_offset = offset.Y / distance;
                double current_cos_offset = offset.X / distance;
                double rotation_sin2 = 2 * problem_rotation_sin * problem_rotation_cos;
                double rotation_cos2 = 2 * problem_rotation_cos * problem_rotation_cos - 1;
                double new_sin_offset = current_cos_offset * rotation_cos2 + current_sin_offset * rotation_sin2;
                double new_cos_offset = current_sin_offset * rotation_cos2 - current_cos_offset * rotation_sin2;
                new_offset = new Point(
                    (int)(distance * new_cos_offset),
                    (int)(distance * new_sin_offset));
                new_pos = new Point(
                    mousePos.X - new_offset.X,
                    mousePos.Y - new_offset.Y);
            }

            positions[piece] = new_pos;
            piece.Mirror();

            if (moving_piece == piece)
                moving_offset = new_offset;
            else
                OnDropPiece(piece, new_pos);

            Invalidate();
        }

        private bool HitTest(Piece piece, Point piecePos, Point mousePos)
        {
            Point poffset = new Point(mousePos.X - piecePos.X, mousePos.Y - piecePos.Y);
            char[][] shape = piece.Shape;
            for (int h = 0; h < shape.Length; ++h)
            {
                for (int w = 0; w < shape[h].Length; ++w)
                {
                    if (shape[h][w] == ' ')
                        continue;
                    int x = w * offset, y = h * offset;
                    BoardCoordToScreen(ref x, ref y);
                    double distance = Math.Sqrt(Math.Pow(poffset.X - x, 2) + Math.Pow(poffset.Y - y, 2));
                    if (distance <= offset/2)
                        return true;
                }
            }
            return false;
        }

        private void OnDropPiece(Piece piece, Point pos)
        {
            int x = pos.X - problem_origin.X;
            int y = pos.Y - problem_origin.Y;
            ScreenCoordToBoard(ref x, ref y);

            int w = (int)Math.Round(1.0 * x / offset);
            int h = (int)Math.Round(1.0 * y / offset);

            if (problem.Place(piece, w, h))
            {
                unplaced_stack.Remove(piece);
                x = w * offset;
                y = h * offset;
                BoardCoordToScreen(ref x, ref y);
                positions[piece] = new Point(problem_origin.X + x, problem_origin.Y + y);
            }
            else
            {
                BringToStackTop(piece);
            }

            OnHint();

            Cursor.Clip = Rectangle.Empty;
        }

        private void OnHint()
        {
            if (hint_type == HintType.NoHint)
                Hint(null);
            else
            {
                lock (hint_mutex)
                {
                    if (hint_thread == null)
                    {
                        hint_thread = new Thread(new ThreadStart(HintThread));
                        hint_thread.IsBackground = true;
                        hint_thread.Start();
                    }
                    hint_problem = (Problem)problem.Clone();
                }
            }
        }

        private void HintThread()
        {
            while (true)
            {
                Problem problem = null;
                HintType type;
                lock (hint_mutex)
                {
                    problem = hint_problem;
                    type = hint_type;
                    hint_problem = null;
                }

                if (problem == null)
                {
                    Thread.Sleep(10);
                    continue;
                }

                if (type == HintType.AdvancedHint && problem is RectangleProblem)
                    type = HintType.NoHint;

                if (type == HintType.NoHint)
                    Hint(null);
                else
                {
                    int placed_pieces = 0;
                    foreach (Piece piece in problem.Pieces)
                    {
                        if (piece.Placed)
                            ++placed_pieces;
                    }

                    string message = null;
                    if (placed_pieces != 0 && placed_pieces != problem.Pieces.Length)
                    {
                        if (type == HintType.NormalHint)
                        {
                            bool solvable = false;
                            foreach (bool result in new Solver(problem).Solve())
                            {
                                if (result)
                                {
                                    solvable = true;
                                    break;
                                }
                            }
                            if (!solvable)
                                message = "Unsolvable";
                        }
                        else
                        {
                            string tmp = problem.ToString();
                            tmp = tmp.Substring(0, tmp.Length - 1);
                            int count = 0;
                            foreach (string solution in embedded_solutions)
                            {
                                if (ProblemIsPartOf(tmp, solution))
                                    ++count;
                            }
                            if (count == 0)
                                message = "Unsolvable";
                            else if (count == 1)
                                message = "1 possible solution";
                            else
                                message = count.ToString() + " possible solutions";
                        }
                    }

                    bool hint_still_valid;
                    lock (hint_mutex)
                    {
                        hint_still_valid = (hint_problem == null);
                    }

                    if (hint_still_valid)
                        Hint(message);
                }
            }
        }

        bool ProblemIsPartOf(string part, string solution)
        {
            if (part.Length != solution.Length)
                return false;

            for (int i = 0; i < part.Length; ++i)
            {
                char p = part[i];
                char s = solution[i];
                if (p != s && s != '\n' && p != ' ')
                    return false;
            }
            return true;
        }

        delegate void HintProc(string msg);
        private void Hint(string msg)
        {
            if (InvokeRequired)
            {
                Invoke(new HintProc(Hint), msg);
                return;
            }

            this.Text = (msg == null ? "Pyramid" : "Pyramid - " + msg);
        }

        private void BringToStackTop(Piece piece)
        {
            Debug.Assert(!piece.Placed);
            unplaced_stack.Remove(piece);
            unplaced_stack.Insert(0, piece);
        }

        private IEnumerable<Piece> EnumPiecesByStackOrder()
        {
            for (int i = 0; i < unplaced_stack.Count; ++i)
            {
                Piece piece = unplaced_stack[i];
                Debug.Assert(!piece.Placed);
                yield return piece;
            }
            foreach (Piece piece in problem.Pieces)
            {
                if (piece.Placed)
                    yield return piece;
            }
        }

        private IEnumerable<Piece> EnumPiecesByStackReverseOrder()
        {
            foreach (Piece piece in problem.Pieces)
            {
                if (piece.Placed)
                    yield return piece;
            }
            for (int i = unplaced_stack.Count - 1; i >= 0; --i)
            {
                Piece piece = unplaced_stack[i];
                Debug.Assert(!piece.Placed);
                yield return piece;
            }
        }

        private void problemStyleToolStripMenuItem_Click(object sender, EventArgs e)
        {
            bool want_triangle = (sender == triangleStyleToolStripMenuItem);
            bool was_triangle = (problem is TriangleProblem);
            if (want_triangle != was_triangle)
            {
                InitProblem(want_triangle ? ProblemType.Triangle : ProblemType.Rectangle);
                triangleStyleToolStripMenuItem.Checked = want_triangle;
                rectangleStyleToolStripMenuItem.Checked = !want_triangle;
                advancedHintToolStripMenuItem.Enabled = want_triangle;
                Invalidate();
            }
        }

        private void hintToolStripMenuItem_Click(object sender, EventArgs e)
        {
            hint_type = (sender == hintOnToolStripMenuItem ? HintType.NormalHint : sender == advancedHintToolStripMenuItem ? HintType.AdvancedHint : HintType.NoHint);
            hintOnToolStripMenuItem.Checked = (hint_type == HintType.NormalHint);
            hintOffToolStripMenuItem.Checked = (hint_type == HintType.NoHint);
            advancedHintToolStripMenuItem.Checked = (hint_type == HintType.AdvancedHint);
            OnHint();
        }

        private void solveItToolStripMenuItem_Click(object sender, EventArgs e)
        {
            bool solveable = false;

            if (unplaced_stack.Count == 0)
                solveable = true;
            else
            {
                Cursor.Current = Cursors.WaitCursor;
                foreach (bool result in new Solver(problem).Solve())
                {
                    if (result)
                    {
                        solveable = true;
                        break;
                    }
                }
                Cursor.Current = Cursors.Default;
            }

            if (solveable)
            {
                InitPiecePositions();
                Invalidate();
            }
            else
            {
                MessageBox.Show("Unsolvable!");
            }
        }

        private void findAllSolutionsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (!(problem is TriangleProblem))
                return;

            triangleStyleToolStripMenuItem.Enabled = false;
            rectangleStyleToolStripMenuItem.Enabled = false;
            solveItToolStripMenuItem.Enabled = false;
            hintOnToolStripMenuItem.Enabled = false;
            findAllSolutionsToolStripMenuItem.Enabled = false;
            stopFindingToolStripMenuItem.Enabled = true;
            solutionIndexToolStripTextBox.Visible = true;
            solutionIndexToolStripTextBox.ReadOnly = true;
            finding_all_solutions = true;
            stop_finding = false;

            Thread finding_thread = new Thread(new ThreadStart(FindingAllSolutionThread));
            finding_thread.IsBackground = true;
            finding_thread.Start();
        }

        private void stopFindingToolStripMenuItem_Click(object sender, EventArgs e)
        {
            stop_finding = true;
        }

        delegate void reportFindingAllSolutionResultProc(int count, bool finished);
        private void reportFindingAllSolutionResult(int count, bool finished)
        {
            if (InvokeRequired)
            {
                reportFindingAllSolutionResultProc proc = reportFindingAllSolutionResult;
                Invoke(proc, count, finished);
                return;
            }

            solutionIndexToolStripTextBox.Text = count.ToString();
            if (finished)
            {
                triangleStyleToolStripMenuItem.Enabled = true;
                rectangleStyleToolStripMenuItem.Enabled = true;
                solveItToolStripMenuItem.Enabled = true;
                hintOnToolStripMenuItem.Enabled = true;
                solutionIndexToolStripTextBox.ReadOnly = false;
                stopFindingToolStripMenuItem.Enabled = false;
                saveSolutionsAsToolStripMenuItem.Enabled = true;
                finding_all_solutions = false;
                stop_finding = false;

                showSolution(count - 1);
            }
        }

        private void FindingAllSolutionThread()
        {
            // Problem problem = (triangleStyleToolStripMenuItem.Checked ? (Problem)new TriangleProblem() : (Problem)new RectangleProblem());
            Problem problem = new TriangleProblem();
            foreach (bool result in new Solver(problem).Solve())
            {
                if (result)
                {
                    all_solutions.Add(problem.ToString());

                    int count = all_solutions.Count;
                    if (count % 97 == 1 || count % 101 == 31 || count % 157 == 67)
                        reportFindingAllSolutionResult(all_solutions.Count, false);
                }

                if (stop_finding)
                    break;
            }

            reportFindingAllSolutionResult(all_solutions.Count, true);
        }

        private void solutionIndexToolStripTextBox_TextChanged(object sender, EventArgs e)
        {
            if (finding_all_solutions)
                return;

            int index = 0;
            if (!tryParseInt(solutionIndexToolStripTextBox.Text, out index))
                return;

            if (index <= 0 || index > all_solutions.Count)
                return;

            showSolution(index - 1);
        }

        private void solutionIndexToolStripTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Up || e.KeyCode == Keys.Down)
            {
                int index = 0;
                if (!tryParseInt(solutionIndexToolStripTextBox.Text, out index))
                    return;

                if (e.KeyCode == Keys.Up)
                    ++index;
                else
                    --index;

                if (index <= 0)
                    index = 1;
                else if (index > all_solutions.Count)
                    index = all_solutions.Count;

                solutionIndexToolStripTextBox.Text = index.ToString();
            }
        }

        private void showSolution(int index)
        {
            Debug.Assert(index >= 0 && index < all_solutions.Count);

            Problem solution = problem.FromString(all_solutions[index]);
            if (solution != null)
            {
                problem = solution;
                InitPiecePositions();
                Invalidate();
                OnHint();
            }
        }

        private static bool tryParseInt(string s, out int n)
        {
            s = s.Trim();
            return int.TryParse(s, out n);
        }

        private void saveSolutionsAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
#if DONOT_INCLUDE
            TextWriter writer = new StreamWriter("solutions.txt");
            foreach (string solution in all_solutions)
                writer.WriteLine(solution.Replace('\n', ' '));
            writer.Close();
#endif
        }

        private void loadSolutionsFromResource()
        {
            byte[] zipped_solutions = Properties.Resources.solutions_gz;
            Stream stream = new GZipStream(new MemoryStream(zipped_solutions), CompressionMode.Decompress);
            StreamReader reader = new StreamReader(stream);
            while (!reader.EndOfStream)
            {
                embedded_solutions.Add(reader.ReadLine().Trim().Replace(' ', '\n'));
            }
        }

        private List<ToolStripMenuItem> levels = new List<ToolStripMenuItem>();
        private void levelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (levels.Count == 0)
            {
                levels.Add(level1ToolStripMenuItem);
                levels.Add(level2ToolStripMenuItem);
                levels.Add(level3ToolStripMenuItem);
                levels.Add(level4ToolStripMenuItem);
                levels.Add(level5ToolStripMenuItem);
                levels.Add(level6ToolStripMenuItem);
                levels.Add(level7ToolStripMenuItem);
                levels.Add(level8ToolStripMenuItem);
                levels.Add(level9ToolStripMenuItem);
            }

            ToolStripMenuItem item = (ToolStripMenuItem)sender;
            for (int i = 0; i < levels.Count; ++i)
            {
                if (item != levels[i])
                    levels[i].Checked = false;
                else
                {
                    levels[i].Checked = true;
                    challenge_level = i + 1;
                }
            }
        }

        class ChallengeComparer : IComparer<Challenge>
        {
            public int Compare(Challenge x, Challenge y)
            {
                return Math.Sign(x.Difficulty - y.Difficulty);
            }
        }
        private void FindAllChallengesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DateTime start = DateTime.Now;

            ChallengeGenerator generator = new ChallengeGenerator();
            List<Challenge> challenges = generator.GenerateChallenges(embedded_solutions);
            challenges.Sort(new ChallengeComparer());
            challenges.Reverse();
            SaveChallenges("challenges.txt", challenges);

            MessageBox.Show(string.Format("{0} challenges found in {1} seconds", challenges.Count, (DateTime.Now - start).TotalSeconds));
            generator = null;
            challenges = null;
            GC.Collect();
        }

        private void SaveChallenges(string file_name, List<Challenge> challenges)
        {
            TextWriter writer = new StreamWriter(file_name);
            foreach (Challenge challenge in challenges)
            {
                writer.WriteLine(challenge.Pattern.Replace(' ', '.').Replace('\n', ' '));
                writer.WriteLine(challenge.SolutionCount);
            }
            writer.Close();
        }

        enum ChallengeSource
        {
            Resource,
            ExternalFile,
        }
        private List<Challenge> LoadChallenges(ChallengeSource source)
        {
            Stream fs = null;

            switch (source)
            {
                case ChallengeSource.Resource:
                    fs = new GZipStream(new MemoryStream(Properties.Resources.challenges_gz), CompressionMode.Decompress);
                    break;

                case ChallengeSource.ExternalFile:
                    fs = new FileStream("challenges.txt", FileMode.Open);
                    break;
            }

            List<Challenge> challenges = new List<Challenge>();
            StreamReader reader = new StreamReader(fs);
            while (!reader.EndOfStream)
            {
                string pattern = reader.ReadLine().Trim().Replace(' ', '\n').Replace('.', ' ');
                int solution_count = int.Parse(reader.ReadLine().Trim());

                HashSet<char> colors = new HashSet<char>(pattern);
                colors.Remove(' ');
                colors.Remove('\n');

                Challenge challenge = new Challenge(pattern, colors.Count, solution_count);
                challenges.Add(challenge);
            }

            return challenges;
        }

        private List<List<Challenge>> SortChallengesByDifficulty(List<Challenge> challenges, int levels)
        {
            Debug.Assert(levels == 9);

            List<List<Challenge>> result = new List<List<Challenge>>(levels);
            for (int level = 0; level < levels; ++level)
                result.Add(new List<Challenge>());

            foreach (Challenge challenge in challenges)
            {
                if (challenge.SolutionCount > 4 || challenge.PlacedPieces >= 11)
                    continue;

                int level;
                if (challenge.PlacedPieces <= 2)
                    level = 8;
                else
                    level = 10 - challenge.PlacedPieces;
                result[level].Add(challenge);
            }

            return result;
        }

        private void generateAChallengePackageToolStripMenuItem_Click(object sender, EventArgs e)
        {
            List<Challenge> challenges = LoadChallenges(ChallengeSource.ExternalFile);
            List<List<Challenge>> sorted = SortChallengesByDifficulty(challenges, max_challenge_level);
            List<Challenge> selected = new List<Challenge>();
            foreach (List<Challenge> level in sorted)
            {
                for (int j = 0; j < Math.Min(2000, level.Count); ++j)
                    selected.Add(level[random.Next(level.Count)]);
            }

            selected.Sort(new ChallengeComparer());
            selected.Reverse();
            SaveChallenges("selected.txt", selected);
        }

        private void newChallengeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (!(problem is TriangleProblem))
                return;

            if (embedded_challenges == null)
                embedded_challenges = SortChallengesByDifficulty(LoadChallenges(ChallengeSource.Resource), max_challenge_level);

            List<Challenge> level = embedded_challenges[challenge_level - 1];
            Challenge challenge = level[random.Next(level.Count)];
            Problem new_problem = problem.FromString(challenge.Pattern.Replace(' ', '.'));
            if (new_problem != null)
            {
                current_challenge = challenge;
                problem = new_problem;
                restartChallengeToolStripMenuItem.Enabled = true;
                InitPiecePositions();
                Invalidate();
                OnHint();
            }
        }

        private void restartChallengeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (current_challenge != null)
            {
                Problem new_problem = problem.FromString(current_challenge.Pattern.Replace(' ', '.'));
                if (new_problem != null)
                {
                    problem = new_problem;
                    InitPiecePositions();
                    Invalidate();
                    OnHint();
                }
            }
        }
    }
}
