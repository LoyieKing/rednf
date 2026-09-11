// ---------------------------------------------------------------------------
// ORIG W 0x084e91a4（报告缺失；objdump 逐指令核实）：
// for i in 0..count-1: Inven_Item& it = items[i]（stride 0x3d）；
//   it.m_addInfo2(+0x07) == addInfo 且 != 0 → return it.m_addInfo(+0x02)；
// 未命中 return 0。（mangled 参数为非 const 指针 _Z18GetItemIdByAddInfoP10Inven_Itemii。）
int GetItemIdByAddInfo(Inven_Item* items, int count, int addInfo)
{
    for (int i = 0; i < count; ++i)
    {
        // P183：ORIG 每次访问重发 imul $0x3d 全链（无 it 指针槽，P0）。
        if (items[i].m_addInfo2 == addInfo && items[i].m_addInfo2 != 0)
            return items[i].m_addInfo;
    }
    return 0;
}