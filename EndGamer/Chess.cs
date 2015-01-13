using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

namespace endgamer
{
    [DebuggerDisplay("X = {X}, Y = {Y}")]
    struct Position
    {
        public int X, Y;

        public Position(Position pos)
        {
            X = pos.X;
            Y = pos.Y;
        }

        public Position(int x, int y)
        {
            X = x;
            Y = y;
        }

        public Position Offset(int dx, int dy)
        {
            return new Position(X + dx, Y + dy);
        }
    }

    [DebuggerDisplay("From = {From}, To = {To}")]
    struct Move
    {
        public Position From, To;

        public Move(Position from, Position to)
        {
            From = from;
            To = to;
        }

        public Move(int from_x, int from_y, int to_x, int to_y)
        {
            From = new Position(from_x, from_y);
            To = new Position(to_x, to_y);
        }
    }

    enum ChessPieceColor
    {
        Red = 0,
        Black = 1,
    }

    [DebuggerDisplay("{Name}, {Color}")]
    abstract class ChessPiece
    {
        public abstract string Name { get; }
        public ChessPieceColor Color;

        public ChessPiece(ChessPieceColor color)
        {
            Color = color;
        }

        public bool IsValidPosition(Position pos)
        {
            return IsValidPosition(pos.X, pos.Y);
        }

        public virtual bool IsValidPosition(int x, int y)
        {
            return true;
        }

        public abstract IEnumerable<Position> GetPossibleMovement(ChessBoard board, Position origin);

        public virtual bool Attackable(ChessBoard board, Position origin, Position dest)
        {
            foreach (Position pos in GetPossibleMovement(board, origin))
            {
                if (pos.X == dest.X && pos.Y == dest.Y)
                    return true;
            }

            return false;
        }
    }

    class ChessPiece_King : ChessPiece
    {
        public override string Name { get { return Color == ChessPieceColor.Red ? "帅" : "将"; } }

        public ChessPiece_King(ChessPieceColor color)
            : base(color)
        {
        }

        public override bool IsValidPosition(int x, int y)
        {
            int y_offset = (Color == ChessPieceColor.Red ? 1 : 8);
            return (x >= 4 && x <= 6 && y >= y_offset && y <= y_offset + 2);
        }

        public override IEnumerable<Position> GetPossibleMovement(ChessBoard board, Position origin)
        {
            Debug.Assert(IsValidPosition(origin));

            int y_offset = (Color == ChessPieceColor.Red ? 1 : 8);
            foreach (Position dir in ChessBoard.Directions)
            {
                int x = origin.X + dir.X;
                int y = origin.Y + dir.Y;
                if (x >= 4 && x <= 6 && y >= y_offset && y <= y_offset + 2)
                    yield return new Position(x, y);
            }
        }

        public override bool Attackable(ChessBoard board, Position origin, Position dest)
        {
            int y_offset = (Color == ChessPieceColor.Red ? 1 : 8);
            Debug.Assert(origin.X >= 4 && origin.X <= 6 && origin.Y >= y_offset && origin.Y <= y_offset + 2);

            if (dest.X >= 4 && dest.X <= 6 && dest.Y >= y_offset && dest.Y <= y_offset + 2)
                return Math.Abs(origin.X - dest.X) + Math.Abs(origin.Y - dest.Y) == 1;
            return false;
        }

        public bool AttackableToKing(ChessBoard board, Position origin, Position dest)
        {
            if (dest.X != origin.X)
                return false;

            int from = Math.Min(dest.Y, origin.Y) + 1;
            int to = Math.Max(dest.Y, origin.Y) - 1;
            for (int i = from; i <= to; ++i)
            {
                if (board.GetPiece(origin.X, i) != null)
                    return false;
            }

            return true;
        }
    }

    class ChessPiece_Guard : ChessPiece
    {
        public override string Name { get { return Color == ChessPieceColor.Red ? "仕" : "士"; } }

        public ChessPiece_Guard(ChessPieceColor color)
            : base(color)
        {
        }

        public override bool IsValidPosition(int x, int y)
        {
            int y_offset = (Color == ChessPieceColor.Red ? 1 : 8);
            return x >= 4 && x <= 6 && y >= y_offset && y <= y_offset + 2 && (x + y) % 2 == 1;
        }

        public override IEnumerable<Position> GetPossibleMovement(ChessBoard board, Position origin)
        {
            Debug.Assert(IsValidPosition(origin));

            int y_offset = (Color == ChessPieceColor.Red ? 1 : 8);
            if (origin.X != 5)
                yield return new Position(5, y_offset + 1);
            else
            {
                yield return new Position(4, y_offset + 0);
                yield return new Position(4, y_offset + 2);
                yield return new Position(6, y_offset + 2);
                yield return new Position(6, y_offset + 0);
            }
        }

        public override bool Attackable(ChessBoard board, Position origin, Position dest)
        {
            int y_offset = (Color == ChessPieceColor.Red ? 1 : 8);
            Debug.Assert(origin.X >= 4 && origin.X <= 6 && origin.Y >= y_offset && origin.Y <= y_offset + 2);
            Debug.Assert((origin.X + origin.Y) % 2 == 1);

            if (dest.X >= 4 && dest.X <= 6 && dest.Y >= y_offset && dest.Y <= y_offset + 2)
                return Math.Abs(origin.X - dest.X) == 1 && Math.Abs(origin.Y - dest.Y) == 1;
            return false;
        }
    }

    class ChessPiece_Minister : ChessPiece
    {
        public override string Name { get { return Color == ChessPieceColor.Red ? "相" : "象"; } }

        public ChessPiece_Minister(ChessPieceColor color)
            : base(color)
        {
        }

        public override bool IsValidPosition(int x, int y)
        {
            int y_offset = (Color == ChessPieceColor.Red ? 1 : 6);
            return x % 2 == 1 && y % 2 == 1 && (x + y) % 4 == 0 && y >= y_offset && y <= y_offset + 4;
        }

        public override IEnumerable<Position> GetPossibleMovement(ChessBoard board, Position origin)
        {
            Debug.Assert(IsValidPosition(origin));

            int y_offset = (Color == ChessPieceColor.Red ? 1 : 6);
            switch (origin.X)
            {
                case 1:
                case 5:
                case 9:
                    if (origin.X != 1)
                    {
                        if (board.GetPiece(origin.X - 1, y_offset + 1) == null)
                            yield return new Position(origin.X - 2, y_offset);
                        if (board.GetPiece(origin.X - 1, y_offset + 3) == null)
                            yield return new Position(origin.X - 2, y_offset + 4);
                    }
                    if (origin.X != 9)
                    {
                        if (board.GetPiece(origin.X + 1, y_offset + 1) == null)
                            yield return new Position(origin.X + 2, y_offset);
                        if (board.GetPiece(origin.X + 1, y_offset + 3) == null)
                            yield return new Position(origin.X + 2, y_offset + 4);
                    }
                    break;

                case 3:
                case 7:
                    int y_obstacle = (origin.Y + y_offset + 2) / 2;
                    if (board.GetPiece(origin.X - 1, y_obstacle) == null)
                        yield return new Position(origin.X - 2, y_offset + 2);
                    if (board.GetPiece(origin.X + 1, y_obstacle) == null)
                        yield return new Position(origin.X + 2, y_offset + 2);
                    break;
            }
        }

        public override bool Attackable(ChessBoard board, Position origin, Position dest)
        {
            int y_offset = (Color == ChessPieceColor.Red ? 1 : 6);
            Debug.Assert(origin.X % 2 == 1 && origin.Y % 2 == 1);
            Debug.Assert((origin.X + origin.Y) % 4 == 0);
            Debug.Assert(origin.Y >= y_offset && origin.Y <= y_offset + 4);

            if (dest.Y >= y_offset && dest.Y <= y_offset+4)
                return Math.Abs(origin.X - dest.X) == 2 && Math.Abs(origin.Y - dest.Y) == 2;
            return false;
        }
    }

    class ChessPiece_Knight : ChessPiece
    {
        public override string Name { get { return "马"; } }

        public ChessPiece_Knight(ChessPieceColor color)
            : base(color)
        {
        }

        struct Forward
        {
            public int OX, OY, TX, TY;
            public Forward(int ox, int oy, int tx, int ty)
            {
                OX = ox;
                OY = oy;
                TX = tx;
                TY = ty;
            }
        };
        static readonly Forward[] forwards = new Forward[]
        {
            new Forward(+1, 0, +2, -1),
            new Forward(+1, 0, +2, +1),
            new Forward(0, +1, +1, +2),
            new Forward(0, +1, -1, +2),
            new Forward(-1, 0, -2, +1),
            new Forward(-1, 0, -2, -1),
            new Forward(0, -1, -1, -2),
            new Forward(0, -1, +1, -2),
        };
        public override IEnumerable<Position> GetPossibleMovement(ChessBoard board, Position origin)
        {
            foreach (Forward forward in forwards)
            {
                int x = origin.X+forward.TX;
                int y = origin.Y+forward.TY;
                if (board.IsValidPosition(x, y) && board.GetPiece(origin.X + forward.OX, origin.Y + forward.OY) == null)
                    yield return new Position(x, y);
            }
        }
    }

    class ChessPiece_Rock : ChessPiece
    {
        public override string Name { get { return "车"; } }

        public ChessPiece_Rock(ChessPieceColor color)
            : base(color)
        {
        }

        public override IEnumerable<Position> GetPossibleMovement(ChessBoard board, Position origin)
        {
            foreach (Position dir in ChessBoard.Directions)
            {
                for (int i = 1; ; ++i)
                {
                    int x = origin.X + dir.X * i;
                    int y = origin.Y + dir.Y * i;
                    if (!board.IsValidPosition(x, y))
                        break;
                    yield return new Position(x, y);
                    if (board.GetPiece(x, y) != null)
                        break;
                }
            }
        }

        public override bool Attackable(ChessBoard board, Position origin, Position dest)
        {
            if (origin.X != dest.X && origin.Y != dest.Y)
                return false;

            int dx = Math.Sign(dest.X - origin.X);
            int dy = Math.Sign(dest.Y - origin.Y);
            for (int i=1; ;++i)
            {
                int x = origin.X + dx * i;
                int y = origin.Y + dy * i;
                if (x == dest.X && y == dest.Y)
                    return true;
                if (board.GetPiece(x, y) != null)
                    return false;
            }
        }
    }

    class ChessPiece_Cannon : ChessPiece
    {
        public override string Name { get { return "砲"; } }

        public ChessPiece_Cannon(ChessPieceColor color)
            : base(color)
        {
        }

        public override IEnumerable<Position> GetPossibleMovement(ChessBoard board, Position origin)
        {
            foreach (Position dir in ChessBoard.Directions)
            {
                for (int i = 1; ; ++i)
                {
                    int x = origin.X + dir.X * i;
                    int y = origin.Y + dir.Y * i;
                    if (!board.IsValidPosition(x, y))
                        break;
                    if (board.GetPiece(x, y) == null)
                        yield return new Position(x, y);
                    else
                    {
                        int x1 = origin.X + dir.X * i;
                        int y1 = origin.Y + dir.Y * i;
                        for (int j = i + 1; ; ++j)
                        {
                            if (!board.IsValidPosition(x1, y1))
                                break;
                            if (board.GetPiece(x1, y1) != null)
                            {
                                yield return new Position(x1, y1);
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }

        public override bool Attackable(ChessBoard board, Position origin, Position dest)
        {
            if (origin.X != dest.X && origin.Y != dest.Y)
                return false;

            int obstacle_count = 0;
            int dx = Math.Sign(dest.X - origin.X);
            int dy = Math.Sign(dest.Y - origin.Y);
            for (int i = 1; ; ++i)
            {
                int x = origin.X + dx * i;
                int y = origin.Y + dy * i;
                if (x == dest.X && y == dest.Y)
                    return (obstacle_count == 1);
                if (board.GetPiece(x, y) != null)
                {
                    if (++obstacle_count > 1)
                        return false;
                }
            }
        }
    }

    class ChessPiece_Pawn : ChessPiece
    {
        public override string Name { get { return Color == ChessPieceColor.Red ? "兵" : "卒"; } }

        public ChessPiece_Pawn(ChessPieceColor color)
            : base(color)
        {
        }

        public override IEnumerable<Position> GetPossibleMovement(ChessBoard board, Position origin)
        {
            int y_dir = (Color == ChessPieceColor.Red ? +1 : -1);
            if (board.IsValidPosition(origin.X, origin.Y + y_dir))
                yield return new Position(origin.X, origin.Y + y_dir);

            if ((Color == ChessPieceColor.Red && origin.Y <= 5) || (Color == ChessPieceColor.Black && origin.Y >= 6))
                yield break;

            if (origin.X > 1)
                yield return new Position(origin.X - 1, origin.Y);
            if (origin.X < 10)
                yield return new Position(origin.X + 1, origin.Y);
        }
    }

    class ChessBoard
    {
        public static readonly Position[] Directions = new Position[]
        {
            new Position(0, +1),
            new Position(0, -1),
            new Position(+1, 0),
            new Position(-1, 0),
        };

        public ChessBoard()
        {
            ClearBoard();
        }

        public ChessBoard Clone()
        {
            ChessBoard new_board = new ChessBoard();
            for (int i = 0; i < pieces.Length; ++i)
                new_board.pieces[i] = pieces[i];
            foreach (var kv in blacks)
                new_board.blacks[kv.Key] = kv.Value;
            foreach (var kv in reds)
                new_board.reds[kv.Key] = kv.Value;
            new_board.kings[0] = kings[0];
            new_board.kings[1] = kings[1];
            return new_board;
        }

        public void InitBoard()
        {
            ClearBoard();

            SetPiece(1, 1, new ChessPiece_Rock(ChessPieceColor.Red));
            SetPiece(2, 1, new ChessPiece_Knight(ChessPieceColor.Red));
            SetPiece(3, 1, new ChessPiece_Minister(ChessPieceColor.Red));
            SetPiece(4, 1, new ChessPiece_Guard(ChessPieceColor.Red));
            SetPiece(5, 1, new ChessPiece_King(ChessPieceColor.Red));
            SetPiece(6, 1, new ChessPiece_Guard(ChessPieceColor.Red));
            SetPiece(7, 1, new ChessPiece_Minister(ChessPieceColor.Red));
            SetPiece(8, 1, new ChessPiece_Knight(ChessPieceColor.Red));
            SetPiece(9, 1, new ChessPiece_Rock(ChessPieceColor.Red));
            SetPiece(2, 3, new ChessPiece_Cannon(ChessPieceColor.Red));
            SetPiece(8, 3, new ChessPiece_Cannon(ChessPieceColor.Red));
            SetPiece(1, 4, new ChessPiece_Pawn(ChessPieceColor.Red));
            SetPiece(3, 4, new ChessPiece_Pawn(ChessPieceColor.Red));
            SetPiece(5, 4, new ChessPiece_Pawn(ChessPieceColor.Red));
            SetPiece(7, 4, new ChessPiece_Pawn(ChessPieceColor.Red));
            SetPiece(9, 4, new ChessPiece_Pawn(ChessPieceColor.Red));

            SetPiece(1, 10, new ChessPiece_Rock(ChessPieceColor.Black));
            SetPiece(2, 10, new ChessPiece_Knight(ChessPieceColor.Black));
            SetPiece(3, 10, new ChessPiece_Minister(ChessPieceColor.Black));
            SetPiece(4, 10, new ChessPiece_Guard(ChessPieceColor.Black));
            SetPiece(5, 10, new ChessPiece_King(ChessPieceColor.Black));
            SetPiece(6, 10, new ChessPiece_Guard(ChessPieceColor.Black));
            SetPiece(7, 10, new ChessPiece_Minister(ChessPieceColor.Black));
            SetPiece(8, 10, new ChessPiece_Knight(ChessPieceColor.Black));
            SetPiece(9, 10, new ChessPiece_Rock(ChessPieceColor.Black));
            SetPiece(2, 8, new ChessPiece_Cannon(ChessPieceColor.Black));
            SetPiece(8, 8, new ChessPiece_Cannon(ChessPieceColor.Black));
            SetPiece(1, 7, new ChessPiece_Pawn(ChessPieceColor.Black));
            SetPiece(3, 7, new ChessPiece_Pawn(ChessPieceColor.Black));
            SetPiece(5, 7, new ChessPiece_Pawn(ChessPieceColor.Black));
            SetPiece(7, 7, new ChessPiece_Pawn(ChessPieceColor.Black));
            SetPiece(9, 7, new ChessPiece_Pawn(ChessPieceColor.Black));
        }

        public void ClearBoard()
        {
            pieces = new ChessPiece[9 * 10];
            blacks = new Dictionary<int, ChessPiece>();
            reds = new Dictionary<int, ChessPiece>();
        }

        public IEnumerable<Tuple<Position, ChessPiece>> Pieces()
        {
            foreach (var kv in blacks)
                yield return new Tuple<Position, ChessPiece>(new Position(kv.Key % 9 + 1, kv.Key / 9 + 1), kv.Value);
            foreach (var kv in reds)
                yield return new Tuple<Position, ChessPiece>(new Position(kv.Key % 9 + 1, kv.Key / 9 + 1), kv.Value);
        }

        public IEnumerable<Tuple<Position, ChessPiece>> Pieces(ChessPieceColor color)
        {
            foreach (var kv in (color == ChessPieceColor.Black ? blacks : reds))
                yield return new Tuple<Position, ChessPiece>(new Position(kv.Key % 9 + 1, kv.Key / 9 + 1), kv.Value);
        }

        public ChessPiece GetPiece(int x, int y)
        {
            return GetPiece(new Position(x, y));
        }

        public ChessPiece GetPiece(Position pos)
        {
            if (!IsValidPosition(pos))
                return null;

            int index = PositionToIndex(pos);
            ChessPiece piece = pieces[index];
            Debug.Assert(piece == null || (piece.Color == ChessPieceColor.Black ? blacks : reds)[index] == piece);
            return piece;
        }

        public void SetPiece(int x, int y, ChessPiece piece)
        {
            Debug.Assert(IsValidPosition(x, y));
            Debug.Assert(piece.IsValidPosition(x, y));

            int index = PositionToIndex(x, y);
            pieces[index] = piece;
            (piece.Color == ChessPieceColor.Black ? blacks : reds)[index] = piece;

            if (piece is ChessPiece_King)
            {
                Debug.Assert(kings[(int)piece.Color] == -1);
                kings[(int)piece.Color] = index;
            }
        }

        public void SetPiece(Position pos, ChessPiece piece)
        {
            SetPiece(pos.X, pos.Y, piece);
        }

        public bool MovePiece(Position from, Position to, ChessPiece piece, bool valid_attack = false)
        {
            Debug.Assert(IsValidPosition(from));
            Debug.Assert(piece.IsValidPosition(from));
            Debug.Assert(IsValidPosition(to));
            Debug.Assert(piece.IsValidPosition(to));

            int to_index = PositionToIndex(to);
            ChessPiece target = GetPiece(to);
            if (target != null)
            {
                if (target.Color == piece.Color)
                    return false;
                if (valid_attack && !piece.Attackable(this, from, to))
                    return false;

                Dictionary<int, ChessPiece> ctpieces = (target.Color == ChessPieceColor.Black ? blacks : reds);
                Debug.Assert(ctpieces[to_index] == target);
                ctpieces.Remove(to_index);

                if (target is ChessPiece_King)
                    kings[(int)target.Color] = -1;
            }

            int from_index = PositionToIndex(from);
            Dictionary<int, ChessPiece> cpieces = (piece.Color == ChessPieceColor.Black ? blacks : reds);

            Debug.Assert(pieces[from_index] == piece);
            Debug.Assert(cpieces[from_index] == piece);

            pieces[from_index] = null;
            cpieces.Remove(from_index);

            pieces[to_index] = piece;
            cpieces[to_index] = piece;

            if (piece is ChessPiece_King)
                kings[(int)piece.Color] = to_index;
            return true;
        }

        public bool IsValidPosition(Position pos)
        {
            return IsValidPosition(pos.X, pos.Y);
        }

        public bool IsValidPosition(int x, int y)
        {
            return x >= 1 && x <= 9 && y >= 1 && y <= 10;
        }

        public bool IsCheckmateAgainst(ChessPieceColor color, ChessPieceColor in_turn)
        {
            ChessPieceColor attacker_color = (ChessPieceColor)(1 - (int)color);
            int king_pos_index = kings[(int)color];
            Position king_pos = new Position(king_pos_index % 9 + 1, king_pos_index / 9 + 1);

            foreach (Tuple<Position, ChessPiece> tup in Pieces(attacker_color))
            {
                Position pos = tup.Item1;
                ChessPiece piece = tup.Item2;
                if (piece.Attackable(this, pos, king_pos))
                    return true;
                if (piece is ChessPiece_King && color != in_turn && ((ChessPiece_King)piece).AttackableToKing(this, pos, king_pos))
                    return true;
            }

            return false;
        }

        private static int PositionToIndex(Position pos)
        {
            return PositionToIndex(pos.X, pos.Y);
        }

        private static int PositionToIndex(int x, int y)
        {
            return y * 9 + x - 10;
        }

        private ChessPiece[] pieces;
        private Dictionary<int, ChessPiece> blacks;
        private Dictionary<int, ChessPiece> reds;
        private int[] kings = new int[2] { -1, -1 };
    }
}
