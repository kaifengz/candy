using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;

namespace Five
{
    using Coord = System.Drawing.Point;
    using Offset = System.Drawing.Point;

    public enum PatternType
    {
        None = 0,
        Two_S,
        Two, // 可以进一步细化————比如说该活二的周围有五个还是六个可能的空间，以及该活二是挨着的还是跳开的
        Three_S,
        Three,
        Four_S,
        Four,
        Five,
        ToBeSixOrMore,
        SixOrMore,
        Max
    }

    public enum ForbiddenPointType
    {
        None,
        DoubleThree,
        DoubleFour,
        SixOrMore,
    }

    [DebuggerDisplay("{chess} {start} - {end} {pattern}")]
    public class Pattern : IComparable
    {
        public ChessType chess;
        public Coord start, end;
        public PatternType pattern;

        public Pattern(ChessType chess_, Coord start_, Coord end_, PatternType pattern_)
        {
            chess = chess_;
            start = start_;
            end = end_;
            pattern = pattern_;
        }

        public int CompareTo(object obj)
        {
            Pattern a = this, b = (Pattern)obj;
            if (a.chess != b.chess)
                return (int)a.chess - (int)b.chess;
            if (a.start != b.start)
                return a.start.X != b.start.X ? a.start.X - b.start.X : a.start.Y - b.start.Y;
            if (a.end != b.end)
                return a.end.X != b.end.X ? a.end.X - b.end.X : a.end.Y - b.end.Y;
            return (int)a.pattern - (int)b.pattern;
        }

        public override bool Equals(object obj)
        {
            return CompareTo(obj) == 0;
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public bool CoverCoord(Coord coord)
        {
            if (coord == start)
                return true;

            int dx1 = start.X - coord.X;
            int dx2 = coord.X - end.X;
            int dy1 = start.Y - coord.Y;
            int dy2 = coord.Y - end.Y;

            bool in_same_line = (dx1 * dy2 == dx2 * dy1);
            bool x_in_range = (dx1 * dx2 > 0);
            bool y_in_range = (dy1 * dy2 > 0);
            return in_same_line && (x_in_range || y_in_range);
        }

        public bool InSameLine(Coord coord)
        {
            int dx1 = start.X - coord.X;
            int dx2 = coord.X - end.X;
            int dy1 = start.Y - coord.Y;
            int dy2 = coord.Y - end.Y;

            bool in_same_line = (dx1 * dy2 == dx2 * dy1);
            return in_same_line;
        }

        public IEnumerable<Coord> AllCoord(ChessBoard board)
        {
            int ox = Math.Sign(end.X - start.X);
            int oy = Math.Sign(end.Y - start.Y);

            for (int i = 0; ; ++i)
            {
                Coord coord = new Coord(start.X + ox * i, start.Y + oy * i);
                if (coord == end)
                    break;
                if (board[coord] == this.chess)
                    yield return coord;
            }
        }

        public IEnumerable<Coord> GetComplements(ChessBoard board)
        {
            int dx = end.X - start.X;
            int dy = end.Y - start.Y;
            int ox = Math.Sign(dx);
            int oy = Math.Sign(dy);
            Debug.Assert(dx == 0 || dy == 0 || Math.Abs(dx) == Math.Abs(dy));
            Debug.Assert(dx != 0 || dy != 0);

            int length = Math.Max(Math.Abs(dx), Math.Abs(dy));
            int count;
            if (pattern == PatternType.Two)
                count = 2;
            else if (pattern == PatternType.Three || pattern == PatternType.Three_S)
                count = 3;
            else if (pattern == PatternType.Four || pattern == PatternType.Four_S)
                count = 4;
            else if (pattern == PatternType.ToBeSixOrMore)
                count = length - 1;
            else
                yield break;

            Debug.Assert(board[start] != ChessType.None);
            Debug.Assert(board.Get(start.X + ox * (length - 1), start.Y + oy * (length - 1)) != ChessType.None);
            if (count < length)
            {
                for (int i = 1; i < length - 1; ++i)
                {
                    Coord coord = new Coord(start.X + ox * i, start.Y + oy * i);
                    if (board[coord] == ChessType.None)
                        yield return coord;
                }
            }

            if (count <= 4)
            {
                int look_out = Math.Min(2 - (length - count), 5 - count);
                Debug.Assert(look_out >= 0 && look_out <= 2);
                for (int i = 1; i <= look_out; ++i)
                {
                    Coord coord = new Coord(start.X - ox * i, start.Y - oy * i);
                    if (!ChessBoard.IsValidCoord(coord) || board[coord] != ChessType.None)
                        break;
                    yield return coord;
                }
                for (int i = 0; i < look_out; ++i)
                {
                    Coord coord = new Coord(end.X + ox * i, end.Y + oy * i);
                    if (!ChessBoard.IsValidCoord(coord) || board[coord] != ChessType.None)
                        break;
                    yield return coord;
                }
            }
        }

        [TestClass]
        public class UnitTest
        {
            [TestMethod]
            public void Test_CoverCoord()
            {
                Pattern p = new Pattern(ChessType.Black, new Coord(7, 7), new Coord(11, 11), PatternType.Four);

                for (int i = 7; i < 11; ++i)
                {
                    Assert.IsTrue(p.CoverCoord(new Coord(i, i)));
                    Assert.IsFalse(p.CoverCoord(new Coord(i, i + 1)));
                    Assert.IsFalse(p.CoverCoord(new Coord(i, i - 1)));
                    Assert.IsFalse(p.CoverCoord(new Coord(i - 1, i)));
                    Assert.IsFalse(p.CoverCoord(new Coord(i + 1, i)));
                }
                Assert.IsFalse(p.CoverCoord(new Coord(6, 6)));
                Assert.IsFalse(p.CoverCoord(new Coord(11, 11)));
            }

            [TestMethod]
            public void Test_InSameLine()
            {
                Pattern p = new Pattern(ChessType.Black, new Coord(7, 7), new Coord(11, 11), PatternType.Four);

                for (int i = 7; i < 11; ++i)
                {
                    Assert.IsTrue(p.InSameLine(new Coord(i, i)));
                    Assert.IsFalse(p.InSameLine(new Coord(i, i + 1)));
                    Assert.IsFalse(p.InSameLine(new Coord(i, i - 1)));
                    Assert.IsFalse(p.InSameLine(new Coord(i - 1, i)));
                    Assert.IsFalse(p.InSameLine(new Coord(i + 1, i)));
                }
                Assert.IsTrue(p.InSameLine(new Coord(6, 6)));
                Assert.IsTrue(p.InSameLine(new Coord(11, 11)));
            }
        }
    }

    public class Situation
    {
        public List<Pattern> BlackPatterns
        {
            get
            {
                return blacks;
            }
        }

        public List<Pattern> WhitePatterns
        {
            get
            {
                return whites;
            }
        }

        public List<Pattern>[] BlackPatternsByType
        {
            get
            {
                return blacksByType;
            }
        }

        public List<Pattern>[] WhitePatternsByType
        {
            get
            {
                return whitesByType;
            }
        }

        public ChessType LastChess
        {
            get; private set;
        }

        public ChessType NextChess
        {
            get; private set;
        }

        public bool GameOver
        {
            get
            {
                return Won || Lost;
            }
        }

        public bool Won
        {
            get
            {
                return blackFive != null && ForbiddenPointType == ForbiddenPointType.None;
            }
        }

        public bool Lost
        {
            get
            {
                return whiteFive != null || ForbiddenPointType != ForbiddenPointType.None;
            }
        }

        public ForbiddenPointType ForbiddenPointType
        {
            get; private set;
        }

        public IEnumerable<Pattern> ForbiddenPointInvolvements
        {
            get
            {
                if (forbiddenPointInvolvements == null)
                    forbiddenPointInvolvements = new List<Pattern>();
                return forbiddenPointInvolvements;
            }
        }

        public List<Coord> GetGameOverReasons(ChessBoard board)
        {
            Debug.Assert(GameOver);
            if (Won)
                return blackFive.AllCoord(board).ToList();
            else
            {
                Debug.Assert(Lost);
                if (whiteFive != null)
                    return whiteFive.AllCoord(board).ToList();
                else
                {
                    List<Coord> reasons = new List<Coord>();
                    foreach (Pattern ptn in forbiddenPointInvolvements)
                        reasons.AddRange(ptn.AllCoord(board));
                    return reasons;
                }
            }
        }

        private List<Pattern> blacks = new List<Pattern>();
        private List<Pattern> whites = new List<Pattern>();
        private List<Pattern>[] blacksByType = new List<Pattern>[(int)PatternType.Max];
        private List<Pattern>[] whitesByType = new List<Pattern>[(int)PatternType.Max];
        private List<Pattern> forbiddenPointInvolvements = null;
        private Pattern blackFive = null;
        private Pattern whiteFive = null;

        private static bool[] forbiddenPointRelevant = new bool[(int)PatternType.Max] {
            // None, Two_S, Two, Three_S, Three, Four_S, Four, Five, ToBeSixOrMore, SixOrMore
            false, false, false, false, true, true, true, false, false, true,
        };

        public Situation(IEnumerable<Pattern> patterns, ChessType lastChess, Coord last)
        {
            if (lastChess == ChessType.None)
                return;

            LastChess = lastChess;
            NextChess = ChessBoard.ReverseChess(lastChess);

            List<Pattern>[] lastInvolvedPatterns = lastChess == ChessType.Black ? new List<Pattern>[(int)PatternType.Max] : null;
            for (int idx = 0; idx < (int)PatternType.Max; ++idx)
            {
                blacksByType[idx] = new List<Pattern>();
                whitesByType[idx] = new List<Pattern>();
                if (lastChess == ChessType.Black && forbiddenPointRelevant[idx])
                    lastInvolvedPatterns[idx] = new List<Pattern>();
            }

            foreach (Pattern ptn in patterns)
            {
                int ptn_idx = (int)ptn.pattern;

                if (ptn.chess == ChessType.Black)
                {
                    blacks.Add(ptn);
                    blacksByType[ptn_idx].Add(ptn);
                    if (lastChess == ChessType.Black && forbiddenPointRelevant[ptn_idx] && ptn.CoverCoord(last))
                        lastInvolvedPatterns[ptn_idx].Add(ptn);
                    if (ptn.pattern == PatternType.Five)
                        blackFive = ptn;
                }
                else
                {
                    Debug.Assert(ptn.chess == ChessType.White);
                    whites.Add(ptn);
                    whitesByType[ptn_idx].Add(ptn);
                    if (ptn.pattern == PatternType.Five || ptn.pattern == PatternType.SixOrMore)
                        whiteFive = ptn;
                }
            }

            if (lastChess == ChessType.White)
            {
                ForbiddenPointType = ForbiddenPointType.None;
                forbiddenPointInvolvements = null;
            }
            else if (lastInvolvedPatterns[(int)PatternType.Three].Count >= 2)
            {
                ForbiddenPointType = ForbiddenPointType.DoubleThree;
                forbiddenPointInvolvements = lastInvolvedPatterns[(int)PatternType.Three];
            }
            else if (lastInvolvedPatterns[(int)PatternType.Four].Count + lastInvolvedPatterns[(int)PatternType.Four_S].Count >= 2)
            {
                ForbiddenPointType = ForbiddenPointType.DoubleFour;
                forbiddenPointInvolvements = new List<Pattern>();
                forbiddenPointInvolvements.AddRange(lastInvolvedPatterns[(int)PatternType.Four]);
                forbiddenPointInvolvements.AddRange(lastInvolvedPatterns[(int)PatternType.Four_S]);
            }
            else if (lastInvolvedPatterns[(int)PatternType.SixOrMore].Count >= 1)
            {
                ForbiddenPointType = ForbiddenPointType.SixOrMore;
                forbiddenPointInvolvements = lastInvolvedPatterns[(int)PatternType.SixOrMore];
            }
            else
            {
                ForbiddenPointType = ForbiddenPointType.None;
                forbiddenPointInvolvements = null;
            }
        }

        [TestClass]
        public class UnitTest
        {
            [TestMethod]
            public void Test_IsForbiddenPoint()
            {
                var cases = new List<Tuple<string, bool>>
                {
                    // 眠三 + 活三
                    Tuple.Create("wbBb\n" +
                                 "w b \n" +
                                 " wbw\n",
                                 false),
                    // 双活三
                    Tuple.Create(" Bbb\n" +
                                 "w b \n" +
                                 "wwwb\n",
                                 true),
                    // 四三
                    Tuple.Create("wBbbb\n" +
                                 "w b  \n" +
                                 "wwwb \n",
                                 false),
                    // 四四
                    Tuple.Create("wbbbB\n" +
                                 " wwb \n" +
                                 " wbw \n" +
                                 " bw  \n",
                                 true),
                    // 另一种四四
                    Tuple.Create("wb bBb b\n" +
                                 "   www  \n",
                                 true),
                    // 长连
                    Tuple.Create("bbbBbb\n" +
                                 "www ww\n",
                                 true),
                    // 三三
                    Tuple.Create("b   \n" +
                                 "Bww \n" +
                                 " bw \n" +
                                 "b bw\n",
                                 true),
                };

                foreach (var c in cases)
                {
                    ChessBoard board = ChessBoard.InitializeFromString(c.Item1);
                    Situation s = SituationAnalyzer.Analyze(board);
                    Assert.AreEqual(c.Item2, s.ForbiddenPointType != ForbiddenPointType.None);
                }
            }
        }
    }

    class SituationAnalyzer
    {
        static readonly Offset[] directions = new Offset[]
        {
            new Offset(-1, -1),
            new Offset(-1, 0),
            new Offset(-1, +1),
            new Offset(0, -1),
            new Offset(0, +1),
            new Offset(+1, -1),
            new Offset(+1, 0),
            new Offset(+1, +1),
        };
        static readonly Offset[] directions4 = new Offset[]
        {
            new Offset(0, +1),
            new Offset(+1, +1),
            new Offset(+1, 0),
            new Offset(+1, -1),
        };

        public static Situation Analyze(ChessBoard board)
        {
            return new Situation(GetBoardPatterns(board).ToList(), board.LastChess, board.Last);
        }

        public static Situation Update(ChessBoard board, Situation situation, IEnumerable<Coord> moves)
        {
            List<Pattern> patterns = new List<Pattern>();
            patterns.AddRange(situation.BlackPatterns);
            patterns.AddRange(situation.WhitePatterns);

            foreach (Coord move in moves)
            {
                List<Pattern> newPatterns = new List<Pattern>();
                foreach (Pattern ptn in patterns)
                {
                    if (!ptn.InSameLine(move))
                        newPatterns.Add(ptn);
                }

                foreach (Pattern ptn in GetCoordPatterns(board, move, false))
                {
                    newPatterns.Add(ptn);
                }

                patterns = newPatterns;
            }

            return new Situation(patterns, board.LastChess, board.Last);
        }

        static IEnumerable<Pattern> GetBoardPatterns(ChessBoard board)
        {
            for (int i = 0; i < ChessBoard.BoardSize; ++i)
            {
                foreach (var p in GetDirectionPatterns(board, new Coord(0, i), new Offset(1, 0)))
                    yield return p;
                foreach (var p in GetDirectionPatterns(board, new Coord(i, 0), new Offset(0, 1)))
                    yield return p;
            }
            for (int i = 0; i < ChessBoard.BoardSize - 4; ++i)
            {
                foreach (var p in GetDirectionPatterns(board, new Coord(0, i), new Offset(1, 1)))
                    yield return p;
                if (i > 0)
                {
                    foreach (var p in GetDirectionPatterns(board, new Coord(i, 0), new Offset(1, 1)))
                        yield return p;
                }
                foreach (var p in GetDirectionPatterns(board, new Coord(ChessBoard.BoardSize - 1, i), new Offset(-1, 1)))
                    yield return p;
                if (i > 0)
                {
                    foreach (var p in GetDirectionPatterns(board, new Coord(ChessBoard.BoardSize - i - 1, 0), new Offset(-1, 1)))
                        yield return p;
                }
            }
        }

        static IEnumerable<Pattern> GetDirectionPatterns(ChessBoard board, Coord start, Offset dir)
        {
            int space_start = 0;
            int chess_start = 0;
            int next_space_start = -1;
            ChessType ptn_chess = ChessType.None;
            bool[] ptn = new bool[ChessBoard.BoardSize];

            int pos = 0;
            for (; ; ++pos)
            {
                Coord curr = new Coord(start.X + dir.X * pos, start.Y + dir.Y * pos);
                if (!ChessBoard.IsValidCoord(curr))
                    break;

                ChessType curr_chess = board[curr];
                if (curr_chess == ChessType.None)
                {
                    if (next_space_start < 0)
                        next_space_start = pos;
                    if (ptn_chess != ChessType.None)
                        ptn[pos - chess_start] = false;
                }
                else
                {
                    if (ptn_chess == ChessType.None)
                    {
                        ptn_chess = curr_chess;
                        chess_start = pos;
                        ptn[0] = true;
                    }
                    else if (ptn_chess == curr_chess)
                    {
                        ptn[pos - chess_start] = true;
                    }
                    else
                    {
                        if (next_space_start < 0)
                            next_space_start = pos;
                        foreach (var p in GetLinePatterns(ptn, 0, next_space_start - chess_start, chess_start - space_start, pos - next_space_start))
                        {
                            int length = p.Item2 - p.Item1;
                            Coord coord_start = new Coord(start.X + dir.X * (chess_start + p.Item1),
                                                          start.Y + dir.Y * (chess_start + p.Item1));
                            Coord coord_end = new Coord(coord_start.X + dir.X * length,
                                                        coord_start.Y + dir.Y * length);
                            yield return new Pattern(ptn_chess, coord_start, coord_end, p.Item3);
                        }
                        ptn_chess = curr_chess;
                        space_start = next_space_start;
                        chess_start = pos;
                        ptn[0] = true;
                    }
                    next_space_start = -1;
                }
            }

            if (ptn_chess != ChessType.None)
            {
                if (next_space_start < 0)
                    next_space_start = pos;
                foreach (var p in GetLinePatterns(ptn, 0, next_space_start - chess_start, chess_start - space_start, pos - next_space_start))
                {
                    int length = p.Item2 - p.Item1;
                    Coord coord_start = new Coord(start.X + dir.X * (chess_start + p.Item1),
                                                  start.Y + dir.Y * (chess_start + p.Item1));
                    Coord coord_end = new Coord(coord_start.X + dir.X * length,
                                                coord_start.Y + dir.Y * length);
                    yield return new Pattern(ptn_chess, coord_start, coord_end, p.Item3);
                }
            }
        }

        struct Span
        {
            public int start, end;
            public Span(int start_, int end_)
            {
                start = start_;
                end = end_;
            }
            public int Length
            {
                get
                {
                    return end - start;
                }
            }
        }

        static IEnumerable<Tuple<int, int, PatternType>> GetLinePatterns(bool[] ptn, int ptn_start, int ptn_end, int space_before, int space_after)
        {
            List<Span> seperators = new List<Span>();
            seperators.Add(new Span(ptn_start - space_before - 1, ptn_start));

            int sep_start = -1;
            for (int i = ptn_start; i < ptn_end; ++i)
            {
                if (!ptn[i] && sep_start < 0)
                    sep_start = i;
                else if (sep_start >= 0 && ptn[i])
                {
                    seperators.Add(new Span(sep_start, i));
                    sep_start = -1;
                }
            }
            Debug.Assert(sep_start < 0);
            seperators.Add(new Span(ptn_end, ptn_end + space_after + 1));
            Debug.Assert(seperators.Count >= 2);

            for (int start = 0; start < seperators.Count - 1; ++start)
            {
                for (int end = start + 1; end < seperators.Count; ++end)
                {
                    PatternType p = GetStrictLinePattern(ptn, seperators[start].end, seperators[end].start,
                        seperators[start].Length - 1, seperators[end].Length - 1);
                    if (p != PatternType.None)
                        yield return new Tuple<int, int, PatternType>(seperators[start].end, seperators[end].start, p);
                }
            }
        }

        static PatternType GetStrictLinePattern(bool[] ptn, int ptn_start, int ptn_end, int space_before, int space_after)
        {
            int ptn_length = ptn_end - ptn_start;
            Debug.Assert(ptn_start >= 0 && ptn_end > ptn_start);
            Debug.Assert(ptn[ptn_start] == true && ptn[ptn_end - 1] == true);
            Debug.Assert(space_before >= 0);
            Debug.Assert(space_after >= 0);

            int total_length = ptn_length + space_before + space_after;
            if (total_length < 5)
                return PatternType.None;
            bool adjacent_to_opponent = (total_length == 5 || space_before == 0 || space_after == 0);

            int chess_count = 0;
            for (int i = ptn_start; i < ptn_end; ++i)
            {
                if (ptn[i])
                    ++chess_count;
            }

            switch (chess_count)
            {
                case 1:
                    return PatternType.None;
                case 2:
                    if (ptn_length <= 4)
                        return adjacent_to_opponent ? PatternType.Two_S : PatternType.Two;
                    else
                        return PatternType.None;
                case 3:
                    if (ptn_length <= 4)
                        return adjacent_to_opponent ? PatternType.Three_S : PatternType.Three;
                    else if (ptn_length == 5)
                        return PatternType.Three_S;
                    else
                        return PatternType.None;
                case 4:
                    if (ptn_length == 4)
                        return adjacent_to_opponent ? PatternType.Four_S : PatternType.Four;
                    else if (ptn_length == 5)
                        return PatternType.Four_S;
                    else
                        return PatternType.None;
                case 5:
                    if (ptn_length == 5)
                        return PatternType.Five;
                    else if (ptn_length == 6)
                        return PatternType.ToBeSixOrMore;
                    else
                        return PatternType.None;
                default:
                    if (chess_count == ptn_length)
                        return PatternType.SixOrMore;
                    else if (chess_count + 1 == ptn_length)
                        return PatternType.ToBeSixOrMore;
                    else
                        return PatternType.None;
            }
        }

        static IEnumerable<Pattern> GetCoordPatterns(ChessBoard board, Coord coord, bool coverCoord = true)
        {
            ChessType chess = board[coord];
            if (chess == ChessType.None)
                yield break;

            foreach (Offset dir in directions4)
            {
                Coord start = coord;
                while (true)
                {
                    Coord tmp = new Coord(start.X - dir.X, start.Y - dir.Y);
                    if (!ChessBoard.IsValidCoord(tmp))
                        break;
                    start = tmp;
                }

                foreach (var p in GetDirectionPatterns(board, start, dir))
                {
                    if (!coverCoord || (p.chess == chess && p.CoverCoord(coord)))
                        yield return p;
                }
            }
        }

        [TestClass]
        public class UnitTest
        {
            protected bool[] ToBooleanArray(string s)
            {
                bool[] array = new bool[s.Length];
                for (int i = 0; i < array.Length; ++i)
                    array[i] = (s[i] != ' ');
                return array;
            }

            [TestMethod]
            public void Test_GetStrictLinePattern()
            {
                var cases = new List<Tuple<string, PatternType, PatternType>>
                {
                    Tuple.Create("#", PatternType.None, PatternType.None),
                    Tuple.Create("##", PatternType.Two, PatternType.Two_S),
                    Tuple.Create("# #", PatternType.Two, PatternType.Two_S),
                    Tuple.Create("#  #", PatternType.Two, PatternType.Two_S),
                    Tuple.Create("#   #", PatternType.None, PatternType.None),
                    Tuple.Create("#    #", PatternType.None, PatternType.None),
                    Tuple.Create("###", PatternType.Three, PatternType.Three_S),
                    Tuple.Create("## #", PatternType.Three, PatternType.Three_S),
                    Tuple.Create("# ##", PatternType.Three, PatternType.Three_S),
                    Tuple.Create("##  #", PatternType.Three_S, PatternType.Three_S),
                    Tuple.Create("# # #", PatternType.Three_S, PatternType.Three_S),
                    Tuple.Create("#  ##", PatternType.Three_S, PatternType.Three_S),
                    Tuple.Create("##   #", PatternType.None, PatternType.None),
                    Tuple.Create("# #  #", PatternType.None, PatternType.None),
                    Tuple.Create("#  # #", PatternType.None, PatternType.None),
                    Tuple.Create("#   ##", PatternType.None, PatternType.None),
                    Tuple.Create("####", PatternType.Four, PatternType.Four_S),
                    Tuple.Create("### #", PatternType.Four_S, PatternType.Four_S),
                    Tuple.Create("## ##", PatternType.Four_S, PatternType.Four_S),
                    Tuple.Create("# ###", PatternType.Four_S, PatternType.Four_S),
                    Tuple.Create("###  #", PatternType.None, PatternType.None),
                    Tuple.Create("## # #", PatternType.None, PatternType.None),
                    Tuple.Create("##  ##", PatternType.None, PatternType.None),
                    Tuple.Create("# ## #", PatternType.None, PatternType.None),
                    Tuple.Create("# # ##", PatternType.None, PatternType.None),
                    Tuple.Create("#  ###", PatternType.None, PatternType.None),
                    Tuple.Create("#####", PatternType.Five, PatternType.Five),
                    Tuple.Create("#### #", PatternType.ToBeSixOrMore, PatternType.ToBeSixOrMore),
                    Tuple.Create("### ##", PatternType.ToBeSixOrMore, PatternType.ToBeSixOrMore),
                    Tuple.Create("## ###", PatternType.ToBeSixOrMore, PatternType.ToBeSixOrMore),
                    Tuple.Create("# ####", PatternType.ToBeSixOrMore, PatternType.ToBeSixOrMore),
                    Tuple.Create("######", PatternType.SixOrMore, PatternType.SixOrMore),
                    Tuple.Create("### ###", PatternType.ToBeSixOrMore, PatternType.ToBeSixOrMore),
                };

                foreach (var c in cases)
                {
                    bool[] ptn = ToBooleanArray(c.Item1);
                    Assert.AreEqual(c.Item2, SituationAnalyzer.GetStrictLinePattern(ptn, 0, ptn.Length, 1, 10));
                    Assert.AreEqual(c.Item3, SituationAnalyzer.GetStrictLinePattern(ptn, 0, ptn.Length, 0, 10));
                }
            }

            [TestMethod]
            public void Test_GetLinePatterns()
            {
                var cases = new List<Tuple<string, List<Tuple<int, int, PatternType>>, List<Tuple<int, int, PatternType>>>>
                {
                    Tuple.Create("##", new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 2, PatternType.Two),
                        }, new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 2, PatternType.Two_S),
                        }),
                    Tuple.Create("###", new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 3, PatternType.Three),
                        }, new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 3, PatternType.Three_S),
                        }),
                    Tuple.Create("####", new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 4, PatternType.Four),
                        }, new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 4, PatternType.Four_S),
                        }),
                    Tuple.Create("#####", new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 5, PatternType.Five),
                        }, new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 5, PatternType.Five),
                        }),
                    Tuple.Create("## #", new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 2, PatternType.Two_S),
                            Tuple.Create(0, 4, PatternType.Three),
                        }, new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 4, PatternType.Three_S),
                        }),
                    Tuple.Create("##  #", new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 2, PatternType.Two),
                            Tuple.Create(0, 5, PatternType.Three_S),
                        }, new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 5, PatternType.Three_S),
                        }),
                    Tuple.Create("# ##", new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 4, PatternType.Three),
                            Tuple.Create(2, 4, PatternType.Two_S),
                        }, new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 4, PatternType.Three_S),
                            Tuple.Create(2, 4, PatternType.Two_S),
                        }),
                    Tuple.Create("#  ##", new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 5, PatternType.Three_S),
                            Tuple.Create(3, 5, PatternType.Two),
                        }, new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 5, PatternType.Three_S),
                            Tuple.Create(3, 5, PatternType.Two),
                        }),
                    Tuple.Create("# # #", new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 3, PatternType.Two_S),
                            Tuple.Create(0, 5, PatternType.Three_S),
                            Tuple.Create(2, 5, PatternType.Two_S),
                        }, new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 5, PatternType.Three_S),
                            Tuple.Create(2, 5, PatternType.Two_S),
                        }),
                    Tuple.Create("# ## #", new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 4, PatternType.Three_S),
                            Tuple.Create(2, 6, PatternType.Three_S),
                        }, new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(2, 6, PatternType.Three_S),
                        }),
                    Tuple.Create("# # # #", new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 3, PatternType.Two_S),
                            Tuple.Create(0, 5, PatternType.Three_S),
                            Tuple.Create(2, 7, PatternType.Three_S),
                            Tuple.Create(4, 7, PatternType.Two_S),
                        }, new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 5, PatternType.Three_S),
                            Tuple.Create(2, 7, PatternType.Three_S),
                            Tuple.Create(4, 7, PatternType.Two_S),
                        }),
                    Tuple.Create("# ### #", new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 5, PatternType.Four_S),
                            Tuple.Create(2, 7, PatternType.Four_S),
                        }, new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 5, PatternType.Four_S),
                            Tuple.Create(2, 7, PatternType.Four_S),
                        }),
                    Tuple.Create("# ###", new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 5, PatternType.Four_S),
                            Tuple.Create(2, 5, PatternType.Three_S),
                        }, new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 5, PatternType.Four_S),
                            Tuple.Create(2, 5, PatternType.Three_S),
                        }),
                    Tuple.Create("### #", new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 3, PatternType.Three_S),
                            Tuple.Create(0, 5, PatternType.Four_S),
                        }, new List<Tuple<int, int, PatternType>> {
                            Tuple.Create(0, 5, PatternType.Four_S),
                        }),
                };

                foreach (var c in cases)
                {
                    bool[] ptn = ToBooleanArray(c.Item1);
                    var subcases = new List<Tuple<int, List<Tuple<int, int, PatternType>>>>
                    {
                        Tuple.Create(10, c.Item2),
                        Tuple.Create(0, c.Item3),
                    };
                    foreach (var subcase in subcases)
                    {
                        var results = SituationAnalyzer.GetLinePatterns(ptn, 0, ptn.Length, subcase.Item1, 10).ToList();
                        results.Sort();
                        Assert.AreEqual(subcase.Item2.Count, results.Count);
                        for (int i = 0; i < results.Count; ++i)
                            Assert.AreEqual(subcase.Item2[i], results[i]);
                    }
                }
            }

            [TestMethod]
            public void Test_GetBoardPatterns()
            {
                string board_str =
                    " wBbb\n" +
                    " w b \n" +
                    " ww  \n" +
                    "b    \n";
                var expected_patterns = new List<Pattern> {
                    new Pattern(ChessType.Black, new Coord(7, 7), new Coord(10, 7), PatternType.Three_S),
                    new Pattern(ChessType.Black, new Coord(7, 7), new Coord(9, 9), PatternType.Two),
                    new Pattern(ChessType.Black, new Coord(8, 7), new Coord(8, 9), PatternType.Two),
                    new Pattern(ChessType.Black, new Coord(9, 7), new Coord(7, 9), PatternType.Two_S),
                    new Pattern(ChessType.White, new Coord(6, 7), new Coord(6, 10), PatternType.Three),
                    new Pattern(ChessType.White, new Coord(6, 8), new Coord(8, 10), PatternType.Two),
                    new Pattern(ChessType.White, new Coord(6, 9), new Coord(8, 9), PatternType.Two),
                };

                ChessBoard board = ChessBoard.InitializeFromString(board_str);
                var results = SituationAnalyzer.GetBoardPatterns(board).ToList();
                results.Sort();

                expected_patterns.Sort();
                Assert.AreEqual(expected_patterns.Count, results.Count);
                for (int i = 0; i < results.Count; ++i)
                    Assert.AreEqual(expected_patterns[i], results[i]);
            }

            [TestMethod]
            public void Test_UpdateSituation()
            {
                ChessBoard board = ChessBoard.InitializeFromString(
                    "    B  \n" +
                    "   bwb \n" +
                    "w b  wb\n" +
                    " bw    \n" +
                    "w      \n");
                Situation s1 = board.Situation;
                board.Place(ChessType.White, 6, 6);
                Situation s2 = board.Situation;
                Assert.AreEqual(2, s2.BlackPatterns.ToList().Count);
            }
        }
    }

    public class ScoreEstimator
    {
        static readonly int[] patternScore = new int[]
        {
            0,      // None,
            3,      // Two_S,
            10,     // Two,
            20,     // Three_S,
            100,    // Three,
            200,    // Four_S,
            1000,   // Four,
            0,      // Five, handled specially
            -500,   // ToBeSixOrMore, partially handled specially
            0,      // SixOrMore, handled specially
        };

        const float firstHandSuperiority = 1.2f;

        public static Score Estimate(Situation situation)
        {
            if (situation.Won)
                return Score.Won;
            if (situation.Lost)
                return Score.Lost;

            float blackScore = SumPatternScores(situation.BlackPatterns);
            float whiteScore = SumPatternScores(situation.WhitePatterns);

            if (situation.NextChess == ChessType.Black)
                blackScore *= firstHandSuperiority;
            else
                whiteScore *= firstHandSuperiority;

            return new Score(blackScore - whiteScore);
        }

        private static float SumPatternScores(IEnumerable<Pattern> patterns)
        {
            float score = 0;
            foreach (Pattern ptn in patterns)
                score += patternScore[(int)ptn.pattern];
            return score;
        }
    }
}
