#pragma once
#include "afx.h"

#define SB_SIZE 6
#define SB_COLOR 5
#define SB_BLOCK_X 30
#define SB_BLOCK_Y 30
#define SB_MARGIN_X 2
#define SB_MARGIN_Y 2
#define SB_DEPTH_CHOICE 5 // ��֦����
#define SB_REST_ALL 25 // ʣ����پͲ���֦

class StarBoardBase
{
public:
    StarBoardBase( int size = SB_SIZE, int colors = SB_COLOR, bool bVerticalClear = true, bool bHorizontalClear = false );
    ~StarBoardBase();

    struct SelChain
    {
        CPoint  pt;         // ����
        int     length;     // ����
    };

    struct BoardSet
    {
        StarBoardBase*  board;      // ���棬���붯̬����/�ͷ�
        CList<CPoint>*  points;     // ���裬���붯̬����/�ͷ�
    };

    // ����
    void Reset(); // ����
    void Random(); // �������
    void Hit( LONG x, LONG y, bool bClear ); // ����
    void HitEdit( LONG x, LONG y, int colors ); // �༭
    bool SplitChains( CList<SelChain>& liSC ); // �ֽ���
    void HighestScore( CList<int>& score, bool bWrite = true ); // �޸���߷�
    void HighestSels( CList<CPoint>& sels, bool bWrite = true ); // �޸���Ѳ���
    void Lists( int& lists, bool bWrite = true ); // �޸Ķ�����
    void HideStep( bool& bHide, bool bWrite = true ); // ���ز���
    void CurrentRunovers( ULONG& cur, bool bWrite = true ); // ��ǰ��������
    void ColorAttr( CArray<int>& arrInt ); // ��ɫͳ�ƽ��

    // ����
    void Copy(const StarBoardBase& base); // ����
    bool IsDead() const; // �Ƿ��Ѿ���Ͷ��·
    int  GetScore() const; // �õ�����
    int  GetRest() const; // �õ�ʣ����
    void ClearHighest(); // �����߼�¼

protected:
    // ����
    void HVClear(); // ˮƽ��ֱ����
    void BlockClear(); // ������
    void CheckDead(); // ����Ƿ���Ͷ��·
    void BuildRefBoard(); // ����������
    void FactSel( CArray<int>& arrBoard, CList<CPoint>& liPoint, LONG x, LONG y ); // �ݹ飺����ѡ�еķ��飬����ӵ�����
    void FactSelV2( CArray<int>& arrBoard, LONG x, LONG y, int& length ); // �ݹ飺��ѡ����λ������

    // ����
    inline int CalcSelScore( int sels ); // ѡ�еķ���
    inline int CalcFinalScore( int rest ); // ��ֵķ���

    // ����
    void ClearRefBoard(); // ���������

private:
    StarBoardBase(const StarBoardBase& base); // ��ֹĬ�ϸ���

protected:
    CArray<int>     m_arrBoard; // ��������
    CArray<int>     m_arrRef; // ��������
    CList<CPoint>   m_liSels; // ѡ�з���
    bool            m_bVerticalClear; // ���ǣ���������� ���� ==> +y����
    bool            m_bHorizontalClear; // ���ǣ���������� ���� ==> +x����
    bool            m_checkDead; // �Ƿ��Ѿ���Ͷ��·
    int             m_selCounts; // ѡ�п������
    int             m_score; // ��Ҫ����
    int             m_restBlocks; // ʣ�෽��
    int             m_iSize; // �������
    int             m_iColors; // ��ɫ����

public:
    // ��̬��Ա
    static CList<CPoint>    m_HighestSels; // ��Ѳ���
    static CCriticalSection m_cs; // �ٽ���
    static int      m_iList; // ���������
    static ULONG    m_Runovers; // ��ǰ�ܼ�����
    static bool     m_bHideStep; // ���ز���
    static CList<int>       m_liHighestScore; // ��ʱ�����߷�
};

class StarBoard : public StarBoardBase
{
public:
    StarBoard( int size = SB_SIZE, int colors = SB_COLOR, bool bVerticalClear = true, bool bHorizontalClear = false );
    ~StarBoard();

public:
    // ����
    void HitFromPoint( CPoint pt, bool bClear = false ); // ����
    void HitEditFromPoint( CPoint pt, int colors); // �༭
    void Display( CDC* pDC, LONG x, LONG y, bool bShowEdit = false ); // ��ʾ
    void FillColor( int color ); // �����ɫ

    // ����
    void SetMargin( LONG x, LONG y ); // ���ü��
    void SetBlock( LONG x, LONG y ); // ���÷����С
    void SetColor( int index, COLORREF color ); // ������ɫ�� index:(0) Ϊ����ɫ
    void SetBorderColor( COLORREF color ); // ����ѡ�б߿���ɫ
    void SetSize( int size ); // ���ý���
    void SetColors( int colors ); // ������ɫ��

    // �㷨
    void RunWorkingThread(); // ���к�̨�߳�

    // �߳����
    void Terminate( bool& bTerminate, bool bWrite = true ); // �Ƿ�Ҫ��ֹ�߳�

    // �ļ�����
    void Serialize( CArchive& ar );

private:
    StarBoard(const StarBoard& base); // ��ֹĬ�ϸ���

private:
    // �㷨
    static UINT AFX_CDECL DaemonProcessor( LPVOID pVoid ); // ��̨���е�����㷨�߳�
    friend static UINT AFX_CDECL DaemonProcessor( LPVOID pVoid );

    void ClearBrush();

protected:
    CArray<CBrush*> m_arrColors; // ��ɫ����
    CArray<COLORREF> m_arrClr; // ��ɫ����(DWORD)
    CFont           m_font; // ��������
    CFont           m_font_cnt; // ��������
    CSize           m_szMargin; // ������
    CSize           m_szBlock; // �����С
    CPen            m_clrBorder; // ѡ�б߿���ɫ

public:
    static int          m_depthC; // ��֦����
    static int          m_restA; // ʣ��ȫ��
    static CWinThread*  m_pThread; // ��̨�߳�
    static bool         m_bTerminate; // �Ƿ�Ҫ��ֹ�߳�
};

