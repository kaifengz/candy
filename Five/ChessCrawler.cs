﻿using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Diagnostics;

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
        protected class SearchNode : IComparable
        {
            public ChessBoard Board;
            public Coord Move;
            public SearchNode Parent;
            public int Depth;

            public SearchNode(ChessBoard board)
            {
                Board = board;
                Move = new Coord();
                Parent = null;
                Depth = 1;
            }

            public SearchNode(ChessBoard board, int x, int y, SearchNode parent)
            {
                Board = board;
                Move = new Coord(x, y);
                Parent = parent;
                Depth = parent.Depth + 1;
            }

            // return negative values if this is better than other for black
            public int CompareTo(object obj)
            {
                SearchNode other = (SearchNode)obj;
                if (Board.Score != other.Board.Score)
                    return -Board.Score.CompareTo(other.Board.Score);
                if (Depth != other.Depth)
                    return (Board.Score.Dominant ? +1 : -1) * (Depth - other.Depth);
                if (Parent != null)
                {
                    Debug.Assert(other.Parent != null);
                    return Parent.CompareTo(other.Parent);
                }
                return 0;
            }

            public bool BetterForBlack(SearchNode other)
            {
                return CompareTo(other) < 0;
            }

            public bool BetterForWhite(SearchNode other)
            {
                return CompareTo(other) > 0;
            }
        }

        public static List<Coord> GetBestMove(ChessBoard board)
        {
            if (board.GameOver)
                return null;

            if (board.ChessCount < 3)
                return new List<Coord>() { GetOpeningMove(board) };

            SearchNode root = new SearchNode(board);
            SearchNode leaf = MinMaxSearch(root, board.NextChess, MaxSearchDepth, null);
            if (leaf == null)
                return null;

            return LeafNodeToPath(leaf);
        }

        private static List<Coord> LeafNodeToPath(SearchNode leaf)
        {
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
            if (board.ChessCount == 0)
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

        protected static SearchNode MinMaxSearch(SearchNode node, ChessType chess, int maxDepth, SearchNode hint)
        {
            ChessBoard board = node.Board;
            List<SearchNode> children = new List<SearchNode>();
            for (int x = 0; x < ChessBoard.BoardSize; ++x)
            {
                for (int y = 0; y < ChessBoard.BoardSize; ++y)
                {
                    if (board.Get(x, y) != ChessType.None)
                        continue;

                    SearchNode child = new SearchNode(board.Clone(), x, y, node);
                    child.Board.Place(chess, y, x);
                    children.Add(child);
                }
            }

            if (children.Count == 0)
                return null;

            if (node.Depth >= maxDepth)
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
                    SearchNode leaf = MinMaxSearch(child, ChessBoard.ReverseChess(chess), maxDepth, best);
                    if (chess == ChessType.Black)
                    {
                        if (leaf.Board.Won)
                            return leaf;
                        if (best == null || leaf.BetterForBlack(best))
                        {
                            best = leaf;
                            if (hint != null && best.BetterForBlack(hint))
                                return best;
                        }
                    }
                    else
                    {
                        Debug.Assert(chess == ChessType.White);
                        if (leaf.Board.Lost)
                            return leaf;
                        if (best == null || leaf.BetterForWhite(best))
                        {
                            best = leaf;
                            if (hint != null && best.BetterForWhite(hint))
                                return best;
                        }
                    }
                }

                return best;
            }
        }

        protected static void SortByScore(List<SearchNode> nodes, ChessType chess)
        {
            nodes.Sort();
            if (chess == ChessType.White)
                nodes.Reverse();
        }

        protected static void Cutoff(ChessType chess, List<SearchNode> children, SearchNode hint)
        {
            // TODO:
            if (children.Count > 10)
                children.RemoveRange(10, children.Count - 10);
        }

        [TestClass]
        public class UnitTest
        {
            [TestMethod]
            public void Test_ResistThree()
            {
                ChessBoard board = ChessBoard.InitializeFromString(
                    "bw w\n" +
                    "Bbw \n" +
                    "  b \n");
                List<Coord> moves = GetBestMove(board);
                Assert.IsNotNull(moves);
                Assert.IsTrue(moves.Count > 0);
                Assert.IsTrue((moves[0].X == 6 && moves[0].Y == 5) ||
                              (moves[0].X == 10 && moves[0].Y == 9));
            }

            [TestMethod]
            public void Test_ResistFourThree()
            {
                ChessBoard board = ChessBoard.InitializeFromString(
                    "    B  \n" +
                    "   bwb \n" +
                    "w b  wb\n" +
                    " bw    \n" +
                    "w      \n");
                List<Coord> moves = GetBestMove(board);
                Assert.IsNotNull(moves);
                Assert.IsTrue(moves.Count > 0);
                Assert.IsTrue(moves[0].X == 8 && moves[0].Y == 6);
            }

            [TestMethod]
            public void Test_ResistFour()
            {
                ChessBoard board = ChessBoard.InitializeFromString(
                    "    B\n" +
                    " w b \n" +
                    "  b w\n" +
                    " bw  \n");
                List<Coord> moves = GetBestMove(board);
                Assert.IsNotNull(moves);
                Assert.IsTrue(moves.Count > 0);
                Assert.IsTrue((moves[0].X == 8 && moves[0].Y == 6) ||
                              (moves[0].X == 3 && moves[0].Y == 11));
            }

            [TestMethod]
            public void Test_ResistDoubleThree()
            {
                ChessBoard board = ChessBoard.InitializeFromString(
                    " bW \n" +
                    "bwwb\n" +
                    "wbw \n" +
                    "b   \n");
                List<Coord> moves = GetBestMove(board);
                Assert.IsNotNull(moves);
                Assert.IsTrue(moves.Count > 0);
                Assert.IsTrue((moves[0].X == 7 && moves[0].Y == 6) ||
                              (moves[0].X == 8 && moves[0].Y == 6) ||
                              (moves[0].X == 7 && moves[0].Y == 10) ||
                              (moves[0].X == 4 && moves[0].Y == 10));
            }

            [TestMethod]
            [Ignore]
            public void Test_Performance()
            {
                for (int i = 0; i < 10; ++i)
                    Test_ResistThree();
            }
        }
    }
}
