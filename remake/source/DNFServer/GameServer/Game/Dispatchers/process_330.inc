// ==== Dispatcher_SeriaRoom_Ani_Deco_Event::process (ORIG 0x081e886a) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e886a orig/ours=187/199 ndiff-blocks=48
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x9c,%esp | mov    %eax,-0x2c(%ebp) | mov    %eax,-0x28(%ebp)
int Dispatcher_SeriaRoom_Ani_Deco_Event::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_SERIAROOM_DECO_EVENT& req = reinterpret_cast<MSG_SERIAROOM_DECO_EVENT&>(msg);
    stSeriaRoomDecoEvent& res = static_cast<stSeriaRoomDecoEvent&>(param);
    // [L7DispG2 负结论] P92 试形：删 err 局部直写 res.m_4 并被后续重读
    //   （ORIG 0x081e889b 形）实测反劣化 199→203（4.4 每次重读要
    //   `mov -0x28(%ebp),%eax; mov 0x4(%eax),%eax` 2 条 > 具名槽 1 条读回）⇒ 回滚。
    int err = check_error(user, msg);
    res.m_4 = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(
            0x6551,
            __PRETTY_FUNCTION__,
            err, 0);
    }

    seriaRoom_AniDeco info = user->getSeriaRoomAniDecoInfo();   // +0x08d（4 字节值返回）

    // 进度上限：info.m_field2(有符号 word) >= maxAniDecoItem() → 0x14
    // （ORIG cmp %ax,%bx / setge：双方取低 16 位有符号比较）
    if (static_cast<short>(info.m_field2) >=
        static_cast<short>(G_CDataManager()->maxAniDecoItem()))
    {
        res.m_4 = 0x14;
        return 0;
    }

    // 客户端指定槽位 = msg+0xe word（即 read 写入的 req.m_pad+1）
    unsigned short slot =
        req.m_decoEventId;

    const CInventory* invenR = user->getCurCharacInvenR();
    Inven_Item slotItem = invenR->GetInvenSlot(static_cast<int>(INVEN_TYPE(1)),
                                               static_cast<int>(slot));

    // find_item(item idx) 缺定义 → 1
    CItem* itemDef = G_CDataManager()->find_item(slotItem.m_addInfo);
    if (itemDef == 0)
    {
        res.m_4 = 1;
        return 0;
    }

    // info.m_field2 == -1 哨兵 → 1（ORIG movzwl -0x2e / cmpw $0xffff）
    if (info.m_field2 == -1)
    {
        res.m_4 = 1;
        return 0;
    }

    // 槽位有效性校验：(idx=info.m_field2, value=itemIdx)；假 → 0x17
    // （ORIG xor $1,%eax：真继续、假报错）
    if (!G_CDataManager()->invalidAniDecoItem(
            static_cast<int>(static_cast<short>(info.m_field2)), slotItem.m_addInfo))
    {
        res.m_4 = 0x17;
        return 0;
    }

    // 成功路径：回显槽位 → 删除道具 → 进度+1 → 落库 → 事件处理
    res.m_8 = static_cast<short>(slot);                         // +0x18d

    CInventory* invenW = user->getCurCharacInvenW();
    invenW->delete_item(INVEN_TYPE(1), static_cast<int>(slot), 1,
                        static_cast<eItemDelReason>(3), true);

    seriaRoom_AniDeco updated = info;                           // m_field0 不变
    updated.m_field2 =
        static_cast<short>(static_cast<unsigned short>(info.m_field2 + 1));
    user->setSeriaRoomAniDecoInfo(updated);

    DB_UpdateSeriaRoomAnimationDeco::makeRequest(
        user->get_acc_id(), user->GetServerGroupToPvP(),
        user->getCurCharacNo(), info.m_field0, updated.m_field2);

    // [R3a01 修复] ORIG 0x081e8ac1 `mov (%eax),%edx` 直接解引用 GetRepeatEvent(0x69)
    //   返回值，**无** NULL 检查（Ghidra C 同证 (**(code**)(*piVar11+0x34))(piVar11,0)）
    //   ⇒ 删除我方多余 `evt != 0 &&` 守卫并去 eventMgr 槽（P0 无槽往返），保持 ORIG 行为
    //   （NULL 输入下 ORIG 崩溃；用户裁定完全复刻 ORIG 逻辑）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x69)->IsEventing(0)) // vtable +0x34 槽
    {
        CEventBase* aniDecoEvent = GlobalData::s_event_manager->GetRepeatEvent(0x69);
        if (aniDecoEvent != 0)
        {
            res.m_4 = static_cast<SeriaRoomAniDecoEvent*>(aniDecoEvent)
                          ->processEvent(*user);
        }
    }
    return 0;
}