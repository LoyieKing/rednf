void CInventory::MoveClearAvatar(int a, int b, int c, int d, int e, int f)
{
    // b28d-solo-CInventory：268/273，N7 间接 call 目标寄存器封顶（4.4.7 后端固有）。
    // b28d-37 [N7 封顶，ORIG 0x850?]：间接 call 目标寄存器 %eax↔%edx 互换（P230 换后端可翻）；268/273。
    CItem* pClearItem = 0;
    CItem* pTargetItem = 0;
    CItem* localDummy = 0;   // ORIG -0x34 槽：第三个具名局部（初值 0，无使用点，P232）
    if (c != 0)
    {
        pClearItem = G_CDataManager()->find_item(c);
        if (pClearItem != 0 && pClearItem->IsClearAvatar())
        {
            if (f != 0)
            {
                pTargetItem = G_CDataManager()->find_item(f);
                if (pTargetItem != 0 && !pTargetItem->IsClearAvatar())
                {
                    SetClearAvatar(d, f);
                    if (d < 0 || d > 0x19)
                    {
                        return;
                    }
                    Inven_Item* pSrcItem = &m_invenItem[d];
                    stAvatarExpansionInfo_t* expInfo =
                        (stAvatarExpansionInfo_t*)GetAvatarItemMgrW()->GetExpansionInfo(
                            pSrcItem->m_addInfo2);
                    if (expInfo == 0)
                    {
                        return;
                    }
                    int avatarIdx = a - 10;
                    if (avatarIdx < 0 || avatarIdx > 0x68)
                    {
                        return;
                    }
                    Inven_Item* slotItem = &m_pAvatarSlot[avatarIdx];
                    std::map<int, AvatarVariation::colorRGB>& colorMap =
                        G_CDataManager()->m_avatarColor.m_colorMap;
                    int colorId1 = (int)expInfo->m_color[0];
                    if (colorMap.find(colorId1) == colorMap.end())
                    {
                        LogManager::logFormat(
                            1, "inventory.cpp", __PRETTY_FUNCTION__, 0x21db,
                            "Avatar color1 error. ItemID(%d), colorId(%d)",
                            pSrcItem->m_addInfo2, (int)expInfo->m_color[0]);
                        return;
                    }
                    int colorId2 = (int)expInfo->m_color[1];
                    if (colorMap.find(colorId2) == colorMap.end())
                    {
                        LogManager::logFormat(
                            1, "inventory.cpp", __PRETTY_FUNCTION__, 0x21e2,
                            "Avatar color2 error. ItemID(%d), colorId(%d)",
                            pSrcItem->m_addInfo2, (int)expInfo->m_color[1]);
                        return;
                    }
                    bool colorOk = GetAvatarItemMgrW()->SetAvatarColor(
                        slotItem->m_addInfo2, expInfo->m_color[0], expInfo->m_color[1]);
                    if (!colorOk)
                    {
                        return;
                    }
                    SendAvatarEvent(0x31a, slotItem->m_addInfo2, expInfo->m_color[0],
                                    expInfo->m_color[1], 0);
                }
            }
        }
    }
    if (f != 0)
    {
        if (pTargetItem == 0)
        {
            pTargetItem = G_CDataManager()->find_item(f);
        }
        if (pTargetItem != 0 && pTargetItem->IsClearAvatar())
        {
            SetClearAvatar(d, 0);
        }
    }
