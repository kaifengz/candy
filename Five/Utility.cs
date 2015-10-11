using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;

namespace Five
{
    class Utility
    {
        private static Random random = new Random();

        public static Random Random
        {
            get
            {
                return random;
            }
        }

        public static void Shuffle<T>(List<T> list)
        {
            for (int i = 0; i < list.Count; ++i)
            {
                int j = random.Next(list.Count);
                if (i != j)
                {
                    T tmp = list[i];
                    list[i] = list[j];
                    list[j] = tmp;
                }
            }
        }

        public static IEnumerable<T> Dedup<T>(IEnumerable<T> items)
        {
            HashSet<T> occurs = new HashSet<T>();
            foreach (T item in items)
            {
                if (!occurs.Contains(item))
                {
                    occurs.Add(item);
                    yield return item;
                }
            }
        }

        [TestClass]
        public class UnitTest
        {
            [TestMethod]
            public void Test_Shuffle()
            {
                List<int> list = new List<int> { 0, 1, 2, 3, 4 };
                Utility.Shuffle(list);
                list.Sort();
                for (int i = 0; i < list.Count; ++i)
                    Assert.AreEqual(i, list[i]);
            }
        }
    }
}
