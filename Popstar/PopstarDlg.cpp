
// PopstarDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Popstar.h"
#include "PopstarDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPopstarDlg �Ի���

#define ORIGIN_X 30
#define ORIGIN_Y 30
#define NEW_BLOCK_X 30
#define NEW_BLOCK_Y 30
#define NEW_SB_SIZE 10
#define NEW_SB_COLORS 5

#define CONFIGUE_FILE_NAME _T("\\Popstar_cfg.ini")
#define SECTION_NAME _T("Popstar Configuration")
#define CUR_CHO_BRU _T("�༭ģʽ ��ǰѡ��#")

CPopstarDlg::CPopstarDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPopstarDlg::IDD, pParent)
    , m_sb(NEW_SB_SIZE, NEW_SB_COLORS)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_font.CreateFont(20, 10, 0, 0, 12, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("����"));

    // ������1��ʼ
    m_sb.SetColor(1, RGB(200, 0, 0));
    m_sb.SetColor(2, RGB(0, 200, 0));
    m_sb.SetColor(3, RGB(0, 200, 200));
    m_sb.SetColor(4, RGB(200, 200, 0));
    m_sb.SetColor(5, RGB(0, 0, 200));
    m_sb.SetBlock(NEW_BLOCK_X, NEW_BLOCK_Y);

    m_origin.SetSize(ORIGIN_X, ORIGIN_Y);

    m_iMaxColors = NEW_SB_COLORS;

    m_bEdit = false;
    m_iChooseColor = 0;
}

void CPopstarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPopstarDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_KEYDOWN()
    ON_WM_CLOSE()
    ON_WM_TIMER()
    ON_WM_CLOSE()
    ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// CPopstarDlg ��Ϣ�������

BOOL CPopstarDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
    ModifyStyle(WS_THICKFRAME, 0, SWP_FRAMECHANGED | SWP_DRAWFRAME);

    CString path_, path;
    // ��ȡ�����ļ�
    if (GetCurrentDirectory(MAX_PATH, path_.GetBuffer(MAX_PATH)))
    {
        path = path_.GetBuffer(MAX_PATH);
        path.Append(CONFIGUE_FILE_NAME);
        if (PathFileExists(path))
        {
            CSize sz;
            int it;
            m_origin.cx = (LONG) GetPrivateProfileInt(SECTION_NAME, _T("Origin X"), ORIGIN_X, path);
            m_origin.cy = (LONG) GetPrivateProfileInt(SECTION_NAME, _T("Origin Y"), ORIGIN_Y, path);
            sz.cx = (LONG) GetPrivateProfileInt(SECTION_NAME, _T("Block X"), NEW_BLOCK_X, path);
            sz.cy = (LONG) GetPrivateProfileInt(SECTION_NAME, _T("Block Y"), NEW_BLOCK_Y, path);
            m_sb.SetBlock(sz.cx, sz.cy);
            sz.cx = (LONG) GetPrivateProfileInt(SECTION_NAME, _T("Margin X"), SB_MARGIN_X, path);
            sz.cy = (LONG) GetPrivateProfileInt(SECTION_NAME, _T("Margin Y"), SB_MARGIN_Y, path);
            m_sb.SetMargin(sz.cx, sz.cy);
            it = GetPrivateProfileInt(SECTION_NAME, _T("Size"), NEW_SB_SIZE, path);
            if (it > 0 && it < 20) m_sb.SetSize(it);
            it = GetPrivateProfileInt(SECTION_NAME, _T("Cut Number"), SB_DEPTH_CHOICE, path);
            if (it > 0 && it < 20) StarBoard::m_depthC = it;
            it = GetPrivateProfileInt(SECTION_NAME, _T("Rest All"), SB_REST_ALL, path);
            if (it > 0) StarBoard::m_restA = it;
            it = GetPrivateProfileInt(SECTION_NAME, _T("Colors"), 0, path);
            if (it > 0)
            {
                m_iMaxColors = it;
                m_sb.SetColors(it);
                CString str;
                DWORD c;
                for (int i = 0; i < it; i++)
                {
                    str.Format(_T("Color#%d"), i + 1);
                    c = (DWORD) GetPrivateProfileInt(SECTION_NAME, str, RGB(128, 128, 128), path);
                    m_sb.SetColor(i + 1, c);
                }
            }
        }
    }

    RECT rect;
    GetClientRect(&rect);
    rect.top = rect.bottom - 30;
    rect.right = rect.left + 300;
    m_edit.Create(ES_READONLY, rect, this, 0);
    m_edit.SetFont(&m_font);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPopstarDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
        // ��ͼ
        CPaintDC dc(this);
        m_sb.Display(&dc, m_origin.cx, m_origin.cy, m_bEdit);

		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CPopstarDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPopstarDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    CPoint origin = point;
    origin.Offset(-m_origin.cx, -m_origin.cy);
    if (m_bEdit)
        m_sb.HitEditFromPoint(origin, m_iChooseColor);
    else
        m_sb.HitFromPoint(origin);
    Invalidate();
    CDialogEx::OnLButtonDown(nFlags, point);
}


void CPopstarDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    CPoint origin = point;
    origin.Offset(-m_origin.cx, -m_origin.cy);
    if (m_bEdit)
        m_sb.HitEditFromPoint(origin, m_iChooseColor);
    else
        m_sb.HitFromPoint(origin, true);
    Invalidate();
    CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CPopstarDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    if ((nChar >= '0' && nChar <= '9') && m_bEdit)
    {
        if (nChar <= ((UINT)m_iMaxColors + 0x30))
        {
            m_iChooseColor = nChar - 0x30;
            UpdateEdit();
        }
    }
    if ((nChar == 'r' || nChar == 'R') && !m_bEdit && StarBoard::m_pThread == NULL)
    {
        bool bT = true;
        m_sb.Terminate(bT);
        m_sb.Reset();
        m_sb.Random();
        Invalidate();
        m_sb.RunWorkingThread();
    }
    if ((nChar == 'c' || nChar == 'C') && !m_bEdit)
    {
        m_sb.RunWorkingThread();
    }
    if (nChar == 's' || nChar == 'S')
    {
        bool bT = true;
        m_sb.Terminate(bT);
    }
    if ((nChar == 'p' || nChar == 'P') && StarBoard::m_pThread == NULL)
    {
        CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
            _T("�����ļ�(*.map)|*.map|�����ļ�(*.*)|*.*||"));
        if (dlg.DoModal() == IDOK)
        {
            CFile f;
            if (f.Open(dlg.GetPathName(), CFile::modeRead))
            {
                CArchive ar(&f, CArchive::load);
                Serialize(ar);
                m_sb.Serialize(ar);
                Invalidate();
            }
            else
            {
                AfxMessageBox(_T("���ļ�ʧ�ܣ�"));
            }
        }
    }
    if ((nChar == 'v' || nChar == 'V') && StarBoard::m_pThread == NULL)
    {
        CFileDialog dlg(FALSE, _T("*.map"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
            _T("�����ļ�(*.map)|*.map|�����ļ�(*.*)|*.*||"));
        if (dlg.DoModal() == IDOK)
        {
            CFile f;
            if (f.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite))
            {
                CArchive ar(&f, CArchive::store);
                Serialize(ar);
                m_sb.Serialize(ar);
                Invalidate();
            }
            else
            {
                AfxMessageBox(_T("�����ļ�ʧ�ܣ�"));
            }
        }
    }
    if ((nChar == 'e' || nChar == 'E') && StarBoard::m_pThread == NULL)
    {
        if (m_bEdit)
        {
            m_edit.ShowWindow(SW_HIDE);
            m_edit.SetWindowText(NULL);
            m_bEdit = false;
        }
        else
        {
            m_edit.ShowWindow(SW_SHOW);
            UpdateEdit();
            m_bEdit = true;
        }
        Invalidate();
    }
    if ((nChar == 'f' || nChar == 'F') && m_bEdit)
    {
        m_sb.FillColor(m_iChooseColor);
        Invalidate();
    }
    if ((nChar == 't' || nChar == 'T') && StarBoard::m_pThread == NULL)
    {
        static bool bcl = false;
        if (!StarBoard::m_HighestSels.IsEmpty()) // �߳���ֹͣ��û�а�ȫ����
        {
            CPoint pt = StarBoard::m_HighestSels.GetHead();
            m_sb.Hit(pt.x, pt.y, bcl);
            if (bcl)
            {
                StarBoard::m_HighestSels.RemoveHead();
            }
            bcl = !bcl;
        }
        Invalidate();
    }
    if (nChar == 'h' || nChar == 'H')
    {
        bool hs;
        m_sb.HideStep(hs, false);
        hs = !hs;
        m_sb.HideStep(hs);
        Invalidate();
    }
    if (nChar == 'l' || nChar == 'L')
    {
        m_sb.ClearHighest();
        Sleep(100);
        Invalidate();
    }
    if (nChar == ' ')
    {
        CString str;
        str = 
            _T("\n---======== ����========---")
            _T("\n�ո�� -- �����˵�")
            _T("\n������ -- ѡ�л�����")
            _T("\n����Ҽ� -- �༭ʱ�����ѡ")
            _T("\nH -- ��ʾ / ���ز���")
            _T("\nC -- ���������߳�")
            _T("\nS -- ֹͣ�����߳�")
            _T("\nR -- ������ɾ���")
            _T("\nE -- �༭���� (���ּ� 0 ~ 9 : ѡ����ɫ [0 => ����ɫ])")
            _T("\nF -- ��ָ����ɫ��� ( [#0 ==> �������] )")
            _T("\nL -- ���������")
            _T("\nT -- ����ִ�� ( ��ѡ������� )")
            _T("\nP -- ���ļ��� (*.map)")
            _T("\nV -- �������ļ� (*.map)")
            _T("\n\n\n\t\t\t\tAuthor: bajdcc")
            ;
        AfxMessageBox(str);
    }
    CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CPopstarDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    if (pMsg->message == WM_KEYDOWN)
    {
        SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
    }
    else if (pMsg->message == WM_APP + 0x30)
    {
        TRACE(_T("Thread send invalidate.\n"));
        Invalidate();
    }
    else if (pMsg->message == WM_APP + 0x40)
    {
        TRACE(_T("Thread send hide lists.\n"));
        m_edit.ShowWindow(SW_HIDE);
        m_edit.SetWindowText(NULL);
    }
    else if (pMsg->message == WM_APP + 0x50)
    {
        TRACE(_T("Thread send show lists.\n"));
        m_edit.ShowWindow(SW_SHOW);
        m_edit.SetWindowText(NULL);
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}


void CPopstarDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    if (nIDEvent == 0x40)
    {
        CString str;
        int lst;
        ULONG runover;
        m_sb.Lists(lst, false);
        m_sb.CurrentRunovers(runover, false);
        str.AppendFormat(_T("��������%4d  �������%4d"), lst, runover);
        m_edit.SetWindowText(str);
    }
    CDialogEx::OnTimer(nIDEvent);
}


void CPopstarDlg::OnClose()
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    bool bT = true;
    m_sb.Terminate(bT);
    CDialogEx::OnClose();
}

void CPopstarDlg::UpdateEdit()
{
    CString str;
    str.Format(_T("%s%d"), CUR_CHO_BRU, m_iChooseColor);
    m_edit.SetWindowText(str);
}


void CPopstarDlg::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {	// storing code
        ar << m_origin << m_iMaxColors;
    }
    else
    {	// loading code
        ar >> m_origin >> m_iMaxColors;
    }
}


void CPopstarDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    CPoint origin = point;
    origin.Offset(-m_origin.cx, -m_origin.cy);
    if (m_bEdit)
    {
        m_sb.HitEditFromPoint(origin, 0);
        Invalidate();
    }
    CDialogEx::OnRButtonDown(nFlags, point);
}
