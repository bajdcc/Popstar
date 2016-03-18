
// PopstarDlg.h : ͷ�ļ�
//

#pragma once
#include "StarBoard.h"

// CPopstarDlg �Ի���
class CPopstarDlg : public CDialogEx
{
// ����
public:
	CPopstarDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_POPSTAR_DIALOG };

    CEdit m_edit;
    CSize m_origin;
    bool m_bEdit; // �༭ģʽ
    int  m_iChooseColor; // ѡ�񻭱�
    UINT  m_iMaxColors; // �����ɫ��

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
    StarBoard m_sb;
    CFont m_font;

    void UpdateEdit();

	// ���ɵ���Ϣӳ�亯��
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
