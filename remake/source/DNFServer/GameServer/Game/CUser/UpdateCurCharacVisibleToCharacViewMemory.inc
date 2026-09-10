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
                const short* expansion = static_cast<const short*>(info);
                vis.m_field4 = expansion[0];
                vis.m_field6 = expansion[1];
        }
    }
            }
