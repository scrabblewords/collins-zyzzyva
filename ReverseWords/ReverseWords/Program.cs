using System;
using System.IO;
using System.Diagnostics;

namespace ReverseWords
{
    class Program
    {
        static void Main(string[] args)
        {
            StreamWriter writer = new StreamWriter(@"C:\ZyzzyvaV-trunk\CSW19-R.txt");


            using (StreamReader reader = File.OpenText(@"C:\ZyzzyvaV-trunk\CSW19-NODEF.txt"))
            {
                while (!reader.EndOfStream)
                {
                    string word = reader.ReadLine();


                    char[] charArray = word.ToCharArray();
                    Array.Reverse(charArray);
                    string reversedWord = new string(charArray);

                    Debug.WriteLine(reversedWord);

                    writer.WriteLine(reversedWord);


                }
            }

            writer.Close();

        }
    }
}
