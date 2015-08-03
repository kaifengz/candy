using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Diagnostics;

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
        private Score score;
        private Coord last;

        public ChessType[] this[int row]
        {
            get
            {
                return rows[row];
            }
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
            score = null;
            last = new Coord();
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
            cloned.score = score;
            cloned.last = last;
            return cloned;
        }

        public Score Score
        {
            get
            {
                if (score == null)
                    score = ChessEvaluator.Evaluate(this);
                return score;
            }
        }

        public bool Won
        {
            get
            {
                return this.Score.Category == ScoreCategory.Won;
            }
        }

        public bool Lost
        {
            get
            {
                return this.Score.Category == ScoreCategory.Lost;
            }
        }

        public bool GameOver
        {
            get
            {
                return this.Score.Category == ScoreCategory.Won ||
                       this.Score.Category == ScoreCategory.Lost;
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
                return last.IsEmpty ? ChessType.None : this[last];
            }
        }

        public static ChessType ReverseChess(ChessType chess)
        {
            return reversedChess[(int)chess];
        }

        public void InitializeForTest()
        {
            ResetBoard();
            Place(ChessType.Black, 7, 7);
            Place(ChessType.White, 6, 8);
            Place(ChessType.Black, 8, 9);
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
        float score = 0;

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
