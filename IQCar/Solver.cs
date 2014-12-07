using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;

namespace IQCar
{
    public class Solver
    {
        public static Node Solve(Placement root)
        {
            if (root.Succeed())
                return new Node(root);

            Queue<Node> wait_queue = new Queue<Node>();
            wait_queue.Enqueue(new Node(root, null));

            List<Placement> all_placements = new List<Placement>();
            all_placements.Add(root);

            while (wait_queue.Count > 0)
            {
                Node node = wait_queue.Dequeue();
                foreach (Placement child in node.Placement.Expand())
                {
                    if (child.Succeed())
                        return new Node(child, node);

                    bool duplicated = false;
                    foreach (Placement p in all_placements)
                    {
                        if (p.IsSame(child))
                        {
                            duplicated = true;
                            break;
                        }
                    }
                    if (duplicated)
                        continue;

                    all_placements.Add(child);
                    wait_queue.Enqueue(new Node(child, node));
                }
            }

            return null;
        }

        public class Node
        {
            public Placement Placement { get; private set; }
            public Node Parent { get; private set; }
            public int Depth { get; private set; }

            public Node(Placement placement, Node parent = null)
            {
                Placement = placement;
                Parent = parent;
                Depth = parent == null ? 0 : parent.Depth + 1;
            }
        }
    }

    public class Car
    {
        public Color Color { get; set; }
        public Placement.Direction Direction { get; set; }
        public int Length { get; set; }

        public Car(Color color, Placement.Direction dir, int length)
        {
            Color = color;
            Direction = dir;
            Length = length;
        }
    }

    [DebuggerDisplay("({x}, {y})")]
    public struct Coord
    {
        public int x, y;

        public Coord(Coord coord)
        {
            this.x = coord.x;
            this.y = coord.y;
        }

        public Coord(int x, int y)
        {
            this.x = x;
            this.y = y;
        }
    }

    [DebuggerDisplay("{GetDebugString()}")]
    public class Placement
    {
        public const int Size = 6;

        public enum Direction
        {
            Vertical = 0,
            Horizontal = 1,
        }

        static readonly Coord[] offsets = new Coord[]
        {
            new Coord(0, 1),
            new Coord(1, 0),
        };

        private Car[] cars;
        private int[] board;
        private int king;

        public Placement(Placement copy = null)
        {
            cars = new Car[Size * Size];
            board = new int[Size * Size];
            king = 0;

            if (copy != null)
            {
                for (int i = 0; i < Size * Size; ++i)
                {
                    cars[i] = copy.cars[i];
                    board[i] = copy.board[i];
                }
                king = copy.king;
            }
        }

        public IEnumerable<Placement> Expand()
        {
            for (int pos = 0; pos < Size * Size; ++pos)
            {
                if (cars[pos] == null)
                    continue;

                for (int steps = -1; ; --steps)
                {
                    Placement move = new Placement(this);
                    if (!move.Move(pos, steps))
                        break;
                    yield return move;
                }

                for (int steps = 1; ; ++steps)
                {
                    Placement move = new Placement(this);
                    if (!move.Move(pos, steps))
                        break;
                    yield return move;
                }
            }
        }

        public bool CanMove(int x, int y, int steps)
        {
            return CanMove(y * Size + x, steps);
        }

        public bool CanMove(Coord coord, int steps)
        {
            return CanMove(coord.x, coord.y, steps);
        }

        public bool Move(int x, int y, int steps)
        {
            return Move(y * Size + x, steps);
        }

        public bool Move(Coord coord, int steps)
        {
            return Move(coord.x, coord.y, steps);
        }

        private bool CanMove(int pos, int steps)
        {
            if (pos < 0 || pos >= Size*Size || steps == 0)
                return false;

            Car car = cars[pos];
            if (car == null)
                return false;

            int x = (pos % Size);
            int y = (pos / Size);
            int dir = (int)car.Direction;
            int length = car.Length;

            int ox = offsets[dir].x;
            int oy = offsets[dir].y;

            int start = (steps < 0 ? steps : length);
            int end = (steps < 0 ? 0 : length + steps);

            for (int i = start; i < end; ++i)
            {
                int nx = x + ox * i;
                int ny = y + oy * i;
                if (nx < 0 || nx >= Size || ny < 0 || ny >= Size)
                    return false;
                int npos = ny * Size + nx;
                if (board[npos] != 0)
                    return false;
            }

            return true;
        }

        private bool Move(int pos, int steps)
        {
            if (!CanMove(pos, steps))
                return false;

            Car car = cars[pos];
            Debug.Assert(car != null);

            int x = (pos % Size);
            int y = (pos / Size);
            int dir = (int)car.Direction;
            int length = car.Length;

            int ox = offsets[dir].x;
            int oy = offsets[dir].y;

            int nx = x + ox * steps;
            int ny = y + oy * steps;
            int npos = ny * Size + nx;

            for (int i = 0; i < length; ++i)
                board[(y + oy * i) * Size + (x + ox * i)] = 0;
            for (int i = 0; i < length; ++i)
                board[(ny + oy * i) * Size + (nx + ox * i)] = npos + 1;

            cars[pos] = null;
            cars[npos] = car;

            if (king == pos + 1)
                king = npos + 1;

            return true;
        }

        public bool Succeed()
        {
            if (king == 0)
                return false;

            Debug.Assert(king > 0 && king <= Size * Size);
            int x = (king - 1) % Size;
            int y = (king - 1) / Size;

            Car car = cars[king - 1];
            Debug.Assert(car != null);
            Debug.Assert(car.Direction == Direction.Horizontal);
            Debug.Assert(y == 3);
            Debug.Assert(x + car.Length <= Size);

            return x + car.Length == Size;
        }

        public static Placement Generate(int level = 35)
        {
            Placement p = new Placement();
            switch (level)
            {
                case 1:
                    p.Place(0, 3, Direction.Horizontal, 2, Color.Red);
                    p.Place(0, 0, Direction.Vertical, 3, Color.Green);
                    p.Place(1, 2, Direction.Horizontal, 3, Color.Pink);
                    p.Place(3, 3, Direction.Vertical, 3, Color.Purple);
                    p.SetKing(0, 3);
                    break;
                case 35:
                    p.Place(1, 3, Direction.Horizontal, 2, Color.Red);
                    p.Place(0, 2, Direction.Vertical, 3, Color.Green);
                    p.Place(1, 4, Direction.Vertical, 2, Color.Green);
                    p.Place(2, 0, Direction.Vertical, 2, Color.Green);
                    p.Place(3, 3, Direction.Vertical, 2, Color.Green);
                    p.Place(4, 3, Direction.Vertical, 2, Color.Green);
                    p.Place(5, 0, Direction.Vertical, 3, Color.Green);
                    p.Place(0, 0, Direction.Horizontal, 2, Color.Purple);
                    p.Place(1, 2, Direction.Horizontal, 2, Color.Purple);
                    p.Place(3, 1, Direction.Horizontal, 2, Color.Purple);
                    p.Place(3, 2, Direction.Horizontal, 2, Color.Purple);
                    p.Place(3, 5, Direction.Horizontal, 3, Color.Purple);
                    p.SetKing(1, 3);
                    break;
                case 100:
                default:
                    p.Place(0, 3, Direction.Horizontal, 2, Color.Red);
                    p.Place(0, 0, Direction.Vertical, 2, Color.Blue);
                    p.Place(0, 4, Direction.Vertical, 2, Color.Blue);
                    p.Place(1, 4, Direction.Vertical, 2, Color.Blue);
                    p.Place(2, 2, Direction.Vertical, 3, Color.Blue);
                    p.Place(3, 1, Direction.Vertical, 2, Color.Blue);
                    p.Place(5, 0, Direction.Vertical, 3, Color.Blue);
                    p.Place(0, 2, Direction.Horizontal, 2, Color.Purple);
                    p.Place(1, 0, Direction.Horizontal, 3, Color.Purple);
                    p.Place(1, 1, Direction.Horizontal, 2, Color.Purple);
                    p.Place(2, 5, Direction.Horizontal, 2, Color.Purple);
                    p.Place(4, 4, Direction.Horizontal, 2, Color.Purple);
                    p.Place(4, 5, Direction.Horizontal, 2, Color.Purple);
                    p.SetKing(0, 3);
                    break;
            }
            return p;
        }

        public bool CanPlace(int x, int y, Direction dir, int length)
        {
            if (x < 0 || x >= Size || y < 0 || y >= Size)
                return false;
            if (dir != Direction.Horizontal && dir != Direction.Vertical)
                return false;
            if (length != 2 && length != 3)
                return false;
            if (dir == Direction.Horizontal && x + length > Size)
                return false;
            if (dir == Direction.Vertical && y + length > Size)
                return false;

            int ox = offsets[(int)dir].x;
            int oy = offsets[(int)dir].y;
            for (int i = 0; i < length; ++i)
            {
                if (board[(y + oy * i) * Size + (x + ox * i)] != 0)
                    return false;
            }

            return true;
        }

        public bool Place(int x, int y, Direction dir, int length, Color color)
        {
            return Place(x, y, new Car(color, dir, length));
        }

        public bool Place(int x, int y, Car car)
        {
            if (!CanPlace(x, y, car.Direction, car.Length))
                return false;

            int ox = offsets[(int)car.Direction].x;
            int oy = offsets[(int)car.Direction].y;

            int pos = y * Size + x;
            for (int i = 0; i < car.Length; ++i)
                board[(y + oy * i) * Size + (x + ox * i)] = pos + 1;
            cars[pos] = car;
            return true;
        }

        public bool IsSame(Placement another)
        {
            for (int i = 0; i < Size * Size; ++i)
                if (board[i] != another.board[i])
                    return false;
            return true;
        }

        public IEnumerable<Tuple<Car, Coord>> GetCars()
        {
            for (int pos = 0; pos < Size * Size; ++pos)
            {
                Car car = cars[pos];
                if (car == null)
                    continue;
                int x = pos % Size;
                int y = pos / Size;
                yield return new Tuple<Car, Coord>(car, new Coord(x, y));
            }
        }

        public Tuple<Car, Coord> GetCar(Coord coord)
        {
            return GetCar(coord.x, coord.y);
        }

        public Tuple<Car, Coord> GetCar(int x, int y)
        {
            if (x < 0 || x >= Size || y < 0 || y >= Size)
                return new Tuple<Car, Coord>(null, new Coord(-1, -1));

            int pos = board[y * Size + x];
            if (pos == 0)
                return new Tuple<Car, Coord>(null, new Coord(x, y));

            --pos;
            return new Tuple<Car, Coord>(cars[pos], new Coord(pos % Placement.Size, pos / Placement.Size));
        }

        public Car RemoveCar(Coord coord)
        {
            return RemoveCar(coord.x, coord.y);
        }

        public Car RemoveCar(int x, int y)
        {
            var v = GetCar(x, y);
            Car car = v.Item1;
            if (car == null)
                return null;

            Coord coord = v.Item2;
            int ox = offsets[(int)car.Direction].x;
            int oy = offsets[(int)car.Direction].y;
            for (int i = 0; i < car.Length; ++i)
            {
                int pos = (y + oy * i) * Size + (x + ox * i);
                cars[pos] = null;
                board[pos] = 0;
                if (i == 0 && king == (pos + 1))
                    king = 0;
            }

            return car;
        }

        public Tuple<Car, Coord> ProbeKing()
        {
            king = 0;
            foreach (var car_coord in GetCars())
            {
                Car car = car_coord.Item1;
                Coord coord = car_coord.Item2;
                if (car.Direction == Placement.Direction.Horizontal && car.Length == 2 && coord.y == 3)
                {
                    SetKing(coord.x, coord.y);
                    return GetKing();
                }
            }

            return new Tuple<Car, Coord>(null, new Coord());
        }

        public bool SetKing(int x, int y)
        {
            if (x < 0 || x >= Size || y < 0 || y >= Size)
                return false;

            int pos = board[y * Size + x];
            if (pos == 0)
                return false;

            king = pos;
            return true;
        }

        public Tuple<Car, Coord> GetKing()
        {
            if (king == 0)
                return new Tuple<Car, Coord>(null, new Coord());

            Debug.Assert(cars[king - 1] != null);
            Debug.Assert(board[king - 1] == king);
            return new Tuple<Car, Coord>(cars[king - 1], new Coord((king - 1) % Size, (king - 1) / Size));
        }

        public string GetDebugString()
        {
            return ToString(null).Replace(';', '\n');
        }

        public string ToString(Dictionary<char, Color> color_map)
        {
            string colors = "ABCDEFGHIJKLMNOPQRST";
            Debug.Assert(Size * Size <= colors.Length * 2);

            int idx = 0;
            Dictionary<int, char> mapping = new Dictionary<int, char>();
            List<string> lines = new List<string>();
            for (int y = 0; y < Size; ++y)
            {
                char[] line = new char[Size];
                for (int x = 0; x < Size; ++x)
                {
                    int pos = y * Size + x;
                    if (board[pos] == 0)
                        line[x] = '.';
                    else if (mapping.ContainsKey(board[pos]))
                        line[x] = mapping[board[pos]];
                    else
                    {
                        char c = colors[idx];
                        ++idx;
                        mapping[board[pos]] = c;
                        line[x] = c;
                        if (color_map != null)
                            color_map[c] = cars[pos].Color;
                    }
                }
                lines.Add(new string(line));
            }
            lines.Reverse();
            return string.Join(";", lines);
        }

        public class ParseError : Exception
        {
            public ParseError(string msg)
                : base(msg)
            {
            }
        }

        public static Placement FromString(string s, Dictionary<char, Color> colors)
        {
            Debug.Assert(s != null && colors != null);

            if (s.Length != Size * Size + Size - 1)
                throw new ParseError(string.Format("Excepted length %d, got %d", Size * Size + Size - 1, s.Length));

            Dictionary<char, Tuple<Car, Coord>> map = new Dictionary<char, Tuple<Car, Coord>>();
            for (int y = 0; y < Size; ++y)
            {
                for (int x = 0; x < Size; ++x)
                {
                    char c = s[y * (Size + 1) + x];
                    if (!map.ContainsKey(c))
                        map[c] = new Tuple<Car, Coord>(null, new Coord(x, y));
                    else
                    {
                        Tuple<Car, Coord> car_coord = map[c];
                        Car car = car_coord.Item1;
                        Coord coord = car_coord.Item2;
                        if (x != coord.x && y != coord.y)
                            throw new ParseError(string.Format("Car %c malformed.", c));
                        int distance = Math.Abs(x - coord.x) + Math.Abs(y - coord.y);

                        Direction dir = x == coord.x ? Direction.Vertical : Direction.Horizontal;
                        if (car == null)
                        {
                            if (distance != 1)
                                throw new ParseError(string.Format("Car %c malformed.", c));
                            if (!colors.ContainsKey(c))
                                throw new ParseError(string.Format("Car %c has no color assigned", c));

                            car = new Car(colors[c], dir, 2);
                        }
                        else
                        {
                            if (dir == car.Direction && distance == car.Length + 1)
                                car.Length += 1;
                            else
                                throw new ParseError(string.Format("Car %c malformed.", c));
                        }
                    }
                }
            }

            Placement placement = new Placement();
            foreach (Tuple<Car, Coord> car_coord in map.Values)
            {
                Car car = car_coord.Item1;
                Coord coord = car_coord.Item2;
                if (car == null)
                    throw new ParseError("Car length == 1");
                if (car.Length > 3)
                    throw new ParseError(string.Format("Car length == %d", car.Length));

                if (!placement.Place(coord.x, coord.y, car))
                    throw new ParseError("Internal error");
            }

            if (placement.ProbeKing().Item1 == null)
                throw new ParseError("King is not found");
            return placement;
        }
    }
}
