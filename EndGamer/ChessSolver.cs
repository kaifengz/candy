using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

namespace endgamer
{
    class ChessSolver
    {
#if DEBUG
        public event EventHandler Visit;
        public class VisitEventArgs : EventArgs
        {
            public ChessBoard Board { get; private set; }
            public VisitEventArgs(ChessBoard board)
            {
                Board = board;
            }
        }
#endif

        public static ChessBoard init_test()
        {
            ChessBoard board = new ChessBoard();

            //board.SetPiece(9, 7, new ChessPiece_Cannon(ChessPieceColor.Red));
            //board.SetPiece(9, 6, new ChessPiece_Cannon(ChessPieceColor.Red));
            //board.SetPiece(9, 5, new ChessPiece_Rock(ChessPieceColor.Red));
            //board.SetPiece(8, 5, new ChessPiece_Rock(ChessPieceColor.Red));
            //board.SetPiece(4, 1, new ChessPiece_King(ChessPieceColor.Red));
            //board.SetPiece(4, 9, new ChessPiece_Pawn(ChessPieceColor.Red));
            //board.SetPiece(5, 9, new ChessPiece_Pawn(ChessPieceColor.Red));

            //board.SetPiece(5, 10, new ChessPiece_Cannon(ChessPieceColor.Black));
            //board.SetPiece(6, 10, new ChessPiece_King(ChessPieceColor.Black));
            //board.SetPiece(6, 8, new ChessPiece_Rock(ChessPieceColor.Black));
            //board.SetPiece(4, 3, new ChessPiece_Pawn(ChessPieceColor.Black));
            //board.SetPiece(5, 2, new ChessPiece_Pawn(ChessPieceColor.Black));

            // TODO:
            board.SetPiece(5, 1, new ChessPiece_King(ChessPieceColor.Red));
            board.SetPiece(4, 8, new ChessPiece_Pawn(ChessPieceColor.Red));
            board.SetPiece(4, 9, new ChessPiece_Pawn(ChessPieceColor.Red));
            board.SetPiece(4, 10, new ChessPiece_Pawn(ChessPieceColor.Red));
            //board.SetPiece(5, 9, new ChessPiece_Pawn(ChessPieceColor.Red));
            //board.SetPiece(6, 8, new ChessPiece_Pawn(ChessPieceColor.Red));

            board.SetPiece(6, 10, new ChessPiece_King(ChessPieceColor.Black));
            board.SetPiece(1, 1, new ChessPiece_Pawn(ChessPieceColor.Black));

            return board;
        }

        public SearchNode Solve_Checkmate(ChessBoard board, ChessPieceColor first, ChessPieceColor attacker)
        {
            SearchNode root = new SearchNode(board);
            SearchNode final = Solve_Checkmate(root, first, first == attacker);
            if (final == null || final.Parent == null)
                return null;

            SearchNode parent = final.Parent;
            while (parent.Parent != null)
            {
                final = parent;
                parent = final.Parent;
            }
            return final;
        }

        protected SearchNode Solve_Checkmate(SearchNode node, ChessPieceColor color, bool attacking)
        {
            ChessPieceColor my_color = color;
            ChessPieceColor enemy_color = (color == ChessPieceColor.Red ? ChessPieceColor.Black : ChessPieceColor.Red);
            SearchNode final = null;

#if DEBUG
            if (Visit != null)
                Visit(this, new VisitEventArgs(node.Board));
#endif

            foreach (Tuple<Position, ChessPiece> tup in node.Board.Pieces(my_color))
            {
                Position from = tup.Item1;
                ChessPiece piece = tup.Item2;

                foreach (Position to in piece.GetPossibleMovement(node.Board, from))
                {
                    ChessBoard new_board = node.Board.Clone();
                    if (!new_board.MovePiece(from, to, piece))
                        continue;

#if DEBUG
                    if (Visit != null)
                        Visit(this, new VisitEventArgs(new_board));
#endif

                    if (attacking)
                    {
                        if (new_board.IsCheckmateAgainst(enemy_color, enemy_color))
                        {
                            SearchNode next = new SearchNode(new_board, node);
                            final = Solve_Checkmate(next, enemy_color, !attacking);
                            if (final != null)
                                return final;
                        }
                    }
                    else
                    {
                        if (!new_board.IsCheckmateAgainst(my_color, enemy_color))
                        {
                            SearchNode next = new SearchNode(new_board, node);
                            SearchNode last = Solve_Checkmate(next, enemy_color, !attacking);
                            if (last == null)
                                return null;
                            if (final == null || final.Depth < last.Depth)
                                final = last;
                        }
                    }
                }
            }

            if (attacking)
                return final;
            else
                return final ?? node;
        }

        public class SearchNode
        {
            public ChessBoard Board { get; private set; }
            public SearchNode Parent { get; private set; }
            public int Depth { get; private set; }

            public SearchNode(ChessBoard board, SearchNode parent = null)
            {
                Board = board;
                Parent = parent;
                Depth = parent == null ? 0 : parent.Depth + 1;
            }
        }
    }
}
