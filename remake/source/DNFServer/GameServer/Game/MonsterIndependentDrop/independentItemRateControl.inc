// ----------------------------------------------------------------------------
// ORIG 0x0834972f _Z26independentItemRateControlR26stMonsterIndependentDrop_t
//                    RKN8WongWork19stGenerateRefData_tE
// 遍历 G_CDataManager()+0xa89c 的 vector<stIndependentDropRateControl>
// （即 m_independentDrop.m_vec18，+0xa884 + 0x18 实证）：
//   ctrl.m_itemId == drop.m_itemIdx 且 drop.m_rate[jobIdx] != 0 时，
//   diff = (u8)ref.m_field3f - (u8)ref.m_field34；仅当 ctrl.m_field04 <= diff：
//   f = (float)(10000 - ctrl.m_field08*diff)；f==0 → f=1.0f；
//   drop.m_rate[jobIdx] = (int)(rate * (f / 10000.0f))（fistpl 舍入）。
void independentItemRateControl(stMonsterIndependentDrop_t& drop,
                                const WongWork::stGenerateRefData_t& ref)
{
    std::vector<stIndependentDropRateControl>& ctrls =
        G_CDataManager()->m_independentDrop.m_vec18;

    const unsigned char jobIdx = (unsigned char)ref.m_rateSlot;    // movzbl 0x36
    const unsigned char base34 = (unsigned char)ref.m_field34;     // movzbl 0x34

    for (std::vector<stIndependentDropRateControl>::iterator it = ctrls.begin();
         it != ctrls.end(); ++it)
    {
        if (it->m_itemId != drop.m_itemIdx)                        // setne → next
            continue;
        if (drop.m_rate[jobIdx] == 0)                              // test → next
            continue;

        // ORIG 0x83497a5 `movzbl 0x3f(%eax),%eax` ⇒ +0x3f 独立字节视图（P196
        // 字段宽度；旧 `m_goldFactor>>8` 恒产 movzwl+shr）。
        const int diff = (int)ref.m_field3f - (int)base34;          // +0x3f（等级上界）
        // ORIG 0x83497c7 `cmp diff; setle %al; test; je <循环尾>` ⇒ 仅当
        // m_field04 <= diff 时**执行**衰减体（Ghidra C 同判：`if (field04 <= diff) {...}`）。
        // 旧源码写成 `<= diff` 时 continue ⇒ 条件反向真 bug（衰减作用到错误条目）。
        if (it->m_field04 > diff)
            continue;

        int value = 10000 - it->m_field08 * diff;                  // imul + 0x2710 sub
        float factor = (float)value;                               // fildl/fstps
        if (factor == 0.0f)                                        // fucompp fldz
            factor = 1.0f;
        // flds 0x8c349ec = 10000.0f；fdivrp/fmulp → rate*(factor/10000.0f)
        drop.m_rate[jobIdx] = (int)((float)drop.m_rate[jobIdx] *
                                    (factor / 10000.0f));          // fistpl 回写
    }
}