using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;


namespace NativeAsrCSharp
{
    public partial class EngineForm : Form
    {
        private Boolean micOut = false;
        static int ENGINE_STATE_NOT_INIT = 1000;
        static int ENGINE_STATE_IDLE = 1001;
        static int ENGINE_STATE_ASR = 1002;


        static int ERROR_READ_RECORD_ERROR = -986;
        private eventCallBack callBack;

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void eventCallBack(int code, string result, int data);

        [DllImport(@"D:\code\repos\NativeAsrCSharp\Debug\NativeAsrDll.dll")]
        static extern int initEngine(eventCallBack callBack);

        [DllImport(@"D:\code\repos\NativeAsrCSharp\Debug\NativeAsrDll.dll")]
        static extern int startAsr();
        [DllImport(@"D:\code\repos\NativeAsrCSharp\Debug\NativeAsrDll.dll")]
        static extern void cancelAsr();

        [DllImport(@"D:\code\repos\NativeAsrCSharp\Debug\NativeAsrDll.dll")]
        static extern int insertVocab(string vocab);

        [DllImport(@"D:\code\repos\NativeAsrCSharp\Debug\NativeAsrDll.dll")]
        static extern void saveRecord(int save);

        [DllImport(@"D:\code\repos\NativeAsrCSharp\Debug\NativeAsrDll.dll")]
        static extern IntPtr getVersion();

        [DllImport(@"D:\code\repos\NativeAsrCSharp\Debug\NativeAsrDll.dll")]
        static extern int getEngineState();

        [DllImport(@"D:\code\repos\NativeAsrCSharp\Debug\NativeAsrDll.dll")]
        static extern void setCallBack(eventCallBack callBack);
        public EngineForm()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            callBack = new eventCallBack(engineCallBack); ;
            int ret = initEngine(callBack);
            if (ret == 0)
            {
                IntPtr temp = getVersion();
                string version = Marshal.PtrToStringAnsi(temp).ToString();
                this.Text = "云知声 " + version;
                MessageBox.Show("初始化成功!");
            }
            else
            {
                MessageBox.Show("初始化失败!");
            }
        }


        public string getVocabString(string tagName, List<string> strList)
        {
            StringBuilder buffer = new StringBuilder();
            buffer.Append("<" + tagName + ">").Append("\n");
            foreach (string name in strList)
            {
                buffer.Append(name).Append("\n");
            }
            buffer.Append("</" + tagName + ">");
            return buffer.ToString();
        }

        public void engineCallBack(int code, string result, int data)
        {
            if (result != null)
            {
                Action<string> action = (str) =>
                {
                    listBoxResult.Items.Add(str);
                    this.listBoxResult.SelectedIndex = this.listBoxResult.Items.Count - 1;
                };
                this.BeginInvoke(action, result);
            }
            if (data == ERROR_READ_RECORD_ERROR)
            {
                micOut = true;
                Action<int> action = (value) =>
                {
                    DialogResult dr = MessageBox.Show("读取录音失败,是否拔出了麦克风设备,请插入后重新启动!", "警告", MessageBoxButtons.OK, MessageBoxIcon.Question);
                    if (dr == DialogResult.OK)
                    {
                        Close();
                    }
                };
                this.BeginInvoke(action, data);
            }
            if (code == 999)
            {
                Action<int> action = (value) =>
                {
                    if (value == ENGINE_STATE_ASR)
                    {
                        labelState.Text = "识别";
                    }
                    if (value == ENGINE_STATE_IDLE)
                    {
                        labelState.Text = "空闲";
                    }
                    if (value == ENGINE_STATE_NOT_INIT)
                    {
                        labelState.Text = "未初始化";
                    }
                };
                this.BeginInvoke(action, data);

            }
        }

        private void EngineForm_Load(object sender, EventArgs e)
        {

        }

        private void EngineForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            setCallBack(null);
            if (!micOut)
            {
                cancelAsr();
            }
        }

        private void listBoxResult_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void buttonInsert_Click(object sender, EventArgs e)
        {
            List<string> list = new List<string>();
            string text = textBoxInsert.Text.ToString();
            String[] array = text.Split('#');
            if (array == null || array.Length == 0)
            {
                MessageBox.Show("请输入有效词表!");
            }
            foreach (string str in array)
            {
                list.Add(str);
            }
            String vocab = "Domain_userdef_commands_slot";
            string result = getVocabString(vocab, list);
            int ret = insertVocab(result);
            if (ret == 0)
            {
                MessageBox.Show("插入词表成功!");
            }
            else
            {
                MessageBox.Show("插入词表失败!");
            }
        }

        private void buttonStart_Click(object sender, EventArgs e)
        {
            if (startAsr() == 0)
            {
                MessageBox.Show("开始识别!");
            }
            else
            {
                MessageBox.Show("开启识别失败!");
            }
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            cancelAsr();
        }

        private void checkBoxSaveRecord_CheckedChanged(object sender, EventArgs e)
        {
            int save = 0;
            if (checkBoxSaveRecord.Checked)
            {
                save = 1;
            }
            saveRecord(save);
        }

    }
}
