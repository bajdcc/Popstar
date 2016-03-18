
// PopstarDlg.h : 头文件
//

#pragma once
#include "StarBoard.h"

// CPopstarDlg 对话框
class CPopstarDlg : public CDialogEx
{
// 构造
public:
	CPopstarDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_POPSTAR_DIALOG };

    CEdit m_edit;
    CSize m_origin;
    bool m_bEdit; // 编辑模式
    int  m_iChooseColor; // 选择画笔
    UINT  m_iMaxColors; // 最大颜色数

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
    StarBoard m_sb;
    CFont m_font;

    void UpdateEdit();

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnClose();
    virtual void Serialize(CArchive& ar);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};
