// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x081dc9ba orig/ours=511/537 ndiff-blocks=97
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 5 个，4.4 libstdc++ 全内联不可复现
//   [S162+S172] 已修：m_seedd.m_field & 3 → m_seedd.m_bits.m_low2 位域视角
//   （P308；Inven_Item.h 匿名 union，sizeof/偏移不变）⇒ 首差位 0→5。
//   首差: mov    %eax,-0x34(%ebp) | ... （S162 后首差位=5）
int Dispatcher_ChangeRandomOption::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // §6.2 语义完成：P167 调用点差集双零（73/73）；orig511/our537，gap-26 为
    // 后端帧/槽分配冗余（N82 残余），非缺语义 → byte 层 N 封顶。
    MSG_CHANGE_RANDOM_OPTION* local_38 = (MSG_CHANGE_RANDOM_OPTION*)&msg;
    Inven_Item* local_34;                // -0x30：GetInvenRef 结果
    CItem* local_30;                     // -0x2c：find_item 结果
    unsigned char local_29;              // -0x25：选项序号（msg+0x12）
    unsigned int local_28;               // -0x24：同上（uint 视图）
    unsigned int local_24;               // -0x20：getOptionModificationCost
    int local_20;                        // -0x1c：Secu Check
    char cVar4;
    bool bVar3;

    // [G4] P0/P183：ORIG 无 state 槽（cmp $2,%eax 直判）、无 invenW 槽 ⇒ 内联
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->CheckInTrade())
    {
        return 0;
    }
    if (user->CheckItemLock(1, (unsigned int)local_38->m_0e) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0xd5);
        return 0;
    }
    local_34 = user->getCurCharacInvenW()->GetInvenRef(
        1, (int)(unsigned int)local_38->m_0e);
    if (local_34 == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x11);
        return 0;
    }
    local_30 = G_CDataManager()->find_item(local_34->m_addInfo);   // *(int*)(local_34+2)
    if (local_30 == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x11);
        return 0;
    }
    local_29 = local_38->m_12;           // movzbl 0x12(eax)→-0x25
    if (((const unsigned char*)local_34)[(unsigned int)(unsigned char)local_29 * 3 + 0x25] == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x11);
        return 0;
    }
    if (((ServiceRestrictManager*)ARAD::Singleton<ServiceRestrictManager>::Get())
            ->isRestricted(user, (RestrictCategory::Enum)1, 0xf))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0xd1);
        return 0;
    }
    local_20 = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x25);
    if (local_20 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, (unsigned char)(local_20 & 0xff));
        return 0;
    }
    if (local_34->m_random.m_random.empty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x13);
        return 0;
    }
    local_28 = (unsigned int)(unsigned char)local_29;
    if (local_28 >= 3)                   // js/jle：<3 继续，否则 err 0x13
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x13);
        return 0;
    }
    if (!local_34->m_random.m_random.m_fielda.empty() &&
        ((unsigned int)local_34->m_random.m_random.m_seedd.m_bits.m_low2 != local_28))
    {
        bVar3 = true;
    }
    else
    {
        bVar3 = false;
    }
    if (bVar3)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 4);
        return 0;
    }
    local_24 = (unsigned int)G_CDataManager()->m_randomOption.getOptionModificationCost(
        (short)local_30->getUsableLevel(), (ENUM_RARITY)local_30->get_rarity());
    if (local_24 == 0xffffffff)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x11);
        return 0;
    }
    local_38->m_10 = 0xffff;
    if ((short)local_38->m_10 == -1)
    {
        if (user->getCurCharacInvenR()->get_money() < (int)local_24)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 10);
            return 0;
        }
    }
    else
    {
        Inven_Item local_81 = user->getCurCharacInvenR()->GetInvenSlot(
            1, (int)(unsigned int)local_38->m_10);  // 返回值落于 -0x7d 区，仅取 +0x2 字段判定
        if (local_81.m_addInfo < 1)      // -0x76 = local_81+0x2（mov -0x76,test,jg）
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x16);
            return 0;
        }
    }
    // static local：ORIG __cxa_guard(0x0930???)/CRandomOptionItemHandle C1 + __cxa_atexit
    static random_option::CRandomOptionItemHandle handle;
    // GetRandomOption() 返回 RandomOptionScript*（GlobalData.cpp:4059 T 0x085f0eb0），
    // ORIG `add $0x154`（0x085f0eb0 后 0x081dce5f）= RandomOptionScript::m_providerModified154
    // （CModifiedRandomOptionGive，基类 CRandomOptionGive 偏移 0，隐式上转）。
    handle.set_random_option_give(
        &reinterpret_cast<RandomOptionScript*>(
            random_option::GetRandomOption())->m_providerModified154);

    // ORIG 栈序（081dce5a..081dceeb）：
    //   &handle, itemIdx=*(int*)(local_34+2), eax=get_rarity, ebx=getUsableLevel,
    //   esi=getItemGroupName, edi=vtable+0x50, local_24=msg+0x12 选项序号,
    //   opts=local_34+0x25（实参右到左=rarity→Level→GroupName→vptr 保活 edi/esi/ebx）
    cVar4 = (char)handle.change_option(
        (unsigned long)local_34->m_addInfo,
        (ENUM_RARITY)local_30->get_rarity(),
        local_30->getUsableLevel(),
        local_30->getItemGroupName(),
        local_30->GetRandomOptionGrade(),                // vtable +0x50 槽虚调用
        (int)local_28,
        &local_34->m_random.m_random);
    // [DNF-FIX-MAGICSEAL 2026-10-07] ORIG 0x081dceeb..0x081dcef5：`call change_option;
    // xor $0x1,%eax; test %al,%al; je 0x081dcf1c(成功续行)` ⇒ 仅 change_option 失败(=0)
    // 才发 0x13。原译 `== 0` 极性写反：成功时内存里已写入变换结果(+0xa/+0xd 槽号)却回
    // 0x13 不扣费不存盘；之后其它槽因 low2 锁定回 4、同槽再回 0x13。
    if ((char)(cVar4 ^ 1) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x13);
        return 0;
    }
    user->m_historyLog.RandomOption(*local_34, (randomOptionReason::T)2);
    {
        char* uVar9c = GlobalData::s_systemTime_.getCurDate();
        statistc_proxy::add(user, "log_random_option", "change_cnt", 1,
                            "%d,'%s'", user->get_charac_10_level_section(), uVar9c);
    }
    local_38->m_10 = 0xffff;
    if ((short)local_38->m_10 == -1)
    {
        statistc_proxy::add(user, "log_random_option", "change_gold", local_24,
                            "%d,'%s'", user->get_charac_10_level_section(),
                            GlobalData::s_systemTime_.getCurDate());
        cVar4 = (char)user->getCurCharacInvenW()->use_money(
            (int)local_24, (eMoneySubReason)0x2c, 1);
        if ((char)(cVar4 ^ 1) != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 10);
            return 0;
        }
    }
    else
    {
        if (user->getCurCharacInvenW()->use_item(
                (int)(unsigned int)local_38->m_10, 1, 1) != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x16);
            return 0;
        }
        user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, (int)local_38->m_10);
    }
    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 0);
    {
        PacketGuard local_44;
        local_44.put_header(0, 0xe);
        local_44.put_byte(0);
        local_44.put_short(1);
        user->getCurCharacInvenR()->MakeItemPacket(
            (INVEN_TYPE)1, (int)(unsigned int)local_38->m_0e, local_44);
        local_44.finalize(true);
        user->Send(local_44);
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1b6);
    }
    return 0;
}