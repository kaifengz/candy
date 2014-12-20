using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace IQCar
{
    class PuzzleLibrary
    {
        public class LoadError : Exception
        {
        }

        public class SaveError : Exception
        {
        }

        private PuzzleLibrary()
        {
        }

        public static readonly PuzzleLibrary Instance = new PuzzleLibrary();

        public void LoadPuzzles(string filename)
        {
            try
            {
                using (Stream stream = new FileStream(filename, FileMode.Open))
                {
                    LoadPuzzles(stream);
                }
            }
            catch (LoadError err)
            {
                throw err;
            }
            catch
            {
                throw new LoadError();
            }
        }

        public void LoadPuzzles(Stream stream)
        {
            try
            {
                XmlDocument doc = new XmlDocument();
                doc.Load(stream);
                XmlElement root = (XmlElement)doc.FirstChild;
                foreach (XmlNode node in root.ChildNodes)
                {
                    try
                    {
                        XmlElement elem = (XmlElement)node;
                        if (elem.Name != "Puzzle")
                            continue;

                        string name = elem.GetAttribute("name");
                        string placement_str = elem.GetAttribute("placement");
                        string color_map_str = elem.GetAttribute("color-map");

                        Dictionary<char, Color> color_map = ColorMapFromString(color_map_str);
                        Placement placement = Placement.FromString(placement_str, color_map);
                        if (placement != null)
                            SetPuzzle(name, placement);
                    }
                    catch (Exception exc)
                    {
                        Debug.Print(exc.Message);
                    }
                }
            }
            catch
            {
                throw new LoadError();
            }
        }

        public void SavePuzzles(string filename)
        {
            using (Stream stream = new FileStream(filename, FileMode.Create))
            {
                SavePuzzles(stream);
            }
        }

        public void SavePuzzles(Stream stream)
        {
            if (puzzles == null)
                return;

            XmlDocument doc = new XmlDocument();
            XmlElement root = doc.CreateElement("Puzzles");
            doc.AppendChild(root);

            foreach (var vpuzzle in puzzles)
            {
                string name = vpuzzle.Key;
                Placement placement = vpuzzle.Value;

                Dictionary<char, Color> color_map = new Dictionary<char, Color>();
                string placement_str = placement.ToString(color_map);
                string color_map_str = ColorMapToString(color_map);

                XmlElement puzzle = doc.CreateElement("Puzzle");
                puzzle.SetAttribute("name", name);
                puzzle.SetAttribute("placement", placement_str);
                puzzle.SetAttribute("color-map", color_map_str);
                root.AppendChild(puzzle);
            }

            doc.Save(stream);
        }

        public Placement GetPuzzle(string name)
        {
            Placement placement;
            return puzzles.TryGetValue(name, out placement) ? placement : null;
        }

        public void SetPuzzle(string name, Placement placement)
        {
            puzzles[name] = placement;
        }

        public IEnumerable<KeyValuePair<string, Placement>> EnumPuzzles()
        {
            return puzzles.AsEnumerable();
        }

        public int Count
        {
            get
            {
                return puzzles.Count;
            }
        }

        public KeyValuePair<string, Placement>? First
        {
            get
            {
                if (puzzles.Count > 0)
                    return puzzles.First();
                return null;
            }
        }

        public KeyValuePair<string, Placement>? Next(string name)
        {
            var next = puzzles.FirstOrDefault(delegate(KeyValuePair<string, Placement> pair)
            {
                return string.Compare(pair.Key, name) > 0;
            });

            if (next.Key != null)
                return next;
            return null;
        }

        public KeyValuePair<string, Placement>? Previous(string name)
        {
            var prev = puzzles.LastOrDefault(delegate(KeyValuePair<string, Placement> pair)
            {
                return string.Compare(pair.Key, name) < 0;
            });

            if (prev.Key != null)
                return prev;
            return null;
        }

        private static string ColorMapToString(Dictionary<char, Color> color_map)
        {
            return string.Join(";", from char_color in color_map
                                    select string.Format("{0}:{1:X}", char_color.Key, char_color.Value.ToArgb()));
        }

        private static Dictionary<char, Color> ColorMapFromString(string s)
        {
            Dictionary<char, Color> color_map = new Dictionary<char, Color>();
            foreach (string pair in s.Split(';'))
            {
                try
                {
                    string[] char_color = pair.Split(':');
                    if (char_color.Length != 2)
                        continue;
                    if (char_color[0].Length != 1)
                        continue;
                    int color = int.Parse(char_color[1], System.Globalization.NumberStyles.HexNumber);
                    color_map[char_color[0][0]] = Color.FromArgb(color);
                }
                catch
                {
                    continue;
                }
            }
            return color_map.Count > 0 ? color_map : null;
        }

        private SortedDictionary<string, Placement> puzzles = new SortedDictionary<string, Placement>();
    }
}
