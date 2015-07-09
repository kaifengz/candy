#include "stdafx.h"
#include "FC.h"

#include "FCDoc.h"
#include "FCView.h"

#include "basic.h"
#include "FCTranslateDlg.h"
#include "FCOptionDlg.h"
#include "FCInterpretDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CFCView::OnCompileCompile() 
{
	SendMessage(WM_COMMAND, ID_FILE_SAVE, 0);

	if(GetDocument()->IsModified())
	{
		MessageBox("保存文件失败，无法编译程序！", "FC", MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	if(!Compile() || m_iErrorCount>0)
	{
		CString message;
		if(m_iErrorCount>0)
			message.Format("源代码中发现 %d 处语法语义错误，请修正源代码！",m_iErrorCount);
		else
			message="编译时本程序发生错误，请与程序提供者联系！";
		MessageBox(message,"FC",MB_OK|MB_ICONEXCLAMATION);
		m_bCompiled=FALSE;
	}
	else
	{
		m_bCompiled=TRUE;

		CString strPath		= GetFileName(GetDocument()->GetPathName());
		CString strTitle	= GetFileName(GetDocument()->GetTitle());
		CString strFCRFile	= GetTempFCRFilePath();
		if(strTitle!="")
		{
			GenerateBinaryFile(strFCRFile, strTitle);//生成二进制可解释执行文件
			if(m_bGenerateBinary)//拷贝二进制可解释执行文件
				::CopyFile(strFCRFile, strPath+".fcr", FALSE);
			if(m_bGenerateText)//生成文本文件
				GenerateTextFile(strPath+".txt", strTitle);
			GenerateExeFile(strPath+".exe", strFCRFile, strTitle);//生成可执行文件
		}
	}
}

void CFCView::OnCompileRebuild() 
{
	m_bCompiled = FALSE;
	OnCompileCompile();
}

void CFCView::OnCompileInterpret() 
{
	OnCompileCompile();
	if(m_bCompiled)
	{
		CFCInterpretDlg dlg;
		dlg.DoModal(m_pCode,m_nCode,m_bCalcRuntime,m_bFileOutput,
			m_bIgnoreCircleNoend,m_bOutputInput,
			m_strOutputFile,m_nPrecision,m_nMemoryApply,
			GetFileName(GetDocument()->GetTitle()));
	}
}

void CFCView::OnCompileExecute() 
{
	OnCompileCompile();
	if(m_bCompiled)
	{
		CString strExeFile = GetFileName(GetDocument()->GetPathName())+".exe";
		WinExec(strExeFile, SW_SHOWDEFAULT);
	}
}

void CFCView::OnCompileTranslate() 
{
	OnCompileCompile();
	if(!m_bCompiled)
		return;

	CFCTranslateDlg dlg;
	if(dlg.DoModal(GetFileName(GetDocument()->GetPathName()),
		m_bInsertSpace,m_bEmptyLine,m_iBackChoice,m_iBackNumber,
		m_bKeepComment,m_bOutPrecision,m_bRunTime,m_bWaitEnd)!=IDOK)
			return;

	m_bInsertSpace	= dlg.m_bInsertSpace;
	m_bEmptyLine	= dlg.m_bEmptyLine;
	m_iBackChoice	= dlg.m_iBackChoice;
	m_iBackNumber	= dlg.m_iBackNumber;
	m_bKeepComment	= dlg.m_bKeepComment;
	m_bOutPrecision	= dlg.m_bOutPrecision;
	m_bRunTime		= dlg.m_bRunTime;
	m_bWaitEnd		= dlg.m_bWaitEnd;
	m_strCppFile	= dlg.m_strObjectFile;

	if(Translate()) // 翻译成功
	{
		MessageBox(
			GetDocument()->GetPathName()+"\n\n已经成功的翻译到\n\n"+m_strCppFile,
			"FC",
			MB_OK|MB_ICONINFORMATION);
	}
}

void CFCView::OnOption() 
{
	CFCOptionDlg dlg;

	dlg.m_bAllowEmptyStatement	= m_bAllowEmptyStatement;
	dlg.m_bAutoChangeType		= m_bAutoChangeType;
	dlg.m_bDisableChangeType	= m_bDisableChangeType;
	dlg.m_bFileOutput			= m_bFileOutput;
	dlg.m_bGenerateBinary		= m_bGenerateBinary;
	dlg.m_bGenerateText			= m_bGenerateText;
	dlg.m_bIgnoreCircleNoend	= m_bIgnoreCircleNoend;
	dlg.m_bIncludeArray			= m_bIncludeArray;
	dlg.m_bIncludeMath			= m_bIncludeMath;
	dlg.m_bIncludeString		= m_bIncludeString;
	dlg.m_bJumpOptimize			= m_bJumpOptimize;
	dlg.m_bOutputInput			= m_bOutputInput;
	dlg.m_bOutRuntime			= m_bCalcRuntime;
	dlg.m_bReportError			= !m_bIgnoreError;
	dlg.m_nPrecision			= m_nPrecision;
	dlg.m_strMemoryApply.Format("%d",m_nMemoryApply);
	dlg.m_strOutputFile			= m_strOutputFile;

	if(dlg.DoModal()!=IDOK)
		return;

	m_bAllowEmptyStatement	= dlg.m_bAllowEmptyStatement;
	m_bAutoChangeType		= dlg.m_bAutoChangeType;
	m_bDisableChangeType	= dlg.m_bDisableChangeType;
	m_bFileOutput			= dlg.m_bFileOutput;
	m_bGenerateBinary		= dlg.m_bGenerateBinary;
	m_bGenerateText			= dlg.m_bGenerateText;
	m_bIgnoreCircleNoend	= dlg.m_bIgnoreCircleNoend;
	m_bIncludeArray			= dlg.m_bIncludeArray;
	m_bIncludeMath			= dlg.m_bIncludeMath;
	m_bIncludeString		= dlg.m_bIncludeString;
	m_bJumpOptimize			= dlg.m_bJumpOptimize;
	m_bOutputInput			= dlg.m_bOutputInput;
	m_bCalcRuntime			= dlg.m_bOutRuntime;
	m_bIgnoreError			= !dlg.m_bReportError;
	m_nMemoryApply			= atoi(dlg.m_strMemoryApply);
	m_nPrecision			= dlg.m_nPrecision;
	m_strOutputFile			= dlg.m_strOutputFile;

	m_bCompiled=FALSE;
}

void CFCView::OnCompileReportError() 
{
	m_bIgnoreError= !m_bIgnoreError;
}

void CFCView::OnUpdateCompileReportError(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(!m_bIgnoreError);
}

void CFCView::OnHelpGrammar() 
{
	CString strTempFile;
	char buffer[1024];
	::GetTempPath(1024,buffer);
	strTempFile.Format("%s//grammar.txt", buffer);

	SaveResToFile(strTempFile,IDR_TEXT_GRAMMAR, "TEXT");
	::ShellExecute(0,0,strTempFile,0,0,SW_SHOW);
}

void CFCView::OnHelpGlobal() 
{
	CString strTempFile;
	char buffer[1024];
	::GetTempPath(1024,buffer);
	strTempFile.Format("%s//global.txt", buffer);

	SaveResToFile(strTempFile,IDR_TEXT_GLOBAL, "TEXT");
	::ShellExecute(0,0,strTempFile,0,0,SW_SHOW);
}

void CFCView::OnExampleHelloworld() 
{
	OnExample("HelloWorld", IDR_FC_HELLOWORLD);
}

void CFCView::OnExamplePermutation() 
{
	OnExample("Permutation", IDR_FC_PERMUTATION);
}

void CFCView::OnExamplePrime() 
{
	OnExample("Prime", IDR_FC_PRIME);
}

void CFCView::OnExamplePi() 
{
	OnExample("PI", IDR_FC_PI);
}

void CFCView::OnExampleHanio() 
{
	OnExample("Hanio", IDR_FC_HANIO);
}

void CFCView::OnExampleFibonacci() 
{
	OnExample("Fibonacci", IDR_FC_FIBONACCI);
}
