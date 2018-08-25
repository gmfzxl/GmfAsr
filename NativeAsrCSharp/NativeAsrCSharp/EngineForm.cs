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
        private int count = 0;

        const string DLL_ADDRESS = @"D:\code\gmfasr\GmfAsr\NativeAsrCSharp\Debug\NativeAsrDll.dll";


        static int ERROR_READ_RECORD_ERROR = -986;
        private eventCallBack callBack;

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void eventCallBack(int code, string result, int data);

        [DllImport(DLL_ADDRESS)]
        static extern int initEngine(string grammerPath, string tagName, eventCallBack callBack);

        [DllImport(DLL_ADDRESS)]
        static extern int startAsr();
        [DllImport(DLL_ADDRESS)]
        static extern void cancelAsr();

        [DllImport(DLL_ADDRESS)]
        static extern void saveRecord(int save);

        [DllImport(DLL_ADDRESS)]
        static extern IntPtr getVersion();

        [DllImport(DLL_ADDRESS)]
        static extern int getEngineState();

        [DllImport(DLL_ADDRESS)]
        static extern void setCallBack(eventCallBack callBack);

        [DllImport(DLL_ADDRESS)]
        static extern int resetGrammer(string grammerPath, string tag);

        [DllImport(DLL_ADDRESS)]
        static extern int releaseEngine();

        [DllImport(DLL_ADDRESS)]
        static extern int setSamplesPerSec(int sample);

        [DllImport(DLL_ADDRESS)]
        static extern int startTest(string testPath);

        [DllImport(DLL_ADDRESS)]
        static extern int setNoiseData(int addNoise, string noisePath, float ratio);

        public EngineForm()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            callBack = new eventCallBack(engineCallBack); ;
            int ret = initEngine(@"..\grammar\tv_main.dat", "tv_main", callBack);
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
            int ret;
            string text;
            if (count % 2 == 0)
            {
                ret = resetGrammer(@"..\grammar\air_main.dat", @"air_main");
                text = @"当前命令是打开/关闭空调";
            }
            else
            {
                ret = resetGrammer(@"..\grammar\tv_main.dat", @"tv_main");
                text = @"当前命令是打开/关闭电视";
            }
            if (ret == 0)
            {
                count++;
                MessageBox.Show("切换成功!" + text);
            }
            else
            {
                MessageBox.Show("切换失败!");
            }
        }

        private void buttonStart_Click(object sender, EventArgs e)
        {
            string noiseFile;
            if (checkBox8K.Checked)
            {
                noiseFile = "noise8k.wav";
                setSamplesPerSec(8000);
            }
            else
            {
                noiseFile = "noise16k.wav";
                setSamplesPerSec(16000);
            }
            try
            {
                float ration = float.Parse(textNoise.Text);
                if (checkBoxNoise.Checked)
                {
                    setNoiseData(1, noiseFile, ration);
                }
                else
                {
                    setNoiseData(0, null, ration);
                }
                if (startAsr() == 0)
                {
                    MessageBox.Show("开始识别!");
                }
                else
                {
                    MessageBox.Show("开启识别失败!");
                }
            }
            catch
            {
                MessageBox.Show("加噪要是浮点数!");
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

        private void button1_Click_1(object sender, EventArgs e)
        {
            releaseEngine();
        }

        private void checkBox8K_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox8K.Checked)
            {
                checkBox16K.Checked = false;
            }
        }

        private void checkBox16K_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox16K.Checked)
            {
                checkBox8K.Checked = false;
            }
        }

        private void buttonTest_Click(object sender, EventArgs e)
        {
            startTest(@"Record\");
        }

        private void textNoise_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
