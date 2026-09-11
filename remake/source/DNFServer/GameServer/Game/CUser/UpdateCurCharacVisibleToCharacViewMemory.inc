// ============================================================================
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x08651628 | _ZN5CUser40UpdateCurCharacVisibleToCharacViewMemoryEv
void CUser::UpdateCurCharacVisibleToCharacViewMemory()
{
    // 2026-09-10（G3CUser-B 负结论）：ORIG 0x08651628 vs ours（88→92 insn，帧
    //   0x34→0x38）差异的根在 **CInventory 头内布局**，非本 .inc 可修：
    //   ORIG 0x0865170f `mov -0x18(%ebp),%eax; imul $0x3d,%eax,%eax; add $0x10,%eax;
    //   add -0x1c(%ebp),%eax; add $0xc,%eax`（vis = inven + i*0x3d + 0x1c）与
    //   `add $0xd0,%eax; shl $0x3,%eax; add -0x1c(%ebp),%eax; add $0x4`（m_invenItem
    //   步长 8、基址 +0x680）两侧采用**不同元素步长/基址**；我方 `m_avatarVisible[i]`
    //   走 8 字节步长视图 ⇒ 结构步长与 ORIG 不符。属 .h（CInventory.h）类型轴，
    //   按约束只报告不落，已报 Main。
    CInventory* inven = getCurCharacInvenW();
    if (inven == 0)
        return;

    for (int i = 0; i < 12; ++i)                                 // ORIG 0..0xb
    {
        CInventory::AvatarVisibleData& vis = inven->m_avatarVisible[i];  // b28d-lane5: Main 代落匿名 struct 包裹（N84）
        if (inven->m_invenItem[i].m_addInfo == 0)
        {
            vis.init();                                          // ORIG 0x0865171f 调 init
        }
        else
        {
            // ORIG：GetClearAvatar(i) 在 if 条件与 else 分支各自重调，结果禁缓存
            // （战术 P183/P48）。
            if (inven->GetClearAvatar(i) == 0)
                vis.m_field0 = inven->m_invenItem[i].m_addInfo;
            else
                vis.m_field0 = inven->GetClearAvatar(i);

            // ORIG key = item+0x07（m_addInfo2）；GetExpansionInfo 返回 {short,short}
            const WongWork::CAvatarItemMgr* mgr = inven->GetAvatarItemMgrR();
            void* info = const_cast<WongWork::CAvatarItemMgr*>(mgr)
                             ->GetExpansionInfo(inven->m_invenItem[i].m_addInfo2);
            if (info != 0)
            {
                // [L9-C组J PATCH J3] ORIG `movzwl 0x2(%eax),%edx` 单条 disp；`expansion[1]`
                //   （常量下标）在 4.4 产 `add $0x2,%eax; movzwl (%eax),%edx`（多 1 条）
                //   ⇒ 改成员访问（p_disp f_pidx vs f_member 直证）。
                struct ExpansionPair
                {
                    short m_first;   // +0x0
                    short m_second;  // +0x2
                };
                const ExpansionPair* expansion =
                    static_cast<const ExpansionPair*>(info);
                vis.m_field4 = expansion->m_first;
                vis.m_field6 = expansion->m_second;
        }
    }
            }



}