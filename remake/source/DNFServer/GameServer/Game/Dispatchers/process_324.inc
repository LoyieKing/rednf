// ==== Dispatcher_CeraPackageOpen::process (ORIG 0x081e788a) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e788a orig/ours=289/287 ndiff-blocks=45
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x28(%ebp) | mov    -0x28(%ebp),%edx
int Dispatcher_CeraPackageOpen::process(CUser* user, MSG_BASE& msg,
                                        ParamBase& param)
{
    char* const M = reinterpret_cast<char*>(&msg);
    char* const P = reinterpret_cast<char*>(&param);

    const int err = check_error(user, msg);
    *reinterpret_cast<int*>(P + 4) = err;                   // stOpenCeraPackage::m_4
    if (err != 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x207),
                                 static_cast<unsigned char>(err & 0xff));
        return 0;
    }

    const short slot = *reinterpret_cast<const short*>(M + 0xe);
    if (user->CheckItemLock(1, static_cast<int>(slot)))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x207), 4);
        return 0;
    }

    const Inven_Item slotItem =
        user->getCurCharacInvenR()->GetInvenSlot(1, static_cast<int>(slot));
    const int itemIdx = slotItem.m_addInfo;                 // Inven_Item+0x02
    if (itemIdx == 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x207), 1);
        return 0;
    }

    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
    {
        return 1;                                           // 静默返回（ORIG ebx=1）
    }
    // ORIG 0x081e79d3：第二段守卫为 `item == 0 || !is_stackable() || GetItemType() != 0x26`
    // 负极性短路链（ORIG 二次判空 `cmpl $0x0,-0x20(%ebp); je 0x081e7a02(失败岛)`，第九轮 ④ 补齐）
    if (item == 0 || !item->is_stackable() || item->GetItemType() != 0x26)
    {
        // vtable+0xc 直调 = GetItemType()（P5；CItem.h 虚序 D1/D0/make_item/GetItemType）
        return 1;
    }

    {
        // ---- 构造特殊商品处理入参（ORIG 0x081e7a1c..0x081e7b31 逐指令转写）----
        WongWork::CCeraShop::stCeraShopItemParam_t ceraParam;   // ORIG ctor 0x0817a244
        // [R21-20 2026-09-13 §27.4] ORIG 0x081e7a8c 循环尾 `mov -0x2c(%ebp),%eax;
        //   movzbl 0x10(%eax),%eax` = 每轮重读 msg+0x10 字段（全函数无具名 count 槽、
        //   循环体内无该字段写点 ⇒ 字段值恒定但 ORIG 逐轮重读）⇒ 删具名 subCount 缓存，
        //   循环条件内联表达式（P0：ORIG 无槽往返 ⇒ 原码为内联右值）。
        for (int i = 0; i < (int)(unsigned int)(unsigned char)M[0x10]; ++i)
        {
            // ORIG pair<int,int> ctor（Rm/Rh）：first=*(ulong*)(M+(i+2)*8+4) 低 32 位，
            // second=*(uchar*)(M+(i+2)*8+8)。显式 pair 构造（非 make_pair）。
            // ORIG 符号 _ZNSt4pairIiiEC1IRmRhEEOT_OT0_：非 const ulong&/uchar& 左值
            unsigned long v1 =
                *(unsigned long*)(M + (i + 2) * 8 + 4);
            unsigned char v2 =
                *(unsigned char*)(M + (i + 2) * 8 + 8);
            ceraParam.m_pairs.push_back(std::pair<int, int>(v1, v2));
        }

        std::vector<Inven_Item> items;                      // ORIG vector<Inven_Item> C1

        STSpecailItem_Param sp(items, ceraParam);           // W 0x08234ec2
        sp.m_user = user;
        sp.m_itemIdx = itemIdx;
        // [R13a18 形态复刻] ORIG 0x081e7ace `movl $0x0,-0x84(%ebp)` = 结构 +0x8
        // （m_value）显式清零写点；我方此前缺该写点（ctor 已置 0 ⇒ 语义不可观察，
        // 但 ORIG 的字段写集合含 +0x8，按「完全复刻」口径补齐，P348）。
        sp.m_value = 0;
        sp.m_count = 0;
        sp.m_time = 0;
        sp.m_flag = 0;
        sp.m_char1d = 0;
        sp.m_char1e = 0;
        STSpecailItem_Result result;                        // W 0x08234f36（清零首 4 字节）
        const bool handlerOk =
            GlobalData::s_pCeraShop->CallSpecialItemHandler(5, sp, result);
        const int resultValue = result.m_value;

        if (!handlerOk)
        {
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x207),
                                     static_cast<unsigned char>(resultValue & 0xff));
            return 0;
        }

        if (resultValue == 0)
        {
            // 扣除礼包本体并回 ACK 包
            user->getCurCharacInvenW()->delete_item(
                static_cast<INVEN_TYPE>(1), static_cast<int>(slot), 1,
                static_cast<eItemDelReason>(3), true);
            PacketGuard pg;                                 // ORIG 此处无 clear（同汇编）
            pg.put_header(1, 0x207);
            pg.put_byte(1);
            pg.put_short(slot);
            pg.finalize(true);
            user->Send(pg);
        }
        else
        {
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x207),
                                     static_cast<unsigned char>(resultValue & 0xff));
        }
        return 0;
    }
}