// ---------------------------------------------------------------------------
// 23) CUser::apply_elixir_item(_Additioal_info&, pair<int,int>&)
//     报告：docs/class_func_reports/CUser/apply_elixir_item.md。ORIG 0x08665274。
//     switch(first)：0x40f..0x412 → 短字段+count*50；0x413/0x414 → int+count*250；
//     0x415 → int@+0x42+count*10；0x416 → 循环 4 个 short+=count*10。
//     _Additioal_info 内字段由 FixAdditionalInfoLeaves 批建模；此处按 m_pad 视图改具名草案。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08665274：p167=CLEAR，gap=-2；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08665274：N7 封顶——数组值/索引的 callee-saved
// 角色互换（ORIG ecx=索引/ebx=值，4.4 ebx=索引/ecx=值）+ lea-vs-add；130 vs 128 条。
void CUser::apply_elixir_item(_Additioal_info& ai, std::pair<int, int>& pr)
{
    switch (pr.first)
    {
    case 0x40f: ai.m_field08 += (short)(pr.second * 50); break;   // +0x08
    case 0x410: ai.m_field0c += (short)(pr.second * 50); break;   // +0x0c
    case 0x411: ai.m_field0a += (short)(pr.second * 50); break;   // +0x0a
    case 0x412: ai.m_field0e += (short)(pr.second * 50); break;   // +0x0e
    case 0x413: ai.m_field00 += pr.second * 250; break;           // +0x00
    case 0x414: ai.m_field04 += pr.second * 250; break;           // +0x04
    case 0x415: ai.m_field42 += pr.second * 10;  break;           // +0x42
    case 0x416:
        // [b28d-22 N7] i 保持 int：unsigned 试形使尾测变 setbe（ORIG 0x0866548x
        // `cmpl $0x3,-0x8; setle` 有符号）⇒ 回退。寄存器角色（ORIG i=%ecx/值=%ebx，
        // 4.4 反置）与加法分组（ORIG lea(%ebx,%eax,1) vs 4.4 add）为 N7/N18a 封顶。
        for (int i = 0; i < 4; ++i)
        {
            /* +0x10, +0x12, +0x14, +0x16（ORIG (i+8)*2 索引形态） */
            // [CoBatchA 2026-09-11 负结论·已回滚] 交换加法操作数
            // （`(short)(pr.second * 10 + ai.m_field10[i])`）实测 **129→129 中性**
            // （`lea (%ebx,%eax,1)` 未复现）⇒ 维持 `+=`；残余为 N7/N18a 寄存器角色封顶。
            ai.m_field10[i] += (short)(pr.second * 10);
        }
        break;
    default:
        break;
    }
}