
// PopstarDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Popstar.h"
#include "PopstarDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPopstarDlg 对话框

#define ORIGIN_X 30
#define ORIGIN_Y 30
#define NEW_BLOCK_X 30
#define NEW_BLOCK_Y 30
#define NEW_SB_SIZE 10
#define NEW_SB_COLORS 5

#define CONFIGUE_FILE_NAME _T("\\Popstar_cfg.ini")
#define SECTION_NAME _T("Popstar Configuration")
#define CUR_CHO_BRU _T("编辑模式 当前选择：#")

CPopstarDlg::CPopstarDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPopstarDlg::IDD, pParent)
    , m_sb(NEW_SB_SIZE, NEW_SB_COLORS)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_font.CreateFont(20, 10, 0, 0, 12, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("宋体"));

    // 索引从1开始
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


// CPopstarDlg 消息处理程序

BOOL CPopstarDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    ModifyStyle(WS_THICKFRAME, 0, SWP_FRAMECHANGED | SWP_DRAWFRAME);

    CString path_, path;
    // 读取配置文件
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPopstarDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
        // 画图
        CPaintDC dc(this);
        m_sb.Display(&dc, m_origin.cx, m_origin.cy, m_bEdit);

		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPopstarDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPopstarDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
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
    // TODO: 在此添加消息处理程序代码和/或调用默认值
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
    // TODO: 在此添加消息处理程序代码和/或调用默认值
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
            _T("局面文件(*.map)|*.map|所有文件(*.*)|*.*||"));
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
                AfxMessageBox(_T("打开文件失败！"));
            }
        }
    }
    if ((nChar == 'v' || nChar == 'V') && StarBoard::m_pThread == NULL)
    {
        CFileDialog dlg(FALSE, _T("*.map"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
            _T("局面文件(*.map)|*.map|所有文件(*.*)|*.*||"));
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
                AfxMessageBox(_T("保存文件失败！"));
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
        if (!StarBoard::m_HighestSels.IsEmpty()) // 线程已停止，没有安全问题
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
            _T("\n---======== 帮助========---")
            _T("\n空格键 -- 帮助菜单")
            _T("\n鼠标左键 -- 选中或消除")
            _T("\n鼠标右键 -- 编辑时清空所选")
            _T("\nH -- 显示 / 隐藏步骤")
            _T("\nC -- 启动计算线程")
            _T("\nS -- 停止计算线程")
            _T("\nR -- 随机生成局面")
            _T("\nE -- 编辑局面 (数字键 0 ~ 9 : 选择颜色 [0 => 背景色])")
            _T("\nF -- 用指定颜色填充 ( [#0 ==> 清空所有] )")
            _T("\nL -- 清除运算结果")
            _T("\nT -- 单步执行 ( 先选择后消除 )")
            _T("\nP -- 从文件打开 (*.map)")
            _T("\nV -- 保存至文件 (*.map)")
            _T("\n\n\n\t\t\t\tAuthor: bajdcc")
            ;
        AfxMessageBox(str);
    }
    CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CPopstarDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
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
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == 0x40)
    {
        CString str;
        int lst;
        ULONG runover;
        m_sb.Lists(lst, false);
        m_sb.CurrentRunovers(runover, false);
        str.AppendFormat(_T("队列数：%4d  结果数：%4d"), lst, runover);
        m_edit.SetWindowText(str);
    }
    CDialogEx::OnTimer(nIDEvent);
}


void CPopstarDlg::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
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
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CPoint origin = point;
    origin.Offset(-m_origin.cx, -m_origin.cy);
    if (m_bEdit)
    {
        m_sb.HitEditFromPoint(origin, 0);
        Invalidate();
    }
    CDialogEx::OnRButtonDown(nFlags, point);
}
