int Dispatcher_UpgradeItemSeparate::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081cbe84（报告 Dispatcher_UpgradeItemSeparate/check_error.md 全文已读）：
    // !(state==3 && curCharac!=0) → 7（&& 短路 bool 物化，jne 双正向跳）；
    // msg 指针局部@-0x10（Check 前绑定）；secuResult@-0xc；!=0 直返；
    // CheckItemLock(1,(int)(ushort)(msg+0x1b)) 真 → 0xd5；假 → 0。
    // +0x1b 暂用具名 ushort 视图（MSG_UPGRADE_ITEM_SEPARATE 未建模，已上报 Main）。
    if (!((user->get_state() == 3) && (user->getCurCharacR() != 0)))
    {
        return 7;
    }
    // P80/P99：协议载荷视图必须 pack(1)，否则 ushort 被自然对齐推到 +0x1c。
    struct __attribute__((packed)) ReqView { char m_head[0x1b]; unsigned short m_itemIdx; };
    const ReqView* pReq = reinterpret_cast<const ReqView*>(&msg);   // ORIG -0x10
    int secuResult = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x27);
    if (secuResult != 0)
    {
        return secuResult;
    }
    if (user->CheckItemLock(1, (int)(unsigned short)pReq->m_itemIdx))
    {
        return 0xd5;
    }
    return 0;
