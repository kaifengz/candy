using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;

namespace Five
{
    using Coord = System.Drawing.Point;
    using Offset = System.Drawing.Point;

    class ChessEvaluator
    {
        enum PatternType
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

        enum ForbiddenPointType
        {
            None,
            DoubleThree,
            DoubleFour,
            SixOrMore,
        };

        const float firstHandSuperiority = 1.2f;

        [DebuggerDisplay("{chess} {start} - {end} {pattern}")]
        struct Pattern : IComparable
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

            public bool CoverCoord(Coord coord)
            {
                int dx1 = start.X - coord.X;
                int dx2 = coord.X - end.X;
                int dy1 = start.Y - coord.Y;
                int dy2 = coord.Y - end.Y;
                bool x_in_range = (dx1 == 0 && dx2 == 0) || (dx1 * dx2 > 0);
                bool y_in_range = (dy1 == 0 && dy2 == 0) || (dy1 * dy2 > 0);
                return (x_in_range && y_in_range) || (dx1 == 0 && dy1 == 0);
            }

            public IEnumerable<Coord> AllCoord()
            {
                int ox = Math.Sign(end.X - start.X);
                int oy = Math.Sign(end.Y - start.Y);

                for (int i = 0; ; ++i)
                {
                    Coord coord = new Coord(start.X + ox * i, start.Y + oy * i);
                    if (coord == end)
                        break;
                    yield return coord;
                }
            }
        }

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
                if (curr.X < 0 || curr.X >= ChessBoard.BoardSize || curr.Y < 0 || curr.Y >= ChessBoard.BoardSize)
                    break;

                ChessType curr_chess = board[curr.Y][curr.X];
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

        static IEnumerable<Pattern> GetCoordPatterns(ChessBoard board, Coord coord)
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
                    if (tmp.X < 0 || tmp.X >= ChessBoard.BoardSize || tmp.Y < 0 || tmp.Y >= ChessBoard.BoardSize)
                        break;
                    start = tmp;
                }

                foreach (var p in GetDirectionPatterns(board, start, dir))
                {
                    if (p.chess == chess && p.CoverCoord(coord))
                        yield return p;
                }
            }
        }

        public static bool IsForbiddenPoint(ChessBoard board, Coord coord)
        {
            ChessType chess = board[coord];
            if (chess != ChessType.Black)
                // 禁手只针对黑棋
                return false;

            return IsForbiddenPoint(GetCoordPatterns(board, coord)) != ForbiddenPointType.None;
        }

        private static ForbiddenPointType IsForbiddenPoint(IEnumerable<Pattern> patterns)
        {
            int[] patternCount = new int[(int)PatternType.Max];
            foreach (var p in patterns)
            {
                patternCount[(int)p.pattern]++;
                ForbiddenPointType type = GetForbiddenPointType(patternCount);
                if (type != ForbiddenPointType.None)
                    return type;
            }

            return ForbiddenPointType.None;
        }

        private static ForbiddenPointType GetForbiddenPointType(int[] patternCount)
        {
            // 同时形成两个以上的活三
            if (patternCount[(int)PatternType.Three] >= 2)
                return ForbiddenPointType.DoubleThree;

            // 同时形成两个以上的四
            if (patternCount[(int)PatternType.Four_S] + patternCount[(int)PatternType.Four] >= 2)
                return ForbiddenPointType.DoubleFour;

            // 长连
            if (patternCount[(int)PatternType.SixOrMore] > 0)
                return ForbiddenPointType.SixOrMore;

            return ForbiddenPointType.None;
        }

        public static List<Coord> GetGameOverReasons(ChessBoard board)
        {
            List<Pattern> patterns = GetCoordPatterns(board, board.Last).ToList();

            if (board.LastChess == ChessType.Black)
            {
                List<Pattern> coord_patterns = new List<Pattern>();
                foreach (Pattern p in patterns)
                {
                    if (p.CoverCoord(board.Last))
                        coord_patterns.Add(p);
                }

                List<Coord> reasons = new List<Coord>();
                switch (IsForbiddenPoint(coord_patterns))
                {
                    case ForbiddenPointType.DoubleThree:
                        foreach (Pattern p in coord_patterns)
                            if (p.pattern == PatternType.Three)
                                reasons.AddRange(p.AllCoord());
                        return reasons;

                    case ForbiddenPointType.DoubleFour:
                        foreach (Pattern p in coord_patterns)
                            if (p.pattern == PatternType.Four || p.pattern == PatternType.Four_S)
                                reasons.AddRange(p.AllCoord());
                        return reasons;

                    case ForbiddenPointType.SixOrMore:
                        foreach (Pattern p in coord_patterns)
                            if (p.pattern == PatternType.SixOrMore)
                                reasons.AddRange(p.AllCoord());
                        return reasons;
                }
            }

            foreach (Pattern p in patterns)
            {
                if (p.pattern == PatternType.Five || p.pattern == PatternType.SixOrMore)
                    return p.AllCoord().ToList();
            }

            return null;
        }

        public static Score Evaluate(ChessBoard board)
        {
            bool checkForbiddenPoint = (board.LastChess == ChessType.Black);
            int[] patternCount = new int[(int)PatternType.Max];

            float[] scores = new float[] { 0, 0, 0 };
            foreach (var ptn in GetBoardPatterns(board))
            {
                PatternType pattern = ptn.pattern;
                Debug.Assert((int)pattern < patternScore.Length);

                if (pattern == PatternType.Five)
                    return ptn.chess == ChessType.Black ? Score.Won : Score.Lost;

                if (pattern == PatternType.SixOrMore)
                    return Score.Lost;

                if (pattern == PatternType.ToBeSixOrMore)
                {
                    if (ptn.chess == ChessType.White)
                        pattern = PatternType.Four_S;
                }
                scores[(int)ptn.chess] += patternScore[(int)pattern];

                if (checkForbiddenPoint && ptn.CoverCoord(board.Last))
                {
                    patternCount[(int)ptn.pattern]++;
                    if (GetForbiddenPointType(patternCount) != ForbiddenPointType.None)
                        return Score.Lost;
                }
            }

            scores[(int)board.NextChess] *= firstHandSuperiority;

            return new Score(scores[(int)ChessType.Black] - scores[(int)ChessType.White]);
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
                    Assert.AreEqual(c.Item2, ChessEvaluator.GetStrictLinePattern(ptn, 0, ptn.Length, 1, 10));
                    Assert.AreEqual(c.Item3, ChessEvaluator.GetStrictLinePattern(ptn, 0, ptn.Length, 0, 10));
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
                        var results = ChessEvaluator.GetLinePatterns(ptn, 0, ptn.Length, subcase.Item1, 10).ToList();
                        results.Sort();
                        Assert.AreEqual(subcase.Item2.Count, results.Count);
                        for (int i = 0; i < results.Count; ++i)
                            Assert.AreEqual(subcase.Item2[i], results[i]);
                    }
                }
            }

            protected ChessBoard ToChessBoard(string s)
            {
                if (s.EndsWith("\n"))
                    s = s.Substring(0, s.Length - 1);

                Coord origin = new Coord(-1, -1);
                string[] lines = s.Split('\n');
                int height = lines.Length;
                int width = lines[0].Length;
                Assert.IsTrue(height <= 15 && width <= 15, "too many lines or columns");
                for (int y = 0; y < height; ++y)
                {
                    Assert.AreEqual(width, lines[y].Length, "mixed lengths of lines");
                    int x = lines[y].IndexOf('B');
                    if (x >= 0)
                    {
                        Assert.IsTrue(origin.X < 0, "More than one origin is found");
                        origin.X = x;
                        origin.Y = y;
                    }
                }
                Assert.IsTrue(origin.X >= 0, "The origin is not found");
                Assert.IsTrue(origin.X <= 7 && origin.X + 8 >= width &&
                              origin.Y <= 7 && origin.Y + 8 >= height,
                              "The origin is incorrectly placed");

                ChessBoard board = new ChessBoard();
                for (int y = 0; y < height; ++y)
                {
                    for (int x = 0; x < width; ++x)
                    {
                        ChessType chess = ChessType.None;
                        char c = lines[y][x];
                        if (c == 'b' || c == 'B')
                            chess = ChessType.Black;
                        else if (c == 'w' || c == 'W')
                            chess = ChessType.White;
                        else
                            continue;
                        board.Place(chess, y + 7 - origin.Y, x + 7 - origin.X, true);
                    }
                }

                return board;
            }

            [TestMethod]
            public void Test_GetBoardPatterns()
            {
                string board_str =
                    "wBbb\n" +
                    "w b \n" +
                    "ww  \n";
                var expected_patterns = new List<Pattern> {
                    new Pattern(ChessType.Black, new Coord(7, 7), new Coord(10, 7), PatternType.Three_S),
                    new Pattern(ChessType.Black, new Coord(7, 7), new Coord(9, 9), PatternType.Two),
                    new Pattern(ChessType.Black, new Coord(8, 7), new Coord(8, 9), PatternType.Two),
                    new Pattern(ChessType.Black, new Coord(9, 7), new Coord(7, 9), PatternType.Two_S),
                    new Pattern(ChessType.White, new Coord(6, 7), new Coord(6, 10), PatternType.Three),
                    new Pattern(ChessType.White, new Coord(6, 8), new Coord(8, 10), PatternType.Two),
                    new Pattern(ChessType.White, new Coord(6, 9), new Coord(8, 9), PatternType.Two),
                };

                ChessBoard board = ToChessBoard(board_str);
                var results = ChessEvaluator.GetBoardPatterns(board).ToList();
                results.Sort();

                expected_patterns.Sort();
                Assert.AreEqual(expected_patterns.Count, results.Count);
                for (int i = 0; i < results.Count; ++i)
                    Assert.AreEqual(expected_patterns[i], results[i]);
            }

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
                    Tuple.Create("wb bBb bw\n" +
                                 "   www   \n",
                                 true),
                    // 长连
                    Tuple.Create("Bbbbbb", true),
                };

                foreach (var c in cases)
                {
                    ChessBoard board = ToChessBoard(c.Item1);
                    bool is_forbidden_point = ChessEvaluator.IsForbiddenPoint(board, new Coord(7, 7));
                    Assert.AreEqual(c.Item2, is_forbidden_point);
                }
            }
        }
    }
}
