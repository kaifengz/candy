using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace Pyramid
{
    class Solver
    {
        public Solver(Problem problem)
        {
            this.problem = problem;
        }

        public IEnumerable<bool> Solve()
        {
            List<Piece> unplaced = new List<Piece>();
            foreach (Piece piece in problem.Pieces)
            {
                if (!piece.Placed)
                    unplaced.Add(piece);
            }
            if (unplaced.Count == 0)
                yield break;

            foreach (bool completed in Solve(unplaced))
            {
                yield return completed;
            }
        }

#if DEBUG
        public static IEnumerable<bool> test()
        {
            Problem p = new TriangleProblem();
            p.Setup();

            Solver solver = new Solver(p);
            foreach (bool completed in solver.Solve())
            {
                yield return completed;
            }
        }
#endif

        protected Problem problem;

        #region Privates

        private IEnumerable<bool> Solve(List<Piece> unplaced)
        {
            char[][] board = problem.Board;
            int height = board.Length;

            // find an empty cell
            int oh = 0, ow = 0;
            for (oh = 0; oh < height; ++oh)
            {
                bool found = false;
                char[] line = board[oh];
                for (ow = 0; ow < line.Length; ++ow)
                {
                    if (line[ow] == ' ')
                    {
                        found = true;
                        break;
                    }
                }
                if (found)
                    break;
            }

            // try to place a piece that could occupy the cell
            for (int pi = 0; pi < unplaced.Count; ++pi)
            {
                Piece piece = unplaced[pi];
                foreach (Piece pv in piece.Variants())
                {
                    char[][] shape = pv.Shape;
                    int first = 0;
                    while (shape[0][first] != pv.Color)
                        ++first;
                    if (ow < first)
                        continue;

                    if (!problem.Place(pv, ow - first, oh))
                        continue;

                    if (unplaced.Count == 1)
                        yield return true;
                    else
                    {
                        yield return false;
                        if (ProblemOp.GetHoleCount(problem) == 0)
                        {
                            List<Piece> unplaced2 = new List<Piece>(unplaced);
                            unplaced2.RemoveAt(pi);
                            foreach (bool completed in Solve(unplaced2))
                                yield return completed;
                        }
                    }

                    problem.Remove(pv);
                }
            }
        }

        #endregion
    }
}
