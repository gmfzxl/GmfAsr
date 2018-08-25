namespace NativeAsrCSharp
{
    partial class EngineForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.buttonInit = new System.Windows.Forms.Button();
            this.listBoxResult = new System.Windows.Forms.ListBox();
            this.buttonInsert = new System.Windows.Forms.Button();
            this.buttonStart = new System.Windows.Forms.Button();
            this.buttonCancel = new System.Windows.Forms.Button();
            this.checkBoxSaveRecord = new System.Windows.Forms.CheckBox();
            this.labelState = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.checkBox8K = new System.Windows.Forms.CheckBox();
            this.checkBox16K = new System.Windows.Forms.CheckBox();
            this.buttonTest = new System.Windows.Forms.Button();
            this.checkBoxNoise = new System.Windows.Forms.CheckBox();
            this.textNoise = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // buttonInit
            // 
            this.buttonInit.Location = new System.Drawing.Point(12, 12);
            this.buttonInit.Name = "buttonInit";
            this.buttonInit.Size = new System.Drawing.Size(75, 23);
            this.buttonInit.TabIndex = 0;
            this.buttonInit.Text = "引擎初始化";
            this.buttonInit.UseVisualStyleBackColor = true;
            this.buttonInit.Click += new System.EventHandler(this.button1_Click);
            // 
            // listBoxResult
            // 
            this.listBoxResult.FormattingEnabled = true;
            this.listBoxResult.ItemHeight = 12;
            this.listBoxResult.Location = new System.Drawing.Point(12, 129);
            this.listBoxResult.Name = "listBoxResult";
            this.listBoxResult.Size = new System.Drawing.Size(892, 256);
            this.listBoxResult.TabIndex = 2;
            this.listBoxResult.SelectedIndexChanged += new System.EventHandler(this.listBoxResult_SelectedIndexChanged);
            // 
            // buttonInsert
            // 
            this.buttonInsert.Location = new System.Drawing.Point(12, 41);
            this.buttonInsert.Name = "buttonInsert";
            this.buttonInsert.Size = new System.Drawing.Size(75, 23);
            this.buttonInsert.TabIndex = 3;
            this.buttonInsert.Text = "更换模型";
            this.buttonInsert.UseVisualStyleBackColor = true;
            this.buttonInsert.Click += new System.EventHandler(this.buttonInsert_Click);
            // 
            // buttonStart
            // 
            this.buttonStart.Location = new System.Drawing.Point(12, 70);
            this.buttonStart.Name = "buttonStart";
            this.buttonStart.Size = new System.Drawing.Size(75, 23);
            this.buttonStart.TabIndex = 5;
            this.buttonStart.Text = "开始识别";
            this.buttonStart.UseVisualStyleBackColor = true;
            this.buttonStart.Click += new System.EventHandler(this.buttonStart_Click);
            // 
            // buttonCancel
            // 
            this.buttonCancel.Location = new System.Drawing.Point(12, 100);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(75, 23);
            this.buttonCancel.TabIndex = 6;
            this.buttonCancel.Text = "关闭识别";
            this.buttonCancel.UseVisualStyleBackColor = true;
            this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
            // 
            // checkBoxSaveRecord
            // 
            this.checkBoxSaveRecord.AutoSize = true;
            this.checkBoxSaveRecord.Location = new System.Drawing.Point(93, 74);
            this.checkBoxSaveRecord.Name = "checkBoxSaveRecord";
            this.checkBoxSaveRecord.Size = new System.Drawing.Size(72, 16);
            this.checkBoxSaveRecord.TabIndex = 7;
            this.checkBoxSaveRecord.Text = "保存录音";
            this.checkBoxSaveRecord.UseVisualStyleBackColor = true;
            this.checkBoxSaveRecord.CheckedChanged += new System.EventHandler(this.checkBoxSaveRecord_CheckedChanged);
            // 
            // labelState
            // 
            this.labelState.AutoSize = true;
            this.labelState.Location = new System.Drawing.Point(93, 17);
            this.labelState.Name = "labelState";
            this.labelState.Size = new System.Drawing.Size(53, 12);
            this.labelState.TabIndex = 8;
            this.labelState.Text = "未初始化";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(327, 17);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 9;
            this.button1.Text = "释放引擎";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click_1);
            // 
            // checkBox8K
            // 
            this.checkBox8K.AutoSize = true;
            this.checkBox8K.Location = new System.Drawing.Point(195, 76);
            this.checkBox8K.Name = "checkBox8K";
            this.checkBox8K.Size = new System.Drawing.Size(36, 16);
            this.checkBox8K.TabIndex = 10;
            this.checkBox8K.Text = "8k";
            this.checkBox8K.UseVisualStyleBackColor = true;
            this.checkBox8K.CheckedChanged += new System.EventHandler(this.checkBox8K_CheckedChanged);
            // 
            // checkBox16K
            // 
            this.checkBox16K.AutoSize = true;
            this.checkBox16K.Checked = true;
            this.checkBox16K.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBox16K.Location = new System.Drawing.Point(251, 77);
            this.checkBox16K.Name = "checkBox16K";
            this.checkBox16K.Size = new System.Drawing.Size(42, 16);
            this.checkBox16K.TabIndex = 11;
            this.checkBox16K.Text = "16k";
            this.checkBox16K.UseVisualStyleBackColor = true;
            this.checkBox16K.CheckedChanged += new System.EventHandler(this.checkBox16K_CheckedChanged);
            // 
            // buttonTest
            // 
            this.buttonTest.Location = new System.Drawing.Point(156, 12);
            this.buttonTest.Name = "buttonTest";
            this.buttonTest.Size = new System.Drawing.Size(124, 23);
            this.buttonTest.TabIndex = 12;
            this.buttonTest.Text = "测试音频文件";
            this.buttonTest.UseVisualStyleBackColor = true;
            this.buttonTest.Click += new System.EventHandler(this.buttonTest_Click);
            // 
            // checkBoxNoise
            // 
            this.checkBoxNoise.AutoSize = true;
            this.checkBoxNoise.Location = new System.Drawing.Point(327, 76);
            this.checkBoxNoise.Name = "checkBoxNoise";
            this.checkBoxNoise.Size = new System.Drawing.Size(72, 16);
            this.checkBoxNoise.TabIndex = 13;
            this.checkBoxNoise.Text = "是否加噪";
            this.checkBoxNoise.UseVisualStyleBackColor = true;
            // 
            // textNoise
            // 
            this.textNoise.Location = new System.Drawing.Point(488, 72);
            this.textNoise.Name = "textNoise";
            this.textNoise.Size = new System.Drawing.Size(100, 21);
            this.textNoise.TabIndex = 14;
            this.textNoise.Text = "1.0";
            this.textNoise.TextChanged += new System.EventHandler(this.textNoise_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(428, 79);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(53, 12);
            this.label1.TabIndex = 15;
            this.label1.Text = "加噪系数";
            // 
            // EngineForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(916, 395);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.textNoise);
            this.Controls.Add(this.checkBoxNoise);
            this.Controls.Add(this.buttonTest);
            this.Controls.Add(this.checkBox16K);
            this.Controls.Add(this.checkBox8K);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.labelState);
            this.Controls.Add(this.checkBoxSaveRecord);
            this.Controls.Add(this.buttonCancel);
            this.Controls.Add(this.buttonStart);
            this.Controls.Add(this.buttonInsert);
            this.Controls.Add(this.listBoxResult);
            this.Controls.Add(this.buttonInit);
            this.Name = "EngineForm";
            this.Text = "云知声";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.EngineForm_FormClosed);
            this.Load += new System.EventHandler(this.EngineForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonInit;
        private System.Windows.Forms.ListBox listBoxResult;
        private System.Windows.Forms.Button buttonInsert;
        private System.Windows.Forms.Button buttonStart;
        private System.Windows.Forms.Button buttonCancel;
        private System.Windows.Forms.CheckBox checkBoxSaveRecord;
        private System.Windows.Forms.Label labelState;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.CheckBox checkBox8K;
        private System.Windows.Forms.CheckBox checkBox16K;
        private System.Windows.Forms.Button buttonTest;
        private System.Windows.Forms.CheckBox checkBoxNoise;
        private System.Windows.Forms.TextBox textNoise;
        private System.Windows.Forms.Label label1;
    }
}

