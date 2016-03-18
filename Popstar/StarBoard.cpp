#include "StdAfx.h"
#include "StarBoard.h"

#define SHOW_MATRIX 0
#define SHOW_BOARD  0
#define SHOW_NUMBER 0
#define SHOW_SELECT 0
#define SHOW_HITTEST 0
#define SHOW_HIT 0

StarBoardBase::StarBoardBase( int size /*= SB_SIZE*/, int colors /*= SB_COLOR*/, bool bVerticalClear /*= true*/, bool bHorizontalClear /*= false*/ )
{
    m_iSize = size;
    m_iColors = colors + 1;
    m_score = 0;
    m_selCounts = 0;
    m_restBlocks = size * size;
    m_checkDead = false;
    m_bHorizontalClear = bHorizontalClear;
    m_bVerticalClear = bVerticalClear;
}

StarBoardBase::~StarBoardBase()
{

}

void StarBoardBase::Reset()
{
    m_checkDead = false;
    m_score = 0;
    m_restBlocks = m_iSize * m_iSize;
    m_arrBoard.RemoveAll();
    m_arrBoard.SetSize(m_iSize * m_iSize);
    ClearHighest();
}

void StarBoardBase::HitEdit( LONG x, LONG y, int colors )
{
    ASSERT(x < m_iSize && x >= 0);
    ASSERT(y < m_iSize && y >= 0);
    ASSERT(colors >= 0 && colors <= m_iColors);

#if SHOW_BOARD == 1
    TRACE(_T("Prev ------------------ Hit Edit begin: show board\n"));
    for (int i = 0; i < m_arrBoard.GetCount(); i++)
    {
        TRACE(_T("%d "), m_arrBoard[i]);
        if (i % m_iSize == m_iSize - 1) TRACE(_T("\n"));
    }
    TRACE(_T("Prev ------------------ Hit Edit end: show board\n"));
#endif // SHOW_BOARD == 1

#if SHOW_HIT
    // 单击某列某列，为核心操作函数
    TRACE(_T("Hit Edit (%d, %d)\n"), x, y);
#endif // _SHOW_HIT

    m_arrBoard[x * m_iSize + y] = colors;

#if SHOW_BOARD == 1
    TRACE(_T("Next ------------------ Hit Edit begin: show board\n"));
    for (int i = 0; i < m_arrBoard.GetCount(); i++)
    {
        TRACE(_T("%d "), m_arrBoard[i]);
        if (i % m_iSize == m_iSize - 1) TRACE(_T("\n"));
    }
    TRACE(_T("Next ------------------ Hit Edit end: show board\n"));
#endif // SHOW_BOARD == 1

}

void StarBoardBase::Hit( LONG x, LONG y, bool bClear )
{
    ASSERT(x < m_iSize && x >= 0);
    ASSERT(y < m_iSize && y >= 0);

#if SHOW_BOARD == 1
    TRACE(_T("Prev ------------------ Hit begin: show board\n"));
    for (int i = 0; i < m_arrBoard.GetCount(); i++)
    {
        TRACE(_T("%d "), m_arrBoard[i]);
        if (i % m_iSize == m_iSize - 1) TRACE(_T("\n"));
    }
    TRACE(_T("Prev ------------------ Hit end: show board\n"));
#endif // SHOW_BOARD == 1


#if SHOW_HIT
    // 单击某列某列，为核心操作函数
    TRACE(_T("Hit (%d, %d)\n"), x, y);
#endif // _SHOW_HIT

    if (!bClear)
    {
        BuildRefBoard();
        if (m_arrRef[x * m_iSize + y] == 0) // 若为孤立或消隐，则无效
        {
            return;
        }

        CArray<int> board;
        board.SetSize(m_iSize * m_iSize);
        m_liSels.RemoveAll();
        FactSel(board, m_liSels, x, y);
        m_selCounts = m_liSels.GetSize();
    }
    else
    {
        BlockClear();
        HVClear();
        m_liSels.RemoveAll();
        m_restBlocks -= m_selCounts;
        m_score += CalcSelScore(m_selCounts);
        m_selCounts = 0;
        BuildRefBoard();
        CheckDead();
    }

#if SHOW_BOARD == 1
    TRACE(_T("Next ------------------ Hit begin: show board\n"));
    for (int i = 0; i < m_arrBoard.GetCount(); i++)
    {
        TRACE(_T("%d "), m_arrBoard[i]);
        if (i % m_iSize == m_iSize - 1) TRACE(_T("\n"));
    }
    TRACE(_T("Next ------------------ Hit end: show board\n"));
#endif // SHOW_BOARD == 1

}

void StarBoardBase::HVClear()
{
    CList<int> li;
    int cols = m_bHorizontalClear ? m_iSize - 1 : 0;
    int add_ = m_bHorizontalClear ? -1 : 1;
    for ( int y = cols; y < m_iSize && y >= 0; y += add_ )
    {
        li.RemoveAll();
        for (int x = 0; x < m_iSize; x++)
        {
            int type = m_arrBoard[x * m_iSize + y];
            if (type != 0)
            {
                if (m_bVerticalClear)
                    li.AddHead(type);
                else
                    li.AddTail(type);  
            }
            m_arrBoard[x * m_iSize + y] = 0;
        }
        if (li.IsEmpty())
        {
            continue;
        }
        POSITION pos = li.GetHeadPosition();
        int index = 0;
        while (pos != NULL)
        {
            ASSERT(index < m_iSize);
            int type = li.GetNext(pos);
            if (m_bVerticalClear)
                m_arrBoard[(m_iSize - index - 1) * m_iSize + cols] = type;
            else
                m_arrBoard[index * m_iSize + cols] = type;
            index++;
        }
        cols += add_;
    }
}

void StarBoardBase::BlockClear()
{
    CPoint pt;
    POSITION pos = m_liSels.GetHeadPosition();
    while (pos != NULL)
    {
        pt = m_liSels.GetNext(pos);
        m_arrBoard[pt.x * m_iSize + pt.y] = 0;
    }
}

void StarBoardBase::CheckDead()
{
    bool bDead = true;
    for (int i = 0; i < m_iSize * m_iSize; i++)
    {
        if (m_arrRef[i] != 0)
        {
            bDead = false;
            break;
        }
    }

    if (bDead)
    {
        m_checkDead = true;
        m_score += CalcFinalScore(m_restBlocks);
    }
}

void StarBoardBase::Random()
{
    srand((UINT)time(NULL));
    int colors = m_iColors - 1;
    for (int i = 0; i < m_iSize * m_iSize; i++)
    {
        m_arrBoard[i] = rand() % colors + 1;
    }
}

void StarBoardBase::ClearRefBoard()
{
    m_arrRef.RemoveAll();
    m_arrRef.SetSize(m_iSize * m_iSize);
}

void StarBoardBase::BuildRefBoard()
{
    ASSERT(m_arrRef.GetSize() == m_iSize * m_iSize);
    ClearRefBoard();
    // x为行，y为列

#if SHOW_MATRIX == 1
    TRACE(_T("------------------------ Ref board: begin\n"));
#endif // SHOW_MATRIX == 1

    for (int x = 0; x < m_iSize; x++)
    {
        for (int y = 0; y < m_iSize; y++)
        {
            int index = x * m_iSize + y;
#if SHOW_MATRIX == 1
            TRACE(_T("%d "), m_arrRef[index]);
#endif // SHOW_MATRIX == 1
            int type = m_arrBoard[index];
            if (type == 0) // 若为空则跳过
                continue;
            if (x > 0)
            {
                if (type == m_arrBoard[(x - 1) * m_iSize + y])
                {
                    m_arrRef[index]++;
                }
            }
            if (y > 0)
            {
                if (type == m_arrBoard[x * m_iSize + y - 1])
                {
                    m_arrRef[index]++;
                }
            }
            if (x < m_iSize - 1)
            {
                if (type == m_arrBoard[(x + 1) * m_iSize + y])
                {
                    m_arrRef[index]++;
                }
            }
            if (y < m_iSize - 1)
            {
                if (type == m_arrBoard[x * m_iSize + y + 1])
                {
                    m_arrRef[index]++;
                }
            }
        }
#if SHOW_MATRIX == 1
        TRACE(_T("\n"));
#endif // SHOW_MATRIX == 1
    }

#if SHOW_MATRIX == 1
    TRACE(_T("------------------------ Ref board: end\n"));
#endif // SHOW_MATRIX == 1
}

void StarBoardBase::FactSel( CArray<int>& arrBoard, CList<CPoint>& liPoint, LONG x, LONG y )
{
    arrBoard[x * m_iSize + y]++;
    liPoint.AddTail(CPoint(x, y));
    int type = m_arrBoard[x * m_iSize + y];
    if (x > 0)
    {
        if (type == m_arrBoard[(x - 1) * m_iSize + y] && arrBoard[(x - 1) * m_iSize + y] == 0)
        {
            FactSel(arrBoard, liPoint, x - 1, y);
        }
    }
    if (y > 0)
    {
        if (type == m_arrBoard[x * m_iSize + y - 1] && arrBoard[x * m_iSize + y - 1] == 0)
        {
            FactSel(arrBoard, liPoint, x, y - 1);
        }
    }
    if (x < m_iSize - 1)
    {
        if (type == m_arrBoard[(x + 1) * m_iSize + y] && arrBoard[(x + 1) * m_iSize + y] == 0)
        {
            FactSel(arrBoard, liPoint, x + 1, y);
        }
    }
    if (y < m_iSize - 1)
    {
        if (type == m_arrBoard[x * m_iSize + y + 1] && arrBoard[x * m_iSize + y + 1] == 0)
        {
            FactSel(arrBoard, liPoint, x, y + 1);
        }
    }
}

void StarBoardBase::FactSelV2( CArray<int>& arrBoard, LONG x, LONG y, int& length )
{
    arrBoard[x * m_iSize + y] = 0;
    length++;
    int type = m_arrBoard[x * m_iSize + y];
    if (x > 0)
    {
        if (type == m_arrBoard[(x - 1) * m_iSize + y] && arrBoard[(x - 1) * m_iSize + y] != 0)
        {
            FactSelV2(arrBoard, x - 1, y, length);
        }
    }
    if (y > 0)
    {
        if (type == m_arrBoard[x * m_iSize + y - 1] && arrBoard[x * m_iSize + y - 1] != 0)
        {
            FactSelV2(arrBoard, x, y - 1, length);
        }
    }
    if (x < m_iSize - 1)
    {
        if (type == m_arrBoard[(x + 1) * m_iSize + y] && arrBoard[(x + 1) * m_iSize + y] != 0)
        {
            FactSelV2(arrBoard, x + 1, y, length);
        }
    }
    if (y < m_iSize - 1)
    {
        if (type == m_arrBoard[x * m_iSize + y + 1] && arrBoard[x * m_iSize + y + 1] != 0)
        {
            FactSelV2(arrBoard, x, y + 1, length);
        }
    }
}

int StarBoardBase::CalcSelScore( int sels )
{
    return 5 * sels * sels;
}

int StarBoardBase::CalcFinalScore( int rest )
{
    return ( rest >= 0 && rest < 10 ) ? 2000 - 20 * rest * rest : 0;
}

bool StarBoardBase::IsDead() const
{
    return m_checkDead;
}

void StarBoardBase::Copy( const StarBoardBase& base )
{
    m_cs.Lock(INFINITE);
    m_iSize = base.m_iSize;
    m_iColors = base.m_iColors;
    m_score = base.m_score;
    m_selCounts = base.m_selCounts;
    m_restBlocks = base.m_restBlocks;
    m_checkDead = base.m_checkDead;
    m_bHorizontalClear = base.m_bHorizontalClear;
    m_bVerticalClear = base.m_bVerticalClear;
    m_liSels.RemoveAll();
    m_liSels.AddTail((CList<CPoint>*)&base.m_liSels);
    m_arrRef.Copy(base.m_arrRef);
    m_arrBoard.Copy(base.m_arrBoard);
    m_cs.Unlock();
}

bool StarBoardBase::SplitChains( CList<SelChain>& liSC )
{
    BuildRefBoard();
    CArray<int> refBoard;
    refBoard.Copy(m_arrRef);

    SelChain sc;

    for (int x = 0; x < m_iSize; x++)
    {
        for (int y = 0; y < m_iSize; y++)
        {
            if (refBoard[x * m_iSize + y] != 0)
            {
                sc.length = 0;
                FactSelV2(refBoard, x, y, sc.length);
                sc.pt.SetPoint(x, y);
                liSC.AddTail(sc);
            }
        }
    }

    if (liSC.IsEmpty())
    {
        m_checkDead = true;
        return false;
    }
    else
    {
        return true;
    }
}

void StarBoardBase::HighestScore( CList<int>& score, bool bWrite /*= true*/ )
{
    m_cs.Lock(INFINITE);
    if (bWrite)
    {
        m_liHighestScore.RemoveAll();
        m_liHighestScore.AddHead(&score);
    }
    else
    {
        score.RemoveAll();
        score.AddHead(&m_liHighestScore);
    }
    m_cs.Unlock();
}

void StarBoardBase::HighestSels( CList<CPoint>& sels, bool bWrite /*= true*/ )
{
    m_cs.Lock(INFINITE);
    if (bWrite)
    {
        m_HighestSels.RemoveAll();
        m_HighestSels.AddHead(&sels);
    }
    else
    {
        sels.RemoveAll();
        sels.AddHead(&m_HighestSels);
    }
    m_cs.Unlock();
}

void StarBoardBase::Lists( int& lists, bool bWrite /*= true*/ )
{
    m_cs.Lock(INFINITE);
    if (bWrite)
        m_iList = lists;
    else
        lists = m_iList;
    m_cs.Unlock();
}

void StarBoardBase::HideStep( bool& bHide, bool bWrite /*= true*/ )
{
    m_cs.Lock(INFINITE);
    if (bWrite)
        m_bHideStep = bHide;
    else
        bHide = m_bHideStep;
    m_cs.Unlock();
}

void StarBoardBase::CurrentRunovers( ULONG& cur, bool bWrite /*= true*/ )
{
    m_cs.Lock(INFINITE);
    if (bWrite)
        m_Runovers = cur;
    else
        cur = m_Runovers;
    m_cs.Unlock();
}

int StarBoardBase::GetScore() const
{
    return m_score;
}

int StarBoardBase::GetRest() const
{
    return m_restBlocks;
}

void StarBoardBase::ClearHighest()
{
    CList<int> ip;
    ip.AddHead(0);
    CList<CPoint> p;
    HighestScore(ip);
    HighestSels(p);
}

void StarBoardBase::ColorAttr( CArray<int>& arrInt )
{
    arrInt.SetSize(m_iColors);
    for (int i = 0; i < m_arrBoard.GetCount(); i++)
    {
        arrInt[m_arrBoard[i]]++;
    }
}

ULONG StarBoardBase::m_Runovers;
bool StarBoardBase::m_bHideStep = true;
CList<int> StarBoardBase::m_liHighestScore;
int StarBoardBase::m_iList;
CWinThread* StarBoard::m_pThread;
CList<CPoint> StarBoardBase::m_HighestSels;
CCriticalSection StarBoardBase::m_cs;
bool StarBoard::m_bTerminate;
int StarBoard::m_restA;
int StarBoard::m_depthC;
bool StarBoard::m_bAccCalc;

//////////////////////////////////////////////////////////////////////////

StarBoard::StarBoard( int size /*= SB_SIZE*/, int colors /*= SB_COLOR*/, bool bVerticalClear /*= true*/, bool bHorizontalClear /*= false */ ):
StarBoardBase(size, colors, bVerticalClear, bHorizontalClear)
{
    ASSERT(size > 0 && colors > 0);
    m_arrBoard.SetSize(size * size); // 初始化清零
    m_arrRef.SetSize(size * size); // 初始化清零
    m_arrColors.SetSize(colors + 1); // 初始化清零
    m_arrClr.SetSize(colors + 1);
    SetColor(0, RGB(0, 0, 0));
    m_szMargin.SetSize(SB_MARGIN_X, SB_MARGIN_Y);
    m_szBlock.SetSize(SB_BLOCK_X, SB_BLOCK_Y);
    m_clrBorder.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    m_font.CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("微软雅黑"));
    m_font_cnt.CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("宋体"));
    m_depthC = SB_DEPTH_CHOICE;
    m_restA = SB_REST_ALL;

    m_liHighestScore.AddHead(0);
}

StarBoard::~StarBoard()
{
    ClearBrush();
}

void StarBoard::SetMargin( LONG x, LONG y )
{
    ASSERT(x > 1 && y > 1);
    m_szMargin.SetSize(x, y);
}

void StarBoard::FillColor( int color )
{
    ASSERT(color >= 0 && color < m_iColors);
    if (color > 0)
    {
        for (int i = 0; i < m_arrBoard.GetCount(); i++)
        {
            if (m_arrBoard[i] == 0)
            {
                m_arrBoard[i] = color;
            }
        }
    }
    else
    {
        m_arrBoard.RemoveAll();
        m_arrBoard.SetSize(m_iSize * m_iSize);
    }
}

void StarBoard::HitEditFromPoint( CPoint pt, int colors )
{
    LONG y = pt.x / (m_szBlock.cx + m_szMargin.cx);
    LONG x = pt.y / (m_szBlock.cy + m_szMargin.cy);

    if (!(x < m_iSize && x >= 0 && y < m_iSize && y >= 0)) // 超出区域，无效
    {
        return;
    }

    TRACE("Hit Edit From Point (%d, %d)\n", x, y);

    if (pt.x > y * (m_szBlock.cx + m_szMargin.cx) + m_szBlock.cx && 
        pt.y > x * (m_szBlock.cy + m_szMargin.cy) + m_szBlock.cy) // 单击了Margin区域，无效
    {
        m_liSels.RemoveAll();
        return;
    }

    HitEdit(x, y, colors);
}

void StarBoard::HitFromPoint( CPoint pt, bool bClear /*= false*/  )
{
    LONG y = pt.x / (m_szBlock.cx + m_szMargin.cx);
    LONG x = pt.y / (m_szBlock.cy + m_szMargin.cy);

    if (!(x < m_iSize && x >= 0 && y < m_iSize && y >= 0)) // 超出区域，无效
    {
        m_liSels.RemoveAll();
        return;
    }

    TRACE("Hit From Point (%d, %d)\n", x, y);

    if (pt.x > y * (m_szBlock.cx + m_szMargin.cx) + m_szBlock.cx && 
        pt.y > x * (m_szBlock.cy + m_szMargin.cy) + m_szBlock.cy) // 单击了Margin区域，无效
    {
        m_liSels.RemoveAll();
        return;
    }

    bool bClear_ = bClear;

    CPoint pt_;
    CPoint local(x, y);
    POSITION pos = m_liSels.GetHeadPosition();
    while (pos != NULL)
    {
        pt_ = m_liSels.GetNext(pos);
        if (pt_ == local)
        {
            bClear_ = true;
            break;
        }
    }

    Hit(x, y, bClear_);
}

void StarBoard::SetBlock( LONG x, LONG y )
{
    ASSERT(x > 0 && y > 0);
    m_szBlock.SetSize(x, y);
}

void StarBoard::Display( CDC* pDC, LONG x, LONG y, bool bShowEdit /*= false*/ )
{
    ASSERT_VALID(pDC);

    CRect rect;

    rect.SetRectEmpty();
#if SHOW_NUMBER == 1
    CString num(_T("0123456789"));
    pDC->SetBkMode(TRANSPARENT);
#endif // SHOW_NUMBER == 1

    // 画方块主体
    for (int i = 0; i < m_iSize; i++)
    {
        for (int j = 0; j < m_iSize; j++)
        {
            int index = m_arrBoard[i * m_iSize + j];
            CBrush* pBrush = m_arrColors[index];
            ASSERT_VALID(pBrush);
            pDC->SelectObject(pBrush);
            rect.top = y + i * (m_szBlock.cy + m_szMargin.cy);
            rect.left = x + j * (m_szBlock.cx + m_szMargin.cx);
            rect.bottom = rect.top + m_szBlock.cy;
            rect.right = rect.left + m_szBlock.cx;
            pDC->FillRect(&rect, pBrush);
#if SHOW_NUMBER == 1
            pDC->TextOut(rect.left, rect.top, num.Mid(index, 1));
#endif // SHOW_NUMBER == 1
        }
    }

    // 画方块阴影
    if (!m_liSels.IsEmpty()) // 查找不到
    {
        rect.SetRectEmpty();
        pDC->SelectObject(m_clrBorder);
        CPoint pt;
        POSITION pos = m_liSels.GetHeadPosition();
#if SHOW_SELECT == 1
        TRACE("Select:\n", pt.x, pt.y);
#endif // SHOW_SELECT == 1
        while (pos != NULL)
        {
            pt = m_liSels.GetNext(pos);
            rect.top = y + pt.x * (m_szBlock.cy + m_szMargin.cy) - 1;
            rect.left = x + pt.y * (m_szBlock.cx + m_szMargin.cx) - 1;
            rect.bottom = rect.top + m_szBlock.cy + 1;
            rect.right = rect.left + m_szBlock.cx + 1;
            pDC->DrawEdge(rect, EDGE_RAISED, BF_RECT);
#if SHOW_SELECT == 1
            TRACE("(%d, %d)\n", pt.x, pt.y);
#endif // SHOW_SELECT == 1
        }
    }

    // 画方块周围的计数
    {
        pDC->SetTextColor(RGB(56, 70, 87));
        pDC->SetBkMode(TRANSPARENT);
        pDC->SelectObject(m_font_cnt);
        CString str;
        for (int i = 0; i < m_iSize; i++)
        {
            RECT rect;
            rect.top = y + i * (m_szBlock.cy + m_szMargin.cy);
            rect.left = x - m_szBlock.cx;
            rect.bottom = rect.top + m_szBlock.cy;
            rect.right = x;
            str.Format(_T("%d"), i+1);
            pDC->DrawText(str, &rect, DT_CENTER | DT_VCENTER);
            rect.left = x + m_iSize * (m_szBlock.cx + m_szMargin.cx);
            rect.right = rect.left + m_szBlock.cx;
            pDC->DrawText(str, &rect, DT_CENTER | DT_VCENTER);
            rect.top = y - m_szBlock.cy;
            rect.left = x + i * (m_szBlock.cx + m_szMargin.cx);
            rect.bottom = rect.top + m_szBlock.cy;
            rect.right = rect.left + m_szBlock.cx;
            pDC->DrawText(str, &rect, DT_CENTER | DT_VCENTER);
            rect.top = y + m_iSize * (m_szBlock.cy + m_szMargin.cy);
            rect.bottom = rect.top + m_szBlock.cy;
            pDC->DrawText(str, &rect, DT_CENTER | DT_VCENTER);
        }
    }

    // 编辑
    if (bShowEdit)
    {
        CArray<int> stat;
        ColorAttr(stat);
        CString str;
        CRect rt;
        rt.top = y + (m_iSize + 1) * (m_szBlock.cy + m_szMargin.cy);
        rt.left = x;
        rt.bottom = rt.top + m_szBlock.cy;
        rt.right = x + m_szBlock.cx;
        CRect rt2(rt);
        pDC->SetBkMode(TRANSPARENT);
        pDC->SelectObject(m_font_cnt);
        for (int i = 0; i < m_arrColors.GetCount(); i++)
        {
            str.Format(_T("%d"), i);
            pDC->FillRect(&rt, m_arrColors[i]);
            pDC->DrawText(str, &rt, DT_CENTER | DT_VCENTER);
            rt2 = rt;
            rt2.top += m_szBlock.cy;
            rt2.bottom += m_szBlock.cy;
            str.Format(_T("%d"), stat[i]);
            pDC->DrawText(str, &rt2, DT_CENTER | DT_VCENTER);
            rt.left += m_szBlock.cx;
            rt.right += m_szBlock.cx;
        }
    }

    // 文字
    {
        CRect rt;
        AfxGetMainWnd()->GetClientRect(&rt);

        CList<CPoint> highest_sels;
        CList<int> highest_score;
        bool bHide = true;
        HideStep(bHide, false);
        HighestScore(highest_score, false);
        HighestSels(highest_sels, false);
        int hs = 0;
        if (!highest_score.IsEmpty())
        {
            hs = highest_score.GetHead();
        }
        
        CString bestSels;
        {
            POSITION pos = highest_sels.GetHeadPosition();
            while (pos)
            {
                CPoint& pt = highest_sels.GetNext(pos);
                bestSels.AppendFormat(_T("(%d, %d), "), pt.x + 1, pt.y + 1);
            }
        }

        CString optStr;
        optStr.Append(_T("按空格键打开帮助\r\n\r\n"));
        optStr.AppendFormat(_T("选中方块数目： %d\r\n"), m_selCounts);
        optStr.AppendFormat(_T("选中方块分数： %d\r\n"), CalcSelScore(m_selCounts));
        optStr.AppendFormat(_T("\r\n剩余： %d\r\n"), m_restBlocks);
        if (m_checkDead)
        {
            optStr.AppendFormat(_T("奖励分： %d\r\n"), CalcFinalScore(m_restBlocks));
        }
        optStr.AppendFormat(_T("当前分数： %d\r\n"), m_score);
        optStr.AppendFormat(_T("\r\n计算最高分： %d\r\n"), hs);
        if (!bHide)
        {
            optStr.AppendFormat(_T("最佳步骤：%s\r\n"), bestSels);
        }
        if (m_checkDead)
        {
            optStr.Append(_T("\r\n游戏结束！按'R'重新开始。\r\n"));
        }

        CRect rtText = rt;
        rtText.top = y + 20;
        rtText.left = x + m_iSize * (m_szBlock.cx + m_szMargin.cx) + 50;
        rtText.bottom = y + m_iSize * (m_szBlock.cy + m_szMargin.cy);
        if (rtText.bottom < 600) rtText.bottom = 600;
        rtText.right -= 20;

        pDC->SetTextColor(RGB(100, 78, 200));
        pDC->SetBkMode(TRANSPARENT);
        pDC->SelectObject(m_font);
        pDC->DrawText(optStr, &rtText, DT_LEFT | DT_WORDBREAK | DT_EDITCONTROL);

        optStr = _T("作者：bajdcc ");
        CRect rtBottom = rt;
        rtBottom.top = rtBottom.bottom - 30;
        pDC->DrawText(optStr, &rtBottom, DT_RIGHT);
    }
}

void StarBoard::SetColor( int index, COLORREF color )
{
    ASSERT(index >= 0 && index < m_iColors);
    if (m_arrColors[index] != NULL)
    {
        delete m_arrColors[index];
    }
    m_arrColors[index] = new CBrush(color);
    m_arrClr[index] = color;
}

void StarBoard::SetBorderColor( COLORREF color )
{
    m_clrBorder.DeleteObject();
    m_clrBorder.CreatePen(PS_SOLID, 1, color);
}

void StarBoard::RunWorkingThread()
{
    if (!m_pThread)
    {
        bool bT = false;
        Terminate(bT);
        m_pThread = AfxBeginThread(DaemonProcessor, this);
    }
}

void StarBoard::Serialize( CArchive& ar )
{
    if (ar.IsStoring())
    {
        ar << m_iSize << m_iColors << m_bVerticalClear << m_bHorizontalClear << m_checkDead << m_selCounts
            << m_score << m_restBlocks << m_szMargin << m_szBlock << m_liHighestScore.GetHead() << m_HighestSels.GetCount();
        if (!m_HighestSels.IsEmpty())
        {
            POSITION pos = m_HighestSels.GetHeadPosition();
            while (pos)
            {
                ar << m_HighestSels.GetNext(pos);
            }
        }
        for (int i = 0; i < m_arrClr.GetCount(); i++)
        {
            ar << m_arrClr[i];
        }
        for (int i = 0; i < m_arrBoard.GetCount(); i++)
        {
            ar << m_arrBoard[i];
        }
    }
    else
    {
        ar >> m_iSize >> m_iColors >> m_bVerticalClear >> m_bHorizontalClear >> m_checkDead >> m_selCounts
            >> m_score >> m_restBlocks >> m_szMargin >> m_szBlock;
        m_liHighestScore.RemoveAll();
        m_HighestSels.RemoveAll();
        m_liHighestScore.AddHead(0);
        ar >> m_liHighestScore.GetHead();
        int pt_cnt = 0;
        ar >> pt_cnt;
        if (pt_cnt > 0)
        {
            for (int i = 0; i < pt_cnt; i++)
            {
                CPoint pt;
                ar >> pt;
                m_HighestSels.AddTail(pt);
            }
        }
        ClearBrush();
        m_arrClr.SetSize(m_iColors);
        m_arrColors.SetSize(m_iColors);
        for (int i = 0; i < m_arrClr.GetCount(); i++)
        {
            ar >> m_arrClr[i];
            SetColor(i, m_arrClr[i]);
        }
        m_arrBoard.SetSize(m_iSize * m_iSize);
        for (int i = 0; i < m_arrBoard.GetCount(); i++)
        {
            ar >> m_arrBoard[i];
        }
    }
}

void StarBoard::ToggleSpeed()
{
    m_bAccCalc = !m_bAccCalc;
}

void StarBoard::Terminate( bool& bTerminate, bool bWrite /*= true*/ )
{
    m_cs.Lock(INFINITE);
    if (bWrite)
        m_bTerminate = bTerminate;
    else
        bTerminate = m_bTerminate;
    m_cs.Unlock();
}

void StarBoard::SetColors( int colors, COLORREF color )
{
    ASSERT(colors > 0);
    ClearBrush();
    m_arrColors.RemoveAll();
    m_iColors = colors + 1;
    m_arrColors.SetSize(m_iColors);
    m_arrClr.SetSize(m_iColors);
    SetColor(0, color);
}

void StarBoard::SetSize( int size )
{
    ASSERT(size > 0);
    m_arrBoard.SetSize(size * size); // 初始化清零
    m_arrRef.SetSize(size * size); // 初始化清零
    m_iSize = size;
    m_restBlocks = size * size;
}

void StarBoard::ClearBrush()
{
    for (int i = 0; i < m_iColors; i++)
    {
        if (m_arrColors[i])
        {
            delete m_arrColors[i];
            m_arrColors[i] = NULL;
        }
    }
    m_arrColors.RemoveAll();
    m_arrClr.RemoveAll();
}

UINT StarBoard::DaemonProcessor( LPVOID pVoid )
{
    StarBoard* pBoard = static_cast<StarBoard*>(pVoid);
    ASSERT(pBoard);

    StarBoardBase newBoard;
    newBoard.Copy(*pBoard); // 考虑线程安全，仅此而已，所以 Copy 应设临界区方法

    // 清空当前总量
    m_Runovers = 0UL;

    ULONG runovers = 0UL;

    // 设置定时器以更新队列数
    AfxGetMainWnd()->SetTimer(0x40, 300, NULL);
    AfxGetMainWnd()->PostMessage(WM_APP + 0x50, 0, 0);

    CList<BoardSet> liBS; // 装有各种局面的链表
    int choice = m_depthC; // 一次只选最优的数目

    // 下面开始计算
    CList<SelChain> liSC; // 链头及其长度
    CList<SelChain> liSC_sav; // 链头及其长度（供插入排序使用）

    SelChain sc;
    SelChain sc_sav;
    BoardSet bs;

    // 初始化最佳
    CList<int> highest_score; // 保存最高分
    CList<CPoint> highest_sels; // 保存最佳步骤

    // 读取上次结果
    pBoard->HighestScore(highest_score, false);
    pBoard->HighestSels(highest_sels, false);

    // 初始化 BoardSet，不必担心释放，因为已加入到队列
    bs.board = new StarBoardBase;
    bs.board->Copy(newBoard);
    bs.points = new CList<CPoint>;

    // 存储队列数
    int lists = 0;

    // 是否终止
    bool bTerminate = false;

    liBS.AddHead(bs);

    srand((UINT)time(NULL));

    while (true)
    {
        if (!m_bAccCalc)
            ::Sleep(100);

        pBoard->Terminate(bTerminate, false);

        POSITION pos_bs = liBS.GetHeadPosition();

        if (bTerminate)
        {
            // 清理现场
            while (pos_bs)
            {
                bs = liBS.GetNext(pos_bs);
                delete bs.board;
                delete bs.points;
            }
            break;
        }

        if (pos_bs)
        {
            bs = liBS.GetNext(pos_bs); // 类似堆栈的使用

            liSC.RemoveAll();
            liSC_sav.RemoveAll();
            if (bs.board->SplitChains(liSC)) // 线程安全？是的，独占使用
            {
                // SplitChains 完成的工作：
                // 分解链 => 各链头 + 长度
                // 下面要做的工作是：
                // 根据长度（连消个数）对链表进行排序（插入排序）

                POSITION pos_sc = liSC.GetHeadPosition();
                sc = liSC.GetNext(pos_sc);
                liSC_sav.AddHead(sc); // 确认 liSC_sav 中有初始比较项

                // 从小到大排序
                while (pos_sc)
                {
                    sc = liSC.GetNext(pos_sc);
                    POSITION pos_sc_sv = liSC_sav.GetHeadPosition();
                    while (pos_sc_sv)
                    {
                        sc_sav = liSC_sav.GetNext(pos_sc_sv);

                        if (!pos_sc_sv)
                        {
                            if (sc.length < sc_sav.length)
                                liSC_sav.InsertBefore(pos_sc_sv, sc);
                            else if (sc.length > sc_sav.length)
                                liSC_sav.InsertAfter(pos_sc_sv, sc);
                            else
                            {
                                // 添加随机元素
                                if (rand() % 2 == 0)
                                    liSC_sav.InsertBefore(pos_sc_sv, sc);
                                else
                                    liSC_sav.InsertAfter(pos_sc_sv, sc);
                            }
                            break;
                        }

                        if (sc.length > sc_sav.length)
                        {
                            liSC_sav.InsertBefore(pos_sc_sv, sc);
                            break;
                        }
                    }
                }

                BoardSet bs_new;

                POSITION pos_cur = liSC_sav.GetTailPosition();

                int index = 0;
                while (pos_cur)
                {
                    if (m_restA <= bs.board->GetRest())
                    {
                        if (index >= choice)
                        {
                            break;
                        }
                    }

                    sc_sav = liSC_sav.GetPrev(pos_cur);

                    bs_new.board = new StarBoardBase;
                    bs_new.board->Copy(*bs.board);      // 复制局面
                    bs_new.points = new CList<CPoint>;
                    bs_new.points->AddHead(bs.points);  // 复制步骤
                    bs_new.points->AddTail(sc_sav.pt);  // 添加步骤

#if SHOW_HITTEST == 1
                    TRACE(_T("\n>>>>>>> Thread: HitTest begin\n"));
#endif // SHOW_HITTEST
                    bs_new.board->Hit(sc_sav.pt.x, sc_sav.pt.y, false); // 模拟选中
                    bs_new.board->Hit(sc_sav.pt.x, sc_sav.pt.y, true); // 模拟消除
#if SHOW_HITTEST == 1
                    TRACE(_T("      score: %d, sels: %d\n"), bs_new.board->GetScore(), bs_new.points->GetCount());
                    TRACE(_T("<<<<<<< Thread: HitTest end\n"));
#endif // SHOW_HITTEST
                    
                    if (bs_new.board->IsDead())
                    {
                        // 总量加一
                        runovers++;
                        pBoard->CurrentRunovers(runovers);

                        // 如果已死，将把当前方法与局部最优方法作比较，保存更优者
                        int score_ = bs_new.board->GetScore();
                        
                        int hs = highest_score.GetHead();

                        if (hs < score_)
                        {
                            highest_score.GetHead() = score_;
                            highest_sels.RemoveAll();
                            highest_sels.AddHead(bs_new.points);
                            pBoard->HighestScore(highest_score);
                            pBoard->HighestSels(highest_sels);
                            AfxGetMainWnd()->PostMessage(WM_APP + 0x30, 0, 0);
                        }

                        delete bs_new.board;
                        delete bs_new.points;
                    }
                    else                        
                    {
                        // 否则存放至 liBS，继续遍历
                        // 若 AddHead 则快速显示结果
                        POSITION insrt = liBS.FindIndex(index);
                        ASSERT(insrt);
                        liBS.InsertAfter(insrt, bs_new);
                    }

                    index++;
                }
            }

            delete bs.board;
            delete bs.points;
            liBS.RemoveHead();

            lists = liBS.GetCount();
            pBoard->Lists(lists);
        }
        else
        {
            // 主局面链表已清空，标志着本次遍历搜索已结束
            // 结果保存在 liBS_suc 中
            break; // 跳出循环
        }
    }

    runovers = 0UL;
    pBoard->HighestScore(highest_score);
    pBoard->HighestSels(highest_sels);
    pBoard->Lists(lists);
    pBoard->CurrentRunovers(runovers);

    AfxGetMainWnd()->PostMessage(WM_APP + 0x30, 0, 0);

    AfxGetMainWnd()->KillTimer(0x40);
    AfxGetMainWnd()->PostMessage(WM_APP + 0x40, 0, 0);

    m_pThread = NULL;

    return 0;
}
