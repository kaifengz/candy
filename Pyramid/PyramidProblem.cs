using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;

namespace Pyramid
{
    abstract class Problem
    {
        public Problem()
        {
            Reset();
        }

        public Problem(Problem pp)
        {
            board = new char[pp.board.Length][];
            for (int i = 0; i < pp.board.Length; ++i)
                board[i] = (char[])pp.board[i].Clone();

            pieces = new Piece[pp.pieces.Length];
            for (int i = 0; i < pp.pieces.Length; ++i)
            {
                pieces[i] = (Piece)pp.pieces[i].Clone();
                pieces_by_color[pieces[i].Color] = pieces[i];
            }
        }

        public Piece[] Pieces { get { return pieces; } }
        public char[][] Board { get { return board; } }
        public string BoardString
        {
            get
            {
                return string.Join("\n", board.Select(line => string.Join("", line)));
            }
        }

        public void Reset()
        {
            init_board_and_pieces(get_raw_shapes(), false);
            foreach (Piece piece in pieces)
                Remove(piece);
        }

        public bool Place(Piece piece, int x, int y)
        {
            Debug.Assert(!piece.Placed);

            if (y < 0 || y + piece.Height > board.Length)
                return false;
            if (x < 0 || x >= board[y].Length)
                return false;

            char color = piece.Color;
            char[][] shape = piece.Shape;
            int width = piece.Width;
            int height = piece.Height;

            char[][] new_lines = new char[height][];
            for (int oy = 0; oy < height; ++oy)
            {
                int y1 = y + oy;
                char[] line = board[y1];
                char[] new_line = new char[line.Length];
                for (int i = 0; i < line.Length; ++i)
                    new_line[i] = line[i];
                for (int ox = 0; ox < width; ++ox)
                {
                    char c = shape[oy][ox];
                    if (c == ' ')
                        continue;
                    Debug.Assert(c == color);

                    int x1 = x + ox;
                    if (x1 >= line.Length)
                        return false;
                    if (line[x1] != ' ')
                        return false;
                    new_line[x1] = color;
                }
                new_lines[oy] = new_line;
            }

            for (int oy = 0; oy < height; ++oy)
                board[y + oy] = new_lines[oy];

            piece.Placed = true;
            piece.X = x;
            piece.Y = y;
            return true;
        }

        public bool Remove(int x, int y)
        {
            Debug.Assert(y >= 0 && y < board.Length);
            Debug.Assert(x >= 0 && x < board[y].Length);

            char color = board[y][x];
            if (color == ' ')
                return false;

            Piece piece = pieces_by_color[color];
            Debug.Assert(piece.Color == color);
            return Remove(piece);
        }

        public bool Remove(Piece piece)
        {
            Debug.Assert(piece.Placed);
            piece.Placed = false;

            char color = piece.Color;
            for (int h = 0; h < board.Length; ++h)
            {
                char[] line = board[h];
                for (int w = 0; w < line.Length; ++w)
                {
                    if (line[w] == color)
                        line[w] = ' ';
                }
            }
            return true;
        }

        public void Shuffle()
        {
            Random rand = new Random();
            foreach (Piece piece in pieces)
            {
                if (piece.Placed)
                    continue;

                int rotate = rand.Next(0, 3);
                for (int r=0; r<rotate; ++r)
                    piece.Rotate();
                if (rand.Next(0, 1) == 0)
                    piece.Mirror();
            }

            for (int i = 0; i < pieces.Length; ++i)
            {
                int j = rand.Next(0, pieces.Length);
                if (i != j)
                {
                    Piece t = pieces[i];
                    pieces[i] = pieces[j];
                    pieces[j] = t;
                }
            }
        }

        public override string ToString()
        {
            int len = 0;
            foreach (char[] line in board)
                len += line.Length + 1;

            char[] res = new char[len];
            int index = 0;
            foreach (char[] line in board)
            {
                foreach (char c in line)
                    res[index++] = c;
                res[index++] = '\n';
            }

            return new string(res);
        }

        public Problem FromString(string s)
        {
            string[] lines = s.Trim().Split('\n');
            if (lines.Length != board.Length)
                return null;

            for (int i = 0; i < lines.Length; ++i)
            {
                if (lines[i].Length != board[i].Length)
                    return null;
            }

            Problem new_problem = (Problem)Clone();
            new_problem.init_board_and_pieces(s, true);
            return new_problem;
        }

        public abstract void Setup(bool place_some=true, bool rotate_and_mirror_unplaced=true, bool reverse_order=true);
        public abstract object Clone();

        #region Privates
        protected char[][] board;
        protected Piece[] pieces;
        protected Dictionary<char, Piece> pieces_by_color = new Dictionary<char, Piece>();

        protected abstract string get_raw_shapes();

        private void init_board_and_pieces(string rshape, bool replace_pieces_only)
        {
            rshape = rshape.Trim();
            rshape = rshape.Replace("\r", "");
            rshape = rshape.Replace("\t", "");
            rshape = rshape.Replace(" ", "");

            if (!replace_pieces_only)
                Debug.Assert(!rshape.Contains('.'));

            string[] lines = rshape.Split('\n');
            int height = lines.Length;
            int width = 0;

            if (replace_pieces_only)
            {
                foreach (Piece piece in pieces)
                    Remove(piece);
            }
            else
            {
                board = new char[height][];
                for (int h = 0; h < height; ++h)
                {
                    char[] line = new char[lines[h].Length];
                    for (int w = 0; w < line.Length; ++w)
                        line[w] = lines[h][w];
                    board[h] = line;
                }
            }

            SortedSet<char> colors = new SortedSet<char>();
            foreach (string line in lines)
            {
                if (line.Length > width)
                    width = line.Length;
                foreach (char color in line)
                    colors.Add(color);
            }

            List<Piece> pieces_list = new List<Piece>(colors.Count());

            foreach (char color in colors)
            {
                int top = height - 1, left = width - 1, bottom = 0, right = 0;
                for (int h = 0; h < height; ++h)
                {
                    for (int w = 0; w < lines[h].Length; ++w)
                    {
                        if (lines[h][w] == color)
                        {
                            top = Math.Min(top, h);
                            left = Math.Min(left, w);
                            bottom = Math.Max(bottom, h);
                            right = Math.Max(right, w);
                        }
                    }
                }

                char[][] shape = new char[bottom + 1 - top][];
                for (int h = 0; h < shape.Length; ++h)
                {
                    shape[h] = new char[right + 1 - left];
                    int w = 0;
                    for (; w < shape[h].Length && w + left < lines[h + top].Length; ++w)
                    {
                        char c = lines[h+top][w+left];
                        shape[h][w] = (c == color ? color : ' ');
                    }
                    for (; w < shape[h].Length; ++w)
                        shape[h][w] = ' ';
                }
                Piece piece = new Piece(color, shape);
                piece.X = left;
                piece.Y = top;
                piece.Placed = true;

                pieces_list.Add(piece);
            }

            if (replace_pieces_only)
            {
                foreach (Piece piece in pieces_list)
                {
                    for (int i = 0; i < pieces.Length; ++i)
                    {
                        if (pieces[i].Color == piece.Color)
                        {
                            pieces[i] = piece;
                            Place(piece, piece.X, piece.Y);
                            break;
                        }
                    }
                }
            }
            else
            {
                pieces = pieces_list.ToArray();
            }

            foreach (Piece piece in pieces)
                pieces_by_color[piece.Color] = piece;
        }
        #endregion
    }

    class TriangleProblem : Problem
    {
        public TriangleProblem()
            : base()
        {
        }

        public TriangleProblem(TriangleProblem pp)
            : base(pp)
        {
        }

        private const string raw_shapes =
            @"AAABCCDDDD
		      AEEBBCCCD
		      AEEFGGGG
		      HHIFFFF
		      HIIIJJ
		      HHIJJ
		      KKKJ
		      LLK
		      LL
		      L";
        protected override string get_raw_shapes()
        {
            return raw_shapes;
        }

        public override object Clone()
        {
            return new TriangleProblem(this);
        }

        public override void Setup(bool place_some=true, bool rotate_and_mirror_unplaced=true, bool reverse_order=true)
        {
            if (place_some)
            {
                Place(pieces[0], 0, 0);
                Place(pieces[1], 3, 0);
                Place(pieces[2], 4, 0);
                Place(pieces[3], 6, 0);
                Place(pieces[4], 1, 1);
            }
            if (rotate_and_mirror_unplaced)
            {
                for (int i = 0; i < pieces.Length; ++i)
                {
                    if (!pieces[i].Placed)
                    {
                        pieces[i].Rotate();
                        pieces[i].Mirror();
                    }
                }
            }
            if (reverse_order)
                Array.Reverse(pieces);
        }
    }

    class RectangleProblem : Problem
    {
        public RectangleProblem()
            : base()
        {
        }

        public RectangleProblem(RectangleProblem pp)
            : base(pp)
        {
        }

        private const string raw_shapes =
            @"CKKLL
              CCKLL
              ACKIL
              ACIII
              AAAIF
              GFFFF
              GDJBB
              GDJJB
              GDDJJ
              HDHEE
              HHHEE";
        protected override string get_raw_shapes()
        {
            return raw_shapes;
        }

        public override object Clone()
        {
            return new RectangleProblem(this);
        }

        public override void Setup(bool place_some = true, bool rotate_and_mirror_unplaced = true, bool reverse_order = true)
        {
            if (place_some)
            {
                //Place(pieces[0], 0, 0);
                //Place(pieces[1], 3, 0);
                //Place(pieces[2], 4, 0);
                //Place(pieces[3], 6, 0);
                //Place(pieces[4], 1, 1);
            }
            if (rotate_and_mirror_unplaced)
            {
                for (int i = 0; i < pieces.Length; ++i)
                {
                    if (!pieces[i].Placed)
                    {
                        pieces[i].Rotate();
                        pieces[i].Mirror();
                    }
                }
            }
            if (reverse_order)
                Array.Reverse(pieces);
        }
    }

    class ProblemOp
    {
        public static uint GetHoleCount(Problem problem, int size_threshold = 3)
        {
            return get_hole_count(problem.Board, size_threshold);
        }

        public static uint GetHoleCount(char[][] board, int size_threshold = 3)
        {
            return get_hole_count(board, size_threshold);
        }

        #region privates
#if !USE_SLOW_HOLE_DETECTION_ALGORITHM
        struct Span
        {
            public int id;
            public int left, right;
            public Span(int id, int left, int right)
            {
                this.id = id;
                this.left = left;
                this.right = right;
            }
        };
        class SpanUnion
        {
            public int pid;
            public int size;
            public List<int> spans;
            public SpanUnion(int primary_id, int size = 0)
            {
                this.pid = primary_id;
                this.size = size;
                this.spans = new List<int>();
            }
        };

        private static uint get_hole_count(char[][] board, int size_threshold = 3)
        {
            List<Span> line1 = null;
            List<SpanUnion> unions = new List<SpanUnion>();
            int next_span_id = 0;

            for (int h = 0; h < board.Length; ++h)
            {
                char[] line = board[h];

                #region search the empty spans of the line
                List<Span> line2 = null;
                int pos = 0;
                while (pos < line.Length)
                {
                    int left = pos;
                    while (left < line.Length && line[left] != ' ')
                        ++left;
                    if (left >= line.Length)
                        break;

                    int right = left + 1;
                    while (right < line.Length && line[right] == ' ')
                        ++right;

                    if (line2 == null)
                        line2 = new List<Span>();
                    line2.Add(new Span(next_span_id, left, right));
                    Debug.Assert(unions.Count == next_span_id);
                    unions.Add(null);
                    ++next_span_id;
                    pos = right + 1;
                }
                #endregion

                if (line2 != null)
                {
                    Debug.Assert(line2.Count > 0);
                    int idx2 = 0;

                    if (line1 != null)
                    {
                        Debug.Assert(line1.Count > 0);
                        int idx1 = 0;
                        while (true)
                        {
                            Span span1 = line1[idx1];
                            Span span2 = line2[idx2];
                            SpanUnion union1 = unions[span1.id];
                            SpanUnion union2 = unions[span2.id];

                            bool adjacent = (
                                span1.right == span2.right ||
                                (span1.right > span2.left && span1.right < span2.right) ||
                                (span2.right > span1.left && span2.right < span1.right)
                                );
                            if (adjacent)
                            {
                                if (union2 == null)
                                {
                                    union2 = unions[span2.id] = union1;
                                    union1.size += span2.right - span2.left;
                                    union1.spans.Add(span2.id);
                                }
                                else if (union2.pid != union1.pid)
                                {
                                    SpanUnion merge_target = union2.pid > union1.pid ? union1 : union2;
                                    SpanUnion merge_source = union2.pid > union1.pid ? union2 : union1;
                                    merge_target.size += merge_source.size;
                                    foreach (int span_id in merge_source.spans)
                                        unions[span_id] = merge_target;
                                    union2 = merge_target;
                                    merge_source.size = 0;
                                    merge_source.spans.Clear();
                                }
                                else
                                {
                                    Debug.Assert(union1 == union2);
                                }
                            }

                            if (span1.right == span2.right)
                            {
                                ++idx1;
                                ++idx2;
                                if (idx1 >= line1.Count || idx2 >= line2.Count)
                                    break;
                            }
                            else if (span1.right < span2.right)
                            {
                                ++idx1;
                                if (idx1 >= line1.Count)
                                    break;
                            }
                            else
                            {
                                Debug.Assert(span1.right > span2.right);
                                if (union2 == null)
                                    union2 = unions[span2.id] = new SpanUnion(span2.id, span2.right - span2.left);
                                ++idx2;
                                if (idx2 >= line2.Count)
                                    break;
                            }
                        }
                    }

                    while (idx2 < line2.Count)
                    {
                        Span span2 = line2[idx2];
                        if (unions[span2.id] == null)
                        {
                            SpanUnion union2 = unions[span2.id] = new SpanUnion(span2.id, span2.right - span2.left);
                            union2.spans.Add(span2.id);
                        }
                        ++idx2;
                    }
                }

                line1 = line2;
            }

            uint small_hole_count = 0;
            HashSet<int> counted_union_pid = new HashSet<int>();
            foreach (SpanUnion union in unions)
            {
                if (!counted_union_pid.Contains(union.pid) && union.size > 0 && union.size < size_threshold)
                {
                    counted_union_pid.Add(union.pid);
                    ++small_hole_count;
                }
            }
            return small_hole_count;
        }

#else
        struct Cell
        {
            public int h, w;
            public Cell(int h, int w)
            {
                this.h = h;
                this.w = w;
            }
        };

        private static uint get_hole_count(char[][] board, int size_threshold = 3)
        {
            List<List<Cell>> holes = new List<List<Cell>>();
            for (int h = 0; h < board.Length; ++h)
            {
                char[] line = board[h];
                for (int w = 0; w < line.Length; ++w)
                {
                    if (line[w] == ' ')
                    {
                        List<Cell> hole = new List<Cell>();
                        hole.Add(new Cell(h, w));
                        holes.Add(hole);
                    }
                }
            }

            while (true)
            {
                bool any_merged = false;
                for (int a = 0; a < holes.Count; ++a)
                {
                    List<Cell> ahole = holes[a];
                    if (ahole.Count == 0)
                        continue;

                    for (int b = a + 1; b < holes.Count; ++b)
                    {
                        List<Cell> bhole = holes[b];
                        if (bhole.Count == 0)
                            continue;

                        bool adjacent = false;
                        for (int ia = 0; ia < ahole.Count; ++ia)
                        {
                            Cell acell = ahole[ia];
                            for (int ib = 0; ib < bhole.Count; ++ib)
                            {
                                Cell bcell = bhole[ib];
                                int hd = acell.h - bcell.h;
                                int wd = acell.w - bcell.w;
                                if (hd*hd + wd*wd == 1)
                                //if ((acell.h == bcell.h && Math.Abs(acell.w - bcell.w) == 1) ||
                                //    (Math.Abs(acell.h - bcell.h) == 1 && acell.w == bcell.w))
                                {
                                    adjacent = true;
                                    break;
                                }
                            }
                            if (adjacent)
                                break;
                        }
                        if (adjacent)
                        {
                            any_merged = true;
                            ahole.AddRange(bhole);
                            bhole.Clear();
                        }
                    }
                }
                if (!any_merged)
                    break;
            }

            uint small_hole_count = 0;
            foreach (List<Cell> hole in holes)
            {
                if (hole.Count > 0 && hole.Count < size_threshold)
                    ++small_hole_count;
            }

            return small_hole_count;
        }
#endif
        #endregion
    }
}
