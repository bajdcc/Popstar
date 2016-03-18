#pragma once
#include "afx.h"

#define SB_SIZE 6
#define SB_COLOR 5
#define SB_BLOCK_X 30
#define SB_BLOCK_Y 30
#define SB_MARGIN_X 2
#define SB_MARGIN_Y 2
#define SB_DEPTH_CHOICE 5 // 剪枝上限
#define SB_REST_ALL 25 // 剩余多少就不剪枝

class StarBoardBase
{
public:
    StarBoardBase( int size = SB_SIZE, int colors = SB_COLOR, bool bVerticalClear = true, bool bHorizontalClear = false );
    ~StarBoardBase();

    struct SelChain
    {
        CPoint  pt;         // 链点
        int     length;     // 长度
    };

    struct BoardSet
    {
        StarBoardBase*  board;      // 局面，必须动态创建/释放
        CList<CPoint>*  points;     // 步骤，必须动态创建/释放
    };

    // 方法
    void Reset(); // 重置
    void Random(); // 随机生成
    void Hit( LONG x, LONG y, bool bClear ); // 触发
    void HitEdit( LONG x, LONG y, int colors ); // 编辑
    bool SplitChains( CList<SelChain>& liSC ); // 分解链
    void HighestScore( CList<int>& score, bool bWrite = true ); // 修改最高分
    void HighestSels( CList<CPoint>& sels, bool bWrite = true ); // 修改最佳步骤
    void Lists( int& lists, bool bWrite = true ); // 修改队列数
    void HideStep( bool& bHide, bool bWrite = true ); // 隐藏步骤
    void CurrentRunovers( ULONG& cur, bool bWrite = true ); // 当前计算总数
    void ColorAttr( CArray<int>& arrInt ); // 颜色统计结果

    // 属性
    void Copy(const StarBoardBase& base); // 复制
    bool IsDead() const; // 是否已经走投无路
    int  GetScore() const; // 得到分数
    int  GetRest() const; // 得到剩余数
    void ClearHighest(); // 清除最高纪录

protected:
    // 方法
    void HVClear(); // 水平垂直消除
    void BlockClear(); // 块消除
    void CheckDead(); // 检查是否走投无路
    void BuildRefBoard(); // 生成引用区
    void FactSel( CArray<int>& arrBoard, CList<CPoint>& liPoint, LONG x, LONG y ); // 递归：返回选中的方块，并添加到链表
    void FactSelV2( CArray<int>& arrBoard, LONG x, LONG y, int& length ); // 递归：将选中链位置置零

    // 内联
    inline int CalcSelScore( int sels ); // 选中的分数
    inline int CalcFinalScore( int rest ); // 结局的分数

    // 属性
    void ClearRefBoard(); // 清空引用区

private:
    StarBoardBase(const StarBoardBase& base); // 禁止默认复制

protected:
    CArray<int>     m_arrBoard; // 方块数据
    CArray<int>     m_arrRef; // 引用数据
    CList<CPoint>   m_liSels; // 选中方块
    bool            m_bVerticalClear; // 若是，则消除完毕 方块 ==> +y方向
    bool            m_bHorizontalClear; // 若是，则消除完毕 方块 ==> +x方向
    bool            m_checkDead; // 是否已经走投无路
    int             m_selCounts; // 选中块的数量
    int             m_score; // 主要分数
    int             m_restBlocks; // 剩余方块
    int             m_iSize; // 方块阶数
    int             m_iColors; // 颜色数量

public:
    // 静态成员
    static CList<CPoint>    m_HighestSels; // 最佳步骤
    static CCriticalSection m_cs; // 临界区
    static int      m_iList; // 计算队列数
    static ULONG    m_Runovers; // 当前总计算量
    static bool     m_bHideStep; // 隐藏步骤
    static CList<int>       m_liHighestScore; // 临时存放最高分
};

class StarBoard : public StarBoardBase
{
public:
    StarBoard( int size = SB_SIZE, int colors = SB_COLOR, bool bVerticalClear = true, bool bHorizontalClear = false );
    ~StarBoard();

public:
    // 方法
    void HitFromPoint( CPoint pt, bool bClear = false ); // 触发
    void HitEditFromPoint( CPoint pt, int colors); // 编辑
    void Display( CDC* pDC, LONG x, LONG y, bool bShowEdit = false ); // 显示
    void FillColor( int color ); // 填充颜色

    // 属性
    void SetMargin( LONG x, LONG y ); // 设置间距
    void SetBlock( LONG x, LONG y ); // 设置方块大小
    void SetColor( int index, COLORREF color ); // 设置颜色， index:(0) 为背景色
    void SetBorderColor( COLORREF color ); // 设置选中边框颜色
    void SetSize( int size ); // 设置阶数
    void SetColors( int colors ); // 设置颜色数

    // 算法
    void RunWorkingThread(); // 运行后台线程

    // 线程相关
    void Terminate( bool& bTerminate, bool bWrite = true ); // 是否要终止线程

    // 文件操作
    void Serialize( CArchive& ar );

private:
    StarBoard(const StarBoard& base); // 禁止默认复制

private:
    // 算法
    static UINT AFX_CDECL DaemonProcessor( LPVOID pVoid ); // 后台运行的求解算法线程
    friend static UINT AFX_CDECL DaemonProcessor( LPVOID pVoid );

    void ClearBrush();

protected:
    CArray<CBrush*> m_arrColors; // 颜色数据
    CArray<COLORREF> m_arrClr; // 颜色数据(DWORD)
    CFont           m_font; // 文字字体
    CFont           m_font_cnt; // 计数字体
    CSize           m_szMargin; // 方块间距
    CSize           m_szBlock; // 方块大小
    CPen            m_clrBorder; // 选中边框颜色

public:
    static int          m_depthC; // 剪枝上限
    static int          m_restA; // 剩余全搜
    static CWinThread*  m_pThread; // 后台线程
    static bool         m_bTerminate; // 是否要终止线程
};

