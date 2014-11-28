using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;

namespace Pyramid
{
    class Piece
    {
        public Piece(char color, char[][] shape)
        {
            this.color = color;
            this.placed = false;
            this.x = 0;
            this.y = 0;

            init_shapes(shape);
            select_shape(0);
        }

        public Piece(Piece pp)
        {
            color = pp.color;

            shapes = new char[pp.shapes.Length][][];
            for (int i = 0; i < pp.shapes.Length; ++i)
            {
                shapes[i] = new char[pp.shapes[i].Length][];
                for (int j = 0; j < pp.shapes[i].Length; ++j)
                    shapes[i][j] = (char[])pp.shapes[i][j].Clone();
            }

            shape_is_dup = (bool[])pp.shape_is_dup.Clone();
            placed = pp.placed;
            x = pp.x;
            y = pp.y;

            select_shape(pp.index);
        }

        public char Color { get { return color; } }
        public char[][] Shape { get { return shape; } }
        public int Width { get { return width; } }
        public int Height { get { return height; } }
        public bool Placed { get { return placed; } set { placed = value; } }
        public int X { get { return x; } set { x = value; } }
        public int Y { get { return y; } set { y = value; } }

        public string ShapeString
        {
            get
            {
                return string.Join("\n", shape.Select(line => string.Join("", line)));
            }
        }

        public IEnumerable<Piece> Variants()
        {
            for (int count = 0; count < shapes.Length; ++count)
            {
                if (!shape_is_dup[index])
                    yield return this;
                select_shape((index + 1) % shapes.Length);
            }
        }

        public void Mirror()
        {
            int count = shapes.Length;
            int mirror_index = (count - 1 - index);
            select_shape(mirror_index);
        }

        public void Rotate()
        {
            int count = shapes.Length;
            if (count > 1)
            {
                Debug.Assert(count % 2 == 0);
                int half = count / 2;
                int rotate_index = (index + 1) % half + index / half * half;
                select_shape(rotate_index);
            }
        }

        public object Clone()
        {
            return new Piece(this);
        }

        #region Privates
        private char color;
        private char[][][] shapes;
        private bool[] shape_is_dup;
        private bool placed;
        private char[][] shape;
        private int index;
        private int width, height;
        private int x, y;

        private char[][] mirror_shape(char[][] shape)
        {
            int height = shape.Length;
            int width = shape[0].Length;

            char[][] mirror = new char[width][];
            for (int w = 0; w < width; ++w)
            {
                mirror[w] = new char[height];
                for (int h = 0; h < height; ++h)
                    mirror[w][h] = shape[h][w];
            }

            return mirror;
        }

        private char[][] rotate_shape(char[][] shape)
        {
            int height = shape.Length;
            int width = shape[0].Length;

            char[][] rotate = new char[width][];
            for (int w = 0; w < width; ++w)
            {
                rotate[w] = new char[height];
                for (int h = 0; h < height; ++h)
                    rotate[w][h] = shape[height - 1 - h][w];
            }

            return rotate;
        }

        private static bool same_shape(char[][] shape1, char[][] shape2)
        {
            if (shape1.Length != shape2.Length)
                return false;
            for (int i = 0; i < shape1.Length; ++i)
            {
                if (shape1[i].Length != shape2[i].Length)
                    return false;
                for (int j = 0; j < shape1[i].Length; ++j)
                    if (shape1[i][j] != shape2[i][j])
                        return false;
            }
            return true;
        }

        private void init_shapes(char[][] raw_shape)
        {
            int height = raw_shape.Length;

            int width = raw_shape[0].Length;
            for (int h = 1; h < height; ++h)
                width = (width >= raw_shape[h].Length ? width : raw_shape[h].Length);

            // normailized shape which all the lines have the same width
            char[][] nshape = new char[height][];
            for (int h = 0; h < height; ++h)
            {
                nshape[h] = new char[width];
                int w = 0;
                for (; w < raw_shape[h].Length; ++w)
                    nshape[h][w] = raw_shape[h][w];
                for (; w < width; ++w)
                    nshape[h][w] = ' ';
            }

            shapes = new char[8][][];
            shapes[0] = nshape;
            shapes[1] = rotate_shape(shapes[0]);
            shapes[2] = rotate_shape(shapes[1]);
            shapes[3] = rotate_shape(shapes[2]);
            Debug.Assert(same_shape(shapes[0], rotate_shape(shapes[3])));
            shapes[7] = mirror_shape(shapes[0]);
            shapes[4] = rotate_shape(shapes[7]);
            shapes[5] = rotate_shape(shapes[4]);
            shapes[6] = rotate_shape(shapes[5]);
            Debug.Assert(same_shape(shapes[7], rotate_shape(shapes[6])));
            Debug.Assert(same_shape(shapes[0], mirror_shape(shapes[7])));
            Debug.Assert(same_shape(shapes[1], mirror_shape(shapes[6])));
            Debug.Assert(same_shape(shapes[2], mirror_shape(shapes[5])));
            Debug.Assert(same_shape(shapes[3], mirror_shape(shapes[4])));

            shape_is_dup = new bool[8];
            for (int i = 0; i < shapes.Length; ++i)
            {
                bool duplicated = false;
                for (int j = 0; j < i; ++j)
                {
                    if (same_shape(shapes[i], shapes[j]))
                    {
                        duplicated = true;
                        break;
                    }
                }
                shape_is_dup[i] = duplicated;
            }
        }

        private void select_shape(int index)
        {
            Debug.Assert(index >= 0);
            Debug.Assert(index < shapes.Length);
            this.index = index;
            this.shape = shapes[index];
            this.height = this.shape.Length;
            this.width = this.shape[0].Length;
        }

        #endregion
    }
}
