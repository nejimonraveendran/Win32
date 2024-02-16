using System.Runtime.InteropServices;

namespace CSharpApp
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        const string cppLib = @"\..\..\..\..\x64\Debug\CppLib.dll";

        [DllImport(cppLib, EntryPoint = "Add", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Unicode)]
        private static extern int Add(int num1, int num2);


        [return: MarshalAs(UnmanagedType.BStr)]
        [DllImport(cppLib, EntryPoint = "GetText", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Unicode)]
        private static extern string GetText();


        private void btnAdd_Click(object sender, EventArgs e)
        {
            var res = Add(1, 2);
            MessageBox.Show($"Answer is {res}");
        }

        private void btnGetText_Click(object sender, EventArgs e)
        {
            var res = GetText();
            MessageBox.Show(res.ToString());
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
    }
}