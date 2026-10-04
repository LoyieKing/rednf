// [b28d-lane2] verdict=DIFF ndiff=150（ORIG 0x084e71ee）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=66（ORIG 0x84e71ee）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
int Inter_CollectItemsReward::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_CollectItemsRewardPktView* pPkt = reinterpret_cast<Inter_CollectItemsRewardPktView*>(data);
    if (user->getCurCharacR() == 0)
        return 0;

    Inven_Item item;                                   // ORIG Inven_Item::Inven_Item()
    // ORIG 0x084e7212 `movl $0x0,-0x1c(%ebp)` 落于 msgBuf 清零（lea -0x18b/mov $0x100,%ebx）
    // **之前** ⇒ 原码在缓冲前有一枚「初始化式 = 0」的具名 int 局部（P198① 预清必写；4.4 对
    // 未读但带初始化式的局部仍发 store）。语义未被 ORIG 读取，照抄 P171。
    // 残：槽位（ORIG itemCount -0x1c 浅于 pPkt -0x20；4.4 先声明深 ⇒ 本序给我方 -0x24/-0x20）
    //   = N82/P212 槽族；两次声明位置实验（前置于 pPkt：pPkt -0x20 达阵但 store 提前到函数首）
    //   已留痕，取 store 位置正确形。
    int itemCount = 0;
    char msgBuf[0x100] = { '\0' };                     // P4 内联清零
    char titleBuf[0x1e] = { '\0' };

    switch (pPkt->m_type)
    {
    case 1:                                            // 收集完成
        item.m_addInfo = 0x21d7;
        item.m_addInfo2 = 1;
        OS_API::snprintf(
            msgBuf, 0xff,
            g_scriptStringManager_.findString(4, "game_server_msg_257", 0),
            user->getCurCharacName());
        break;
    case 2:                                            // 兑换完成
        item.m_addInfo = 0x68c8;
        item.m_addInfo2 = 1;
        OS_API::snprintf(
            msgBuf, 0xff,
            g_scriptStringManager_.findString(4, "game_server_msg_259", 0),
            user->getCurCharacName());
        break;
    case 0:                                            // 收集进行中
        item.m_addInfo = 0x21d7;
        item.m_addInfo2 = 1;
        OS_API::snprintf(
            msgBuf, 0xff,
            g_scriptStringManager_.findString(4, "game_server_msg_258", 0),
            user->getCurCharacName());
        break;
    default:                                           // ORIG 保持 item 字段未设置
        break;
    }

    strncpy(titleBuf, g_scriptStringManager_.findString(4, "game_server_msg_256", 0),
            0x1d);

    CItem* found = G_CDataManager()->find_item(item.m_addInfo);
    if (found != 0)
    {   // ORIG vptr+0x08 槽 = CItem::make_item(Inven_Item&)（P5/P62）
        found->make_item(item);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            titleBuf, item, 0, (unsigned int)user->getCurCharacNo(), msgBuf,
            strlen(msgBuf), 0, (ENUM_SERVER_GROUP)user->GetServerGroup(), true, false);
        cMyTrace(__PRETTY_FUNCTION__,
            0x69c0, 0)("[collect items]reward: m_id(), charac_no(), reward()",
              (int)user->get_acc_id(), (int)user->getCurCharacNo(),
              (int)item.m_addInfo);
    }
    return 0;
}