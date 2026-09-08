// df_game_r Game/ AdditionalDungeonInfo（附加副本信息，QuestScript::m_field16c 元素）。
// ORIG 符号：
//   _ZN21AdditionalDungeonInfoD1Ev W 0x08a6887c（空体）
//   _ZN21AdditionalDungeonInfoD2Ev W 0x08a6887c（与 D1 同址）
// 布局证据（ORIG ELF objdump/nm）：
//   - 无 _ZTV21AdditionalDungeonInfo / _ZTI21AdditionalDungeonInfo → 非多态，
//     析构非 virtual（HdrCreate1 旧建模的 virtual 已修正）。
//   - new_allocator::construct（0x08a66dde）直接位拷贝 8 字节（两个 mov）；
//     __normal_iterator operator-（0x08a68820）sar $0x3 → sizeof == 8。
//   - importQuestScript "[dungeon info]" 分支：两次 ScanInt 后 push_back，
//     两字段各 4 字节。
// 属主 TU：CDungeonExpStatistic.cpp 提供空析构定义；其余 TU 只 include 本头。
#ifndef DNF_GAME_ADDITIONALDUNGEONINFO_H_
#define DNF_GAME_ADDITIONALDUNGEONINFO_H_

class AdditionalDungeonInfo
{
public:
    // [b28d-Main 代落·S141 P12] 删用户析构声明：ORIG 0x08a6887c 仅他处 W，
    // importQuestScript 段零 D1 调用 ⇒ 隐式 trivial。sizeof/偏移/mangled 不变。

    int m_field0;   // +0x00（"[dungeon info]" 首 ScanInt，语义推断）
    int m_field4;   // +0x04（"[dungeon info]" 次 ScanInt，语义推断）
};


#endif // DNF_GAME_ADDITIONALDUNGEONINFO_H_
