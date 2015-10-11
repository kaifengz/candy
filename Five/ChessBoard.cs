using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Diagnostics;
using System.Collections.Generic;

namespace Five
{
    using Coord = System.Drawing.Point;

    public class ChessBoard
    {
        public const int BoardSize = 15;
        private static readonly ChessType[] nextChess = { ChessType.Black, ChessType.White };
        private static readonly ChessType[] reversedChess = { ChessType.None, ChessType.White, ChessType.Black };

        private ChessType[][] rows;
        private int chessCount;
        private Situation situation;
        private Score score;
        private Coord last;
        private List<Coord> movesAfterAnalyzedSituation;

        public ChessType Get(int x, int y)
        {
            return rows[y][x];
        }

        public ChessType this[Coord coord]
        {
            get
            {
                return rows[coord.Y][coord.X];
            }
        }

        public int ChessCount
        {
            get
            {
                return chessCount;
            }
        }

        public ChessType NextChess
        {
            get
            {
                return nextChess[chessCount % 2];
            }
        }

        public ChessBoard()
        {
            ResetBoard();
        }

        private void ResetBoard()
        {
            rows = new ChessType[BoardSize][];
            for (int row = 0; row < BoardSize; ++row)
                rows[row] = new ChessType[BoardSize];
            chessCount = 0;
            situation = null;
            score = null;
            last = new Coord();
            if (movesAfterAnalyzedSituation != null)
                movesAfterAnalyzedSituation.Clear();
        }

        public void Place(ChessType chess, Coord coord, bool dont_validate_order = false)
        {
            Place(chess, coord.Y, coord.X, dont_validate_order);
        }

        public void Place(ChessType chess, int row, int column, bool dont_validate_order=false)
        {
            if (!TryPlace(chess, row, column, dont_validate_order))
                throw new PlaceError();
        }

        public bool TryPlace(ChessType chess, int row, int column, bool dont_validate_order=false)
        {
            if (row < 0 || row >= BoardSize || column < 0 || column >= BoardSize || rows[row][column] != ChessType.None)
                return false;

            if (!dont_validate_order && chess != NextChess)
                return false;

            rows[row][column] = chess;
            chessCount++;
            score = null;
            last = new Coord(column, row);
            if (movesAfterAnalyzedSituation == null)
                movesAfterAnalyzedSituation = new List<Coord>();
            movesAfterAnalyzedSituation.Add(last);
            return true;
        }

        public bool Undo(Coord new_last)
        {
            if (chessCount <= 0)
                return false;

            Debug.Assert(this[last] != ChessType.None);
            if (chessCount > 1 && this[new_last] != reversedChess[(int)this[last]])
                return false;

            if (movesAfterAnalyzedSituation != null && movesAfterAnalyzedSituation.Count > 0)
            {
                Debug.Assert(movesAfterAnalyzedSituation[movesAfterAnalyzedSituation.Count - 1] == last);
                movesAfterAnalyzedSituation.RemoveAt(movesAfterAnalyzedSituation.Count - 1);
            }
            else
            {
                situation = null;
            }

            rows[last.Y][last.X] = ChessType.None;
            last = new_last;
            chessCount--;
            score = null;
            return true;
        }

        public ChessBoard Clone()
        {
            ChessBoard cloned = new ChessBoard();
            for (int x = 0; x < BoardSize; ++x)
            {
                for (int y = 0; y < BoardSize; ++y)
                    cloned.rows[y][x] = rows[y][x];
            }
            cloned.chessCount = chessCount;
            cloned.situation = situation;
            cloned.movesAfterAnalyzedSituation = new List<Coord>(movesAfterAnalyzedSituation);
            cloned.score = score;
            cloned.last = last;
            return cloned;
        }

        public Situation Situation
        {
            get
            {
                if (situation == null)
                {
                    situation = SituationAnalyzer.Analyze(this);
                    if (movesAfterAnalyzedSituation != null)
                        movesAfterAnalyzedSituation.Clear();
                }
                else if (movesAfterAnalyzedSituation != null && movesAfterAnalyzedSituation.Count > 0)
                {
                    situation = SituationAnalyzer.Update(this, situation, movesAfterAnalyzedSituation);
                    movesAfterAnalyzedSituation.Clear();
                }
                return situation;
            }
        }

        public Score Score
        {
            get
            {
                if (score == null)
                    score = ScoreEstimator.Estimate(Situation);
                return score;
            }
        }

        public bool Won
        {
            get
            {
                return Situation.Won;
            }
        }

        public bool Lost
        {
            get
            {
                return Situation.Lost;
            }
        }

        public bool ThisChessWin(ChessType chess)
        {
            return chess == ChessType.Black ? Won : Lost;
        }

        public bool ThisChessLost(ChessType chess)
        {
            return chess == ChessType.White ? Won : Lost;
        }

        public bool GameOver
        {
            get
            {
                return Situation.GameOver;
            }
        }

        public Coord Last
        {
            get
            {
                return last;
            }
        }

        public ChessType LastChess
        {
            get
            {
                if (chessCount == 0)
                    return ChessType.None;

                Debug.Assert(this[last] != ChessType.None);
                return this[last];
            }
        }

        public static ChessType ReverseChess(ChessType chess)
        {
            return reversedChess[(int)chess];
        }

        public static ChessBoard InitializeFromString(string s, Coord? last = null)
        {
            if (s.EndsWith("\n"))
                s = s.Substring(0, s.Length - 1);

            Coord origin = new Coord(-1, -1);
            string[] lines = s.Split('\n');
            int height = lines.Length;
            int width = lines[0].Length;
            Debug.Assert(height <= 15 && width <= 15, "too many lines or columns");
            for (int y = 0; y < height; ++y)
            {
                Debug.Assert(width == lines[y].Length, "mixed lengths of lines");
                int x = lines[y].IndexOf('B');
                if (x >= 0)
                {
                    Debug.Assert(origin.X < 0, "More than one origin is found");
                    origin.X = x;
                    origin.Y = y;
                }
                x = lines[y].IndexOf('W');
                if (x >= 0)
                {
                    Debug.Assert(origin.X < 0, "More than one origin is found");
                    origin.X = x;
                    origin.Y = y;
                }
            }
            Debug.Assert(origin.X >= 0, "The origin is not found");
            Debug.Assert(origin.X <= 7 && origin.X + 8 >= width &&
                         origin.Y <= 7 && origin.Y + 8 >= height,
                         "The origin is incorrectly placed");

            ChessBoard board = new ChessBoard();
            int blacks = 0;
            int whites = 0;
            for (int y = 0; y < height; ++y)
            {
                for (int x = 0; x < width; ++x)
                {
                    ChessType chess = ChessType.None;
                    char c = lines[y][x];
                    if (c == 'b' || c == 'B')
                    {
                        ++blacks;
                        chess = ChessType.Black;
                    }
                    else if (c == 'w' || c == 'W')
                    {
                        ++whites;
                        chess = ChessType.White;
                    }
                    else
                        continue;
                    board.Place(chess, y + 7 - origin.Y, x + 7 - origin.X, true);
                }
            }

            // force the center be the last move for the testing purpose
            board.last = last ?? new Coord(7, 7);
            Debug.Assert(board.LastChess != ChessType.None);
            if (board.LastChess == ChessType.Black)
                Debug.Assert(blacks == whites + 1);
            else
                Debug.Assert(blacks == whites);
            return board;
        }
    }

    public enum ChessType
    {
        None = 0,
        Black,
        White,
    }

    public class PlaceError : Exception
    {
    }

    public enum ScoreCategory
    {
        Estimated = 0,
        Won,
        Lost,
    }

    [DebuggerDisplay("{Category} {score}")]
    public class Score : IComparable
    {
        public ScoreCategory Category { get; private set; }
        public float score { get; private set; }

        public static readonly Score Won = new Score(0, ScoreCategory.Won);
        public static readonly Score Lost = new Score(0, ScoreCategory.Lost);

        public Score(float score, ScoreCategory category = ScoreCategory.Estimated)
        {
            this.Category = category;
            this.score = score;
        }

        public int CompareTo(object other)
        {
            Score another = (Score)other;
            switch (Category)
            {
                case ScoreCategory.Won:
                    return another.Category == ScoreCategory.Won ? 0 : +1;

                case ScoreCategory.Lost:
                    return another.Category == ScoreCategory.Lost ? 0 : -1;

                case ScoreCategory.Estimated:
                    switch (another.Category)
                    {
                        case ScoreCategory.Estimated: return Math.Sign(score - another.score);
                        case ScoreCategory.Won: return -1;
                        case ScoreCategory.Lost: return +1;
                    }
                    break;
            }

            Debug.Assert(false);
            return 0;
        }

        public bool Dominant
        {
            get
            {
                return Category == ScoreCategory.Won ||
                       (Category == ScoreCategory.Estimated && score >= 0.0);
            }
        }

        public static bool operator <(Score a, Score b)
        {
            return a.CompareTo(b) < 0;
        }

        public static bool operator >(Score a, Score b)
        {
            return a.CompareTo(b) > 0;
        }

        [TestClass]
        public class UnitTest
        {
            [TestMethod]
            public void Test_Comparison()
            {
                Assert.IsTrue(Score.Won > Score.Lost);
                Assert.IsTrue(Score.Won > new Score(0));
                Assert.IsTrue(Score.Lost < new Score(0));
                Assert.IsTrue(new Score(1) > new Score(0));
            }
        }
    }
}
