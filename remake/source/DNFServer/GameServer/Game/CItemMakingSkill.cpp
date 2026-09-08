// CItemMakingSkill.cpp —— CItemMakingSkill::getData(char*) const
// （OrigItemMaking15 批；本 TU 独占 CItemMakingSkill.cpp/.h）。
//
// ORIG T 0x08545b42（df_game_r objdump 逐指令转写，范围 0x08545b42..0x85b7d）：
//   sub $0x10；out = pData（-0x8）；i = 0（-0x4）；
//   do 循环 i<=0xa（cmpl $0xa; setle）：
//     movzwl 0x6(%this,%i,2),%cx（m_skillLevels[i] 零扩展）
//     mov %cx,0x4(%out,%i,2)（目标 pData+4+i*2，ushort 16 位写）
//
// SIG_ITEM_MAKING_SKILL 布局（_saveData 0x08545276 佐证）：characNo@+0（4 字节，
// 由 _saveData 填 get_charac_no）＋ m_skillLevels[11]@+4（ushort ×11），总 0x1c。
// getData 仅序列化技能表段，characNo 不由本函数写。
// 循环写法照 P2（cmpl $imm; setle ⇔ for(int i=0; i<=N; ++i)）。
//
// [b28-L14] 残差封顶 N 类（P30/N6 单条 scaled 寻址不可折 + N18a 双段 regmove）：
//   ORIG 0x08545b57 循环体 = `mov -0x4,%edx; mov -0x4,%ecx; mov 0x8(%ebp),%eax;
//   movzwl 0x6(%eax,%ecx,2),%ecx; mov -0x8(%ebp),%eax; mov %cx,0x4(%eax,%edx,2)`
//   —— 4.1 把「i 基址 + 常数 2」折进单条 disp+scale。4.4 -O0 对含常数下标的
//   store/load 恒展开 add/shl 链：`out[i+2]`、`(out+2)[i]`（P205 形实测仍展开，
//   因基数是寄存器而非段基址）、`*(unsigned short*)((char*)pData+4+i*2)`
//   三形均产 6 条；读侧 `0x6(%eax,%ecx,2)` 可折但目标 `0x4(%eax,%edx,2)` 不可。
//   两侧同证 = 跨版本寻址折叠固有差（同 N81/N94 族），语义等价（11 项 ushort
//   搬移到 pData+4）。
#include "CItemMakingSkill.h"

#include "CDataManager.h"   // G_CDataManager() / GetItemMakingSkill() / stItemMakingSkill

void CItemMakingSkill::getData(char* pData) const
{
    unsigned short* out = (unsigned short*)pData;
    for (int i = 0; i <= 0xa; ++i)
        out[i + 2] = m_skillLevels[i];
}
// ============================================================================
// CItemMakingSkill::GetPrevExpGap —— ORIG T 0x08545b7e（OrigItemGen20 批）。
// 报告 docs/class_func_reports/CItemMakingSkill/GetPrevExpGap.md：
//   G_CDataManager()->GetItemMakingSkill() 得技能经验表宿主（stItemMakingSkill，
//   +0x18 为 vector<pair<int,int>> 档位表）；idx = level - 1；
//   守卫 (unsigned)idx >= size() || idx == 0 → 返回 0（ORIG cmp/jbe + cmpl/jne
//   双判，jbe 为无符号比较）；否则返回 档位[idx].first - 档位[idx-1].first
//   （两次 vector::operator[]，idx 与 idx-1；ORIG mov (%eax),%ebx/sub 形态）。
// ============================================================================
int CItemMakingSkill::GetPrevExpGap(int level)
{
    stItemMakingSkill* table =
        reinterpret_cast<stItemMakingSkill*>(G_CDataManager()->GetItemMakingSkill());
    int idx = level - 1;
    if (static_cast<unsigned int>(idx) >= table->m_vec18.size() || idx == 0)
        return 0;
    return table->m_vec18[idx].first - table->m_vec18[idx - 1].first;
}
