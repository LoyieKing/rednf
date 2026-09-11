// [b28d-solo-CDataManager AE] 修复：原嵌套 if 形被 4.4 折成内存直比（cmpl $imm,0xc(%ebp)），
// ORIG 为形参物化 + 寄存器比较链。改 switch 连续 case 组（0x2898bf/c0→53、
// c1/c2→57）即复现 `mov 0xc(%ebp),%eax; cmp $0x2898bf,%eax; jl fail; …`
// 决策链（P251/P261 switch 判据 + 探针 /tmp/p_sp/h.cpp f6 逐条全等）。
void ChattingEmoticonBase::AddPremiumIndex(int index)
{
    // ORIG 0x080e5dde：特定 index 映射到 premium 槽位集合。
    switch (index)
    {
    case 0x2898bf:
    case 0x2898c0:
        m_set.insert(0x53);
        break;
    case 0x2898c1:
    case 0x2898c2:
        m_set.insert(0x57);
        break;
    }
}