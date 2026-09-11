// ORIG Dispatcher_AradJumpingCharacter::checkWorkState 0x0819f44c W：
//   Secu Check(user,0x2a)；eventKind==2 时要求存在 m_level>0x3b 的角色，
//   否则错误 8；当前角色 level>0x11 → 错误 0x12；取 (INVEN 1,
//   sig.m_itemIndex 槽) 物品：空 → 错误 0x11；eventKind==1 须 item idx
//   ==0x28d468、eventKind==2 须 ==0x28d469，否则错误 0x11；其余 eventKind
//   → 错误 0x11。全部通过返回 true。
bool Dispatcher_AradJumpingCharacter::checkWorkState(CUser* user,
                                                     ::arad::SigAradJumpingCharacter& sig)
{
    // [b23S3C] P48：m_eventKind 三用点各重取（ORIG 无 jumpKind 槽）；hasLevel
    // 声明预清提前至函数头（ORIG `movb $0,-0x15` 在 Check call 前）；isEmpty
    // 正形早返（ORIG test;je else 岛）。残余 N 封顶：`Inven_Item item =
    // GetInvenSlot()` 4.4 copy-elision 直构 sret（N86，ORIG 4.1 先 ctor+temp+
    // 15 mov 逐字段拷贝）；各 err-return 岛位 4.4 jump-threading 差。
    bool hasLevelOver0x3b = false;                                // ORIG -0x15
    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x2a);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, (unsigned char)secuErr);
        return false;
    }

    if (sig.m_eventKind == 2)
    {
        for (std::size_t i = 0; i < user->m_characList.size(); ++i)
        {
            if (user->m_characList[i].m_level > 0x3b)             // elem +0x27 word
            {
                hasLevelOver0x3b = true;
                break;
            }
        }
        if (!hasLevelOver0x3b)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 8);
            return false;
        }
    }

    // 当前角色等级门（ORIG cmp $0x11 / setg：level>0x11 报错 0x12）。
    if (user->get_charac_level() > 0x11)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x12);
        return false;
    }

    CInventory* inv = user->getCurCharacInvenW();
    Inven_Item item;                 // ORIG `call Inven_ItemC1Ev` 默认构造
    item = inv->GetInvenSlot(1, (int)(unsigned short)sig.m_itemIndex);
    if (item.isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x11);
        return false;
    }
    if (sig.m_eventKind == 1)
    {
        // b28：ORIG 0x0819f25x 双臂 `cmp $K,%eax; je <err>`——命中不等即直落错误体
        // （无真岛）。4.4 正向 == 形产 jne+真值岛（+2 jmp）；改负判 `!=`：je 直落
        // err、真臂 fall-through 至统一尾 return true，与 ORIG 布局同构。
        if (item.m_addInfo != 0x28d468)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x11);
            return false;
        }
    }
    else if (sig.m_eventKind == 2)
    {
        if (item.m_addInfo != 0x28d469)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x11);
            return false;
        }
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x249, 0x11);
        return false;
    }
    return true;
}