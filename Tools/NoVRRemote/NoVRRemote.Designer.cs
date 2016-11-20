using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NoVR
{
    public partial class NoVRRemote
    {
        private void InitializeComponent()
        {
            this.sendButton = new System.Windows.Forms.Button();
            this.idBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.xBox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.yBox = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.zBox = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // sendButton
            // 
            this.sendButton.Location = new System.Drawing.Point(12, 12);
            this.sendButton.Name = "sendButton";
            this.sendButton.Size = new System.Drawing.Size(150, 68);
            this.sendButton.TabIndex = 0;
            this.sendButton.Text = "Send";
            this.sendButton.UseVisualStyleBackColor = true;
            this.sendButton.Click += new System.EventHandler(this.sendButton_Click);
            // 
            // idBox
            // 
            this.idBox.Location = new System.Drawing.Point(237, 12);
            this.idBox.Name = "idBox";
            this.idBox.Size = new System.Drawing.Size(100, 22);
            this.idBox.TabIndex = 1;
            this.idBox.TextChanged += new System.EventHandler(this.idBox_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(168, 12);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(19, 17);
            this.label1.TabIndex = 2;
            this.label1.Text = "Id";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(168, 40);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(17, 17);
            this.label2.TabIndex = 4;
            this.label2.Text = "X";
            // 
            // xBox
            // 
            this.xBox.Location = new System.Drawing.Point(237, 40);
            this.xBox.Name = "xBox";
            this.xBox.Size = new System.Drawing.Size(100, 22);
            this.xBox.TabIndex = 3;
            this.xBox.TextChanged += new System.EventHandler(this.xBox_TextChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(168, 68);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(17, 17);
            this.label3.TabIndex = 6;
            this.label3.Text = "Y";
            // 
            // yBox
            // 
            this.yBox.Location = new System.Drawing.Point(237, 68);
            this.yBox.Name = "yBox";
            this.yBox.Size = new System.Drawing.Size(100, 22);
            this.yBox.TabIndex = 5;
            this.yBox.TextChanged += new System.EventHandler(this.yBox_TextChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(168, 96);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(17, 17);
            this.label4.TabIndex = 8;
            this.label4.Text = "Z";
            // 
            // zBox
            // 
            this.zBox.Location = new System.Drawing.Point(237, 96);
            this.zBox.Name = "zBox";
            this.zBox.Size = new System.Drawing.Size(100, 22);
            this.zBox.TabIndex = 7;
            this.zBox.TextChanged += new System.EventHandler(this.zBox_TextChanged);
            // 
            // VrSimRemote
            // 
            this.ClientSize = new System.Drawing.Size(377, 203);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.zBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.yBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.xBox);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.idBox);
            this.Controls.Add(this.sendButton);
            this.Name = "NoVRRemote";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        private System.Windows.Forms.TextBox idBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox xBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox yBox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox zBox;
    }
}
