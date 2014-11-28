using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Pyramid
{
    class Challenge
    {
        private string pattern;
        private int placed_pieces;
        private int solution_count;

        public Challenge(string pattern, int placed_pieces)
        {
            this.pattern = pattern;
            this.placed_pieces = placed_pieces;
            this.solution_count = 1;
        }

        public Challenge(string pattern, int placed_pieces, int solution_count)
        {
            this.pattern = pattern;
            this.placed_pieces = placed_pieces;
            this.solution_count = solution_count;
        }

        public void IncreaseSolutionCount()
        {
            ++this.solution_count;
        }

        public string Pattern
        {
            get
            {
                return pattern;
            }
        }

        public int SolutionCount
        {
            get
            {
                return solution_count;
            }
        }

        public int PlacedPieces
        {
            get
            {
                return placed_pieces;
            }
        }

        // The less the Difficulty, the harder to solve the challenge
        public double Difficulty
        {
            get
            {
                return 3.0 * solution_count / Math.Pow(3.0, 12 - placed_pieces);
            }
        }
    }

    class ChallengeGenerator
    {
        private List<string> solutions;
        private Dictionary<string, Challenge> challenges;

        private const uint min_placed_pieces = 1;
        private const uint max_placed_pieces = 10;

        public List<Challenge> GenerateChallenges(List<string> solutions)
        {
            this.solutions = solutions;
            this.challenges = new Dictionary<string, Challenge>();

            foreach (string solution in solutions)
            {
                DismantleSolution(solution);
            }

            List<Challenge> challenges = this.challenges.Values.ToList();
            this.solutions = null;
            this.challenges = null;
            GC.Collect();
            return challenges;
        }

        private void DismantleSolution(string solution)
        {
            char[] lookup = new char[256];
            lookup['\n'] = '\n';

            char[] pattern = solution.ToArray();
            foreach (char color in pattern)
                Debug.Assert(color == '\n' || (color >= 'A' && color < 'A' + 12));

            string[] lines = solution.Split('\n');
            char[][] board = new char[lines.Length][];
            for (int i = 0; i < lines.Length; ++i)
                board[i] = lines[i].ToArray();

            for (int i = 1; i < (1 << 12) - 1; ++i)
            {
                int t = i;
                int placed_pieces = 0;
                for (char color = 'A'; color < 'A' + 12; ++color)
                {
                    if (t % 2 == 0)
                        lookup[color] = ' ';
                    else
                    {
                        lookup[color] = color;
                        ++placed_pieces;
                    }
                    t /= 2;
                }

                if (placed_pieces < min_placed_pieces || placed_pieces > max_placed_pieces)
                    continue;

                for (int j = 0; j < pattern.Length; ++j)
                    pattern[j] = lookup[solution[j]];

                string pattern_str = new string(pattern);
                if (challenges.ContainsKey(pattern_str))
                    challenges[pattern_str].IncreaseSolutionCount();
                else if (IsGoodPattern(pattern, board))
                    challenges[pattern_str] = new Challenge(pattern_str, placed_pieces);
            }
        }

        private bool IsGoodPattern(char[] pattern, char[][] board)
        {
            int row = 0, column = 0;
            for (int j = 0; j < pattern.Length; ++j)
            {
                if (pattern[j] == '\n')
                {
                    Debug.Assert(row < board.Length && column == board[row].Length);
                    ++row;
                    column = 0;
                }
                else
                {
                    Debug.Assert(row < board.Length && column < board[row].Length);
                    board[row][column] = pattern[j];
                    ++column;
                }
            }

            if (ProblemOp.GetHoleCount(board, 55) > 1)
                return false;

            for (int i = 0; i < board.Length; ++i)
            {
                char[] line = board[i];
                for (int j = 0; j < line.Length; ++j)
                    line[j] = (line[j] == ' ' ? 'A' : ' ');
            }

            if (ProblemOp.GetHoleCount(board, 55) > 1)
                return false;

            return true;
        }
    }
}
