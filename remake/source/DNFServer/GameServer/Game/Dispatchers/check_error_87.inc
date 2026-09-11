// ==== Dispatcher_StackableActionUse::check_error (ORIG 0x0825e332) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0825e332 orig/ours=127/128 ndiff-blocks=13
//   帧差 sub $70 vs $4 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x70,%esp | mov    %eax,-0x14(%ebp) | mov    -0x14(%ebp),%edx
int Dispatcher_StackableActionUse::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    MSG_STACKABLE_ACTION_USE& req = reinterpret_cast<MSG_STACKABLE_ACTION_USE&>(msg);

    // 背包校验和不一致仅记 hack 计数，不中断（ORIG 形态）
    if (user->getCurCharacInvenCheckSum() != req.m_10)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25b, 1, 0, 0);
    }
    if (user->CheckInTrade())
    {
        return 0xd8;
    }
    if (req.m_14 == 0)                                   // +0x14 action type 非零
    {
        return 0x17;
    }

    // [DispA6 P0] ORIG 无 slot 槽（0825e3f4 `movzwl 0xd(%eax),%eax; movswl
    //   %ax,%ebx` 直传 GetInvenSlot）也无 itemIdx 槽（0825e424 `mov -0x4f(%ebp),
    //   %eax; mov %eax,%ebx` 从 item 临时直读 +0x02 入 find_item）⇒ 两具名局部
    //   内联（帧 0x80→0x70，msg 槽 -0x1c→-0x14 归位）。
    // ORIG 0x0825e3xx：GetInvenSlot 用 getCurCharacInvenR、try_use_item 用
    // getCurCharacInvenRefR（两不同 accessor，不缓存为一处）。
    Inven_Item item = reinterpret_cast<const CInventory*>(
                          user->getCurCharacInvenR())
                          ->GetInvenSlot(
                              1, static_cast<int>(static_cast<short>(req.m_0d)));

    CItem* itemDef =
        G_CDataManager()->find_item(static_cast<int>(item.m_addInfo));
    if (itemDef == 0)
    {
        return 0x11;
    }
    // ORIG test %al 单字节检验（P3 收窄）：verify 通过返回 al==1
    if ((unsigned char)user->verifyStackableItemProcess(
            static_cast<StackableAction::Type>(static_cast<char>(req.m_14)),
            itemDef) != 1u)
    {
        return 0x17;
    }
    if (user->CheckCoolTimeItem(static_cast<unsigned long>(item.m_addInfo)))
    {
        return 0x13;
    }
    int ret = reinterpret_cast<const CInventory*>(
                  user->getCurCharacInvenRefR())
                  ->try_use_item(
                      static_cast<int>(static_cast<short>(req.m_0d)), 1,
                      static_cast<int>(static_cast<char>(req.m_0f)));
    if (ret != 0)
    {
        return ret;
    }
    return 0;
}