void CUser::UpdateCurCharacVisibleToCharacViewMemory()
{
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
