using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace POV
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Main : Page
    {
        public Main()
        {
            this.InitializeComponent();
        }

        private RenderTargetBitmap buffer;
        private DrawingVisual drawingVisual = new DrawingVisual();


        protected override void OnRender(DrawingContext drawingContext)
        {
            base.OnRender(drawingContext);
            buffer = new RenderTargetBitmap((int)Background.Width, (int)Background.Height, 96, 96, PixelFormats.Pbgra32);
            Background.Source = buffer;
            DrawStuff();
        }

        private void DrawStuff()
        {
            if (buffer == null)
                return;

            using (DrawingContext drawingContext = drawingVisual.RenderOpen())
            {
                drawingContext.DrawRectangle(new SolidColorBrush(Colors.Red), null, new Rect(0, 0, 10, 10));
            }

            buffer.Render(drawingVisual);
        }
    }
}
