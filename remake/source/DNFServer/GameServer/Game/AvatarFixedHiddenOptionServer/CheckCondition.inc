bool AvatarFixedHiddenOptionServer::CheckCondition(
    CUser* user, arad::SigAvatarHiddenOptionChange* sig)
{
    // [b28d-56 判词·N] N7/N82附：ORIG 0x0817e4c6 236 条 vs 我们 239 条；残余为
    // 寄存器角色差（ORIG 全程 %edx 算 materialSlot：`mov 0x10(%ebp),%edx;
    // movzwl (%edx),%edx; movswl %dx,%edx`；4.4 恒 %eax+cwtl）。语义完成。
    CInventory* invW = user->getCurCharacInvenW();
    // 材料槽（INVEN 1）—— ORIG 以 sret 形式调用 GetInvenSlot（按值返回）
    Inven_Item material;
    material = invW->GetInvenSlot(1, (int)sig->m_materialSlot);
    if (material.isEmpty())
    {
        LogManager::logFormat(
            1, "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp",
            __PRETTY_FUNCTION__,
            0x71,
            "AVATAR HIDDEN OPTION CHANGE : Material GetInvenSlot() failed. [slot:%d] [charac_no:%d]",
            // [b28d-56] ORIG 0x0817e5?? 发两段 `movzwl (%edx),%edx; movswl %dx,%edx`
            // （SIGFIX 双段判据 ⇒ 源码 `(int)(short)字段`，字段本身 ushort）。
            (int)(short)sig->m_materialSlot, (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 0x16);
        return false;
    }
    // [b23S3C] ORIG +0x162 call Singleton::Get(); add $4 → m_script 再调成员
    // （P48 使用点重取单例），m_script 直成员调会少一发 call（p167 实证）。
    if (ARAD::Singleton<AvatarFixedHiddenOptionServer>::Get()
            ->isMaterialItemIndex((int)material.m_addInfo))
    {
        Inven_Item avatar;
        avatar = invW->GetInvenSlot(2, (int)sig->m_avatarSlot);
        if (avatar.isEmpty())
        {
            LogManager::logFormat(
                1, "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp",
                __PRETTY_FUNCTION__,
                0x83,
                "AVATAR HIDDEN OPTION CHANGE : Avatar GetInvenSlot() failed. [slot:%d] [charac_no:%d]",
                (int)sig->m_avatarSlot, (unsigned int)user->getCurCharacNo());
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 0x16);
            return false;
        }
        CItem* item = G_CDataManager()->find_item((int)avatar.m_addInfo);
        if (item == 0 || !item->IsAvatarItem())
            return false;
        if (!item->IsHiddenOption())
        {
            LogManager::logFormat(
                1, "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp",
                __PRETTY_FUNCTION__,
                0x94,
                "AVATAR HIDDEN OPTION CHANGE : IsHiddenOption() failed. [index:%d] [charac_no:%d]",
                (int)avatar.m_addInfo, (unsigned int)user->getCurCharacNo());
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 0x11);
            return false;
        }
        return true;
    }
    LogManager::logFormat(
        1, "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp",
        __PRETTY_FUNCTION__,
        0x78,
        "AVATAR HIDDEN OPTION CHANGE : isMaterial() failed [index:%d] [charac_no:%d]",
        (int)material.m_addInfo, (unsigned int)user->getCurCharacNo());
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 0x11);
    return false;
