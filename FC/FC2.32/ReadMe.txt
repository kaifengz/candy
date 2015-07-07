..	文件					代码长度（行）
■	basic.cpp				 135
■	FC-Compile.cpp			3674
■	FC-GenerateFile.cpp		 572
■	FC-Interpret.cpp		1324
■	FC-MenuMessage.cpp		 154
■	FC-Morphology.cpp		 561
■	FC-Translate.cpp		1152
■	FCErrorReportDlg.cpp	 214
■	FCInputDlg.cpp			 193
■	FCInterpretDlg.cpp		 147
■	FCOptionDlg.cpp			 219
■	FCTextDlg.cpp			 218
■	FCTranslateDlg.cpp		 142
■	FCView.cpp				 175
■	basic.h					  15
■	FC-define.h				 399
■	FCErrorReportDlg.h		  49
■	FCInputDlg.h			  64
■	FCInterpretDlg.h		  98
■	FCOptionDlg.h			  71
■	FCTextDlg.h				  54
■	FCTranslateDlg.h		  64
■	FCView.h				 224
	总计					9918

###############################################################################

..开始开发FC2.31(2003.11.06)

■	不在版本2.3语言上做任何改动
■	修改任何有版本2.3信息的地方
■	增加选项的保存功能，下次打开时能恢复上次的选项设置(2003.12.28)
■	使用VC的cl编译生成的CPP文件

□	进一步清除BUG
	■	发现BUG，数组作为参数调用时有错误
	■	解释执行中空操作没有“iCode++”
	■	不能正确计算average([]double,int)、average([]int,int)
	■	不能正确的输出'\0'

###############################################################################

..开始开发FC2.32(2004.02.20)

■	不在版本2.31语言上做任何改动
■	修改任何有版本2.31信息的地方
■	不再使用VC的cl编译生成的CPP文件
□	修改代码，使代码更符合规范
	■	删除FCTextDlg.cpp、FCTextDlg.h，使用记事本显示帮助
	□	修改ini文件的读写方式
■	不再编写外部解释执行程序，改用可执行的壳加伪代码实现外部的解释执行
■	编译前自动保存
■	在程序中添加FC程序示例

□	进一步清除BUG
	■	不能正确的输入实数
