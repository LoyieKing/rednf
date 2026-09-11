// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
bool AvatarFixedHiddenOptionServer::CheckCondition(
    CUser* user, arad::SigAvatarHiddenOptionChange* sig)
{
    // [b28d-56 判词·N → L4fEventAvatar 2026-09-11 清零] ORIG 0x0817e4c6。
    // 旧「残余寄存器角色差」经 P59 guard 形复原后消失：原 isMaterialItemIndex
    // 判断写的是 if(真){正文} 后置失败块，4.4 把失败块排到函数尾（我方 239 条、
    // ORIG 置前 236 条）；改为 `if (!isMaterialItemIndex(...)) { 失败块; return
    // false; }` 后两侧逐行一致（AE，236/236）。
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
    // [L4fEventAvatar 2026-09-11] P59 guard 形复原：ORIG 0x0817e63c `xor $0x1,%eax;
    //   test %al,%al; je 0x0817e6aa <+0x1e4>`（je 直达 avatar 正文），失败块
    //   （0x0817e643 logFormat「isMaterial() failed」+ SendCmdError + mov $0 + jmp）
    //   紧随判断之后 ⇒ 原码 `if (!isMaterialItemIndex(...)) { …失败…; return false; }`；
    //   写 if(真){正文} 后置失败块会被 4.4 排到函数尾（实测与我方旧形同）。
    if (!ARAD::Singleton<AvatarFixedHiddenOptionServer>::Get()
             ->isMaterialItemIndex((int)material.m_addInfo))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp",
            __PRETTY_FUNCTION__,
            0x78,
            "AVATAR HIDDEN OPTION CHANGE : isMaterial() failed [index:%d] [charac_no:%d]",
            (int)material.m_addInfo, (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 0x11);
        return false;
    }
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