using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace IQCar
{
    class PuzzleLibrary
    {
        public void LoadPuzzles(string filename)
        {
            XmlDocument doc = new XmlDocument();
            doc.Load(filename);
            XmlElement root = (XmlElement)doc.FirstChild;
            foreach (XmlNode node in root.ChildNodes)
            {
                if (!(node is XmlElement))
                    continue;

                XmlElement elem = (XmlElement)node;
                if (elem.Name != "Puzzle")
                    continue;

                // TODO:
            }
        }

        public void SavePuzzles(string filename)
        {
            if (puzzles == null)
                return;
        }

        private Dictionary<string, Placement> puzzles;
    }
}
