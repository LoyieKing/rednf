// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPartyResultRecvFlag/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
    unsigned char IsRecv(int idx) const;
    void SetRecvFlag(int idx);
    void Clear();

private:
    char m_flag[4];       // +0x00
    char m_pad[0x30];     // +0x04..+0x33
};

#include "CPartyResultRecvFlag/CPartyResultRecvFlag.inc"
{
    Clear();
}

#include "CPartyResultRecvFlag/SetRecvFlag.inc"
{
    if (!IsValidIdx(idx))
    {
        return;
    }
    m_flag[idx] = 1;
}

#include "CPartyResultRecvFlag/IsRecv.inc"
{
    if (!IsValidIdx(idx))
    {
        return 0;
    }
    return m_flag[idx];
}

#include "CPartyResultRecvFlag/Clear.inc"
{
    memset(m_flag, 0, sizeof(m_flag));
}

#include "CPartyResultRecvFlag/IsValidIdx.inc"
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
