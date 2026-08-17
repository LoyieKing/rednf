// df_game_r PvpResultRecvFlag 还原（G2-4 批次）。
// 对照 docs/class_func_reports/PvpResultRecvFlag.md 与 ORIG 反汇编逐函数实现。
// 布局：8 个 byte 标志（PvpResultRecvFlag 总尺寸 8）。
// 注意：GameStubs.cpp 无本类桩，本 TU 直接提供权威定义。

#include <string.h>

class PvpResultRecvFlag
{
public:
    PvpResultRecvFlag();
    ~PvpResultRecvFlag();

    bool IsValidIdx(int idx) const;
    char IsRecv(int idx) const;
    void SetRecvFlag(int idx);
    void Clear();

private:
    char m_flag[8];  // +0x00
};

PvpResultRecvFlag::PvpResultRecvFlag()
{
    Clear();
}

PvpResultRecvFlag::~PvpResultRecvFlag()
{
}

char PvpResultRecvFlag::IsRecv(int idx) const
{
    if (idx < 0 || idx > 7)
    {
        return 0;
    }
    return m_flag[idx];
}

void PvpResultRecvFlag::SetRecvFlag(int idx)
{
    if (!IsValidIdx(idx))
    {
        return;
    }
    m_flag[idx] = 1;
}

void PvpResultRecvFlag::Clear()
{
    for (int i = 0; i <= 7; ++i)
    {
        m_flag[i] = 0;
    }
}

bool PvpResultRecvFlag::IsValidIdx(int idx) const
{
    if (idx >= 0 && idx <= 7)
    {
        return true;
    }
    else
    {
        return false;
    }
}
