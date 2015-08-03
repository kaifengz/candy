using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Five
{
    using Coord = System.Drawing.Point;

    class ChessCrawler
    {
#if DEBUG
        const int MaxSearchDepth = 4;
#else
        const int MaxSearchDepth = 5;
#endif

        [DebuggerDisplay("{Move} {Board.Score.Category} {Board.Score.score}")]
        protected class SearchNode
        {
            public ChessBoard Board;
            public Coord Move;
            public SearchNode Parent;

            public SearchNode(ChessBoard board, int x, int y, SearchNode parent)
            {
                Board = board;
                Move = new Coord(x, y);
                Parent = parent;
            }
        }

        public static List<Coord> GetBestMove(ChessBoard board)
        {
            if (board.GameOver)
                return null;

            if (board.ChessCount < 3)
                return new List<Coord>() { GetOpeningMove(board) };

            SearchNode root = new SearchNode(board, 0, 0, null);
            SearchNode leaf = MinMaxSearch(root, board.NextChess, 1, null);
            Debug.Assert(leaf != null);

            if (leaf.Parent == null)
                return null;

            List<Coord> path = new List<Coord>();
            SearchNode node = leaf;
            do
            {
                path.Add(node.Move);
                node = node.Parent;
            }
            while (node.Parent != null);
            path.Reverse();
            return path;
        }

        public static Coord GetOpeningMove(ChessBoard board)
        {
            Debug.Assert(board.ChessCount < 3);

            int center = (ChessBoard.BoardSize - 1) / 2;
            if (board.ChessCount == 1)
                return new Coord(center, center);
            else
            {
                int min = center - board.ChessCount;
                int max = center + board.ChessCount + 1;
                while (true)
                {
                    Coord move = new Coord(Utility.Random.Next(min, max), Utility.Random.Next(min, max));
                    if (board[move] == ChessType.None)
                        return move;
                }
            }
        }

        protected static SearchNode MinMaxSearch(SearchNode node, ChessType chess, int depth, Score hint)
        {
            ChessBoard board = node.Board;
            List<SearchNode> children = new List<SearchNode>();
            for (int x = 0; x < ChessBoard.BoardSize; ++x)
            {
                for (int y = 0; y < ChessBoard.BoardSize; ++y)
                {
                    if (board[y][x] != ChessType.None)
                        continue;

                    SearchNode child = new SearchNode(board.Clone(), x, y, node);
                    child.Board.Place(chess, y, x);
                    children.Add(child);
                }
            }

            if (children.Count == 0)
                return node;

            if (depth >= MaxSearchDepth)
            {
                Utility.Shuffle(children);
                SortByScore(children, chess);
                return children[0];
            }
            else
            {
                Utility.Shuffle(children);
                SortByScore(children, chess);
                Cutoff(chess, children, hint);
                Debug.Assert(children.Count > 0);

                if (chess == ChessType.Black && children[0].Board.Won)
                    return children[0];
                else if (chess == ChessType.White && children[0].Board.Lost)
                    return children[0];

                SearchNode best = null;

                foreach (SearchNode child in children)
                {
                    SearchNode leaf = MinMaxSearch(child, ChessBoard.ReverseChess(chess), depth + 1, best?.Board.Score);
                    if (chess == ChessType.Black)
                    {
                        if (leaf.Board.Won)
                            return leaf;
                        if (best == null || best.Board.Score < leaf.Board.Score)
                        {
                            best = leaf;
                            if (hint != null && best.Board.Score > hint)
                                return best;
                        }
                    }
                    else
                    {
                        Debug.Assert(chess == ChessType.White);
                        if (leaf.Board.Lost)
                            return leaf;
                        if (best == null || best.Board.Score > leaf.Board.Score)
                        {
                            best = leaf;
                            if (hint != null && best.Board.Score < hint)
                                return best;
                        }
                    }
                }

                return best;
            }
        }

        protected static void SortByScore(List<SearchNode> nodes, ChessType chess)
        {
            if (chess == ChessType.Black)
                nodes.Sort((a, b) => b.Board.Score.CompareTo(a.Board.Score));
            else
                nodes.Sort((a, b) => a.Board.Score.CompareTo(b.Board.Score));
        }

        protected static void Cutoff(ChessType chess, List<SearchNode> children, Score hint)
        {
            // TODO:
            if (children.Count > 10)
                children.RemoveRange(10, children.Count - 10);
        }

        [TestClass]
        public class UnitTest
        {
            [TestMethod]
            public void Test_Three()
            {
                //   6 7 8 9 
                // 5 . . . .
                // 6 . b w . w
                // 7 . B b w
                // 8 . . . b
                // 9 .
                ChessBoard board = new ChessBoard();
                board.Place(ChessType.Black, 7, 7);
                board.Place(ChessType.White, 6, 8);
                board.Place(ChessType.Black, 8, 9);
                board.Place(ChessType.White, 6, 10);
                board.Place(ChessType.Black, 7, 8);
                board.Place(ChessType.White, 7, 9);
                board.Place(ChessType.Black, 6, 7);

                List<Coord> moves = ChessCrawler.GetBestMove(board);
                Assert.IsNotNull(moves);
                Assert.IsTrue(moves.Count > 0);
                Assert.IsTrue((moves[0].X == 6 && moves[0].Y == 5) ||
                              (moves[0].X == 10 && moves[0].Y == 9));
            }
        }
    }
}
