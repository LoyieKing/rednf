// df_game_r CPartyResultRecvFlag 还原（G2-4 批次）。
// CParty +0x1dc，总尺寸 0x34；无独立报告，按 ORIG 反汇编 +
// PvpResultRecvFlag.md 参考实现。方法清单（ORIG 符号）：
//   C1 0x82a4cb2 / Clear 0x82a4cc6 / SetRecvFlag 0x85bf3d8 /
//   IsRecv 0x85bf406 / IsValidIdx 0x85bf438（ORIG 无 D1/D2）。
// 布局：+0x00 char flag[4]，+0x04..+0x33 保留区。

#include <string.h>

class CPartyResultRecvFlag
{
public:
    CPartyResultRecvFlag();

    bool IsValidIdx(int idx) const;
    char IsRecv(int idx) const;
    void SetRecvFlag(int idx);
    void Clear();

private:
    char m_flag[4];       // +0x00
    char m_pad[0x30];     // +0x04..+0x33
};

CPartyResultRecvFlag::CPartyResultRecvFlag()
{
    Clear();
}

void CPartyResultRecvFlag::SetRecvFlag(int idx)
{
    if (!IsValidIdx(idx))
    {
        return;
    }
    m_flag[idx] = 1;
}

char CPartyResultRecvFlag::IsRecv(int idx) const
{
    if (!IsValidIdx(idx))
    {
        return 0;
    }
    return m_flag[idx];
}

void CPartyResultRecvFlag::Clear()
{
    memset(m_flag, 0, sizeof(m_flag));
}

bool CPartyResultRecvFlag::IsValidIdx(int idx) const
{
    if (idx >= 0 && idx <= 3)
    {
        return true;
    }
    else
    {
        return false;
    }
}
