using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Drawing;
using System.Reflection;
using System.Drawing.Text;

namespace fileproc {
	static class Utils {
		static Random Rndm = new Random();

		public static T Rnd<T>(this IEnumerable<T> Collection) {
			T[] Arr = Collection.ToArray();
			return Arr[Rndm.Next(Arr.Length)];
		}
	}

	class Program {
		static string DataName = "data";
		static string BinsName = "bins";
		static string BinsDir;
		static string DataDir;
		static string GrubCfg;

		static List<string> ProcessedFiles = new List<string>();


		static void Main(string[] args) {
			string ExeLocation = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
			BinsDir = Path.Combine(ExeLocation, BinsName);
			DataDir = Path.Combine(ExeLocation, DataName);
			GrubCfg = Path.Combine(ExeLocation, "boot", "grub", "grub.cfg");

			if (!Directory.Exists(DataDir))
				return;

			if (!Directory.Exists(BinsDir))
				Directory.CreateDirectory(BinsDir);

			string[] Files = Directory.EnumerateFiles(DataDir).ToArray();
			foreach (var F in Files)
				Convert(Path.GetFullPath(F));

			string CfgContent = File.ReadAllText(GrubCfg);
			string ProcessedFilesPaths = string.Join("\n\t", ProcessedFiles.Select((F) => string.Format("module /{0} {0}", F))).Replace("\\", "/");
			CfgContent = CfgContent.Replace("#(BOOT_MODULES)", ProcessedFilesPaths);
			File.WriteAllText(GrubCfg, CfgContent);
		}

		static void Convert(string Pth) {
			string Out = Path.Combine(BinsDir, Path.GetFileNameWithoutExtension(Pth) + ".bin");
			string Ext = Path.GetExtension(Pth).ToLower();

			Console.WriteLine("Converting {0}", Pth);

			if (new string[] { ".bmp", ".gif", ".jpeg", ".jpg", ".png", ".tiff" }.Contains(Ext))
				ConvertImage(Pth, Out);
			else if (new string[] { ".ttf" }.Contains(Ext))
				ConvertFont(Pth, Out);
			else
				throw new Exception("Unsupported file extension " + Ext);

			ProcessedFiles.Add(Path.Combine(BinsName, Path.GetFileName(Out)));
		}

		static void ConvertImage(Image Img, string OutFile) {
			using (BinaryWriter BW = new BinaryWriter(File.OpenWrite(OutFile))) {
				using (Bitmap Bmp = new Bitmap(Img)) {
					BW.Write((int)Bmp.Width);
					BW.Write((int)Bmp.Height);

					for (int y = 0; y < Bmp.Height; y++)
						for (int x = 0; x < Bmp.Width; x++) {
							Color Clr = Bmp.GetPixel(x, y);

							BW.Write(Clr.R);
							BW.Write(Clr.G);
							BW.Write(Clr.B);
							BW.Write(Clr.A);
						}
				}
			}
		}

		static void ConvertImage(string InFile, string OutFile) {
			ConvertImage(Image.FromFile(InFile), OutFile);
		}

		static void ConvertFont(string InFile, string OutFile) {
			const int FSize = 16;

			PrivateFontCollection PFC = new PrivateFontCollection();
			PFC.AddFontFile(InFile);
			Font Fnt = new Font(PFC.Families[0], FSize);

			using (Bitmap Bmp = new Bitmap(FSize * 16, FSize * 16)) {
				using (Graphics Gfx = Graphics.FromImage(Bmp)) {
					Gfx.TextRenderingHint = TextRenderingHint.AntiAliasGridFit;

					for (int x = 0; x < 16; x++)
						for (int y = 0; y < 16; y++) {
							Gfx.FillRectangle((Brush)typeof(Brushes).GetProperties().Rnd().GetGetMethod().Invoke(null, null), x * FSize, y * FSize, FSize, FSize);
							Gfx.DrawString(((char)(y * 16 + x)).ToString(), Fnt, Brushes.White, x * FSize, y * FSize);
						}


				}

				Bmp.Save("TEST.png");
				ConvertImage(Bmp, OutFile);
			}
		}
	}
}
