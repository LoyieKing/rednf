int Dispatcher_SeriaRoom_Deco_Event::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [b23S3C] ORIG 0x081e5cde：req(-0x18)/p(-0x14) 两引用先物化；check_error 直存
    // p.m_4（ORIG 2 条 `mov -0x14,%edx; mov %eax,0x4(%edx)`，无 err 槽 P0）；GCD/代理
    // 每用点重取（P48：ORIG 两次 getCurCharacInvenR/G_CDataManager call）；vaild 判定
    // ORIG `xor $1;test%al;je` ==1 直测（P194）；deleteFlag 预清两句式保留死块。
    MSG_SERIAROOM_DECO_EVENT& req = reinterpret_cast<MSG_SERIAROOM_DECO_EVENT&>(msg);
    stSeriaRoomDecoEvent& p = static_cast<stSeriaRoomDecoEvent&>(param);
    p.m_4 = check_error(user, msg);
    if (p.m_4 >= 1)
        return 0;
    if (p.m_4 < 0)
        return LineFunc(0x6010, __PRETTY_FUNCTION__, p.m_4, 0);
    Inven_Item item = user->getCurCharacInvenR()->GetInvenSlot(
        1, (unsigned int)req.m_decoEventId);
    if (G_CDataManager()->find_item(item.m_addInfo) == 0)
    {
        p.m_4 = 0x11;
        return 0;
    }
    if (G_CDataManager()->getVaildEventItem(item.m_addInfo) != 1)
    {
        p.m_4 = 0x17;
        return 0;
    }
    p.m_8 = req.m_decoEventId;
    bool deleteFlag = false;
    if (deleteFlag)
    {
        user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)1, (unsigned int)req.m_decoEventId, 1,
            (eItemDelReason)3, true);
    }
    return 0;
