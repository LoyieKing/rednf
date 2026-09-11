int CMission::Reward(CUser& user) const
{
    // [b28d-70] 判词：AE 124=124（m_rankRange.first 读 +0x18；P167 CLEAR 16=16）。
    // b28d-50：ORIG 0x085e32xx 对 get_BaseMissionExp_byRank 的结果先
    // `mov $0,%edx; mov %eax,-0x30; mov %edx,-0x2c; fildll` ⇒ 原码为
    // `(float)(unsigned long long)` 零扩展 64 位（int 恒 fildl）。
    // b28d-50：ORIG 先 fildll（call 结果）后 flds m_expRate（0x085e3352 段，
    // 无临时槽）⇒ 书写序为「call 结果 * m_expRate」；写成 m_expRate * call() 会
    // 把 m_expRate 先 fstps 到临时槽再 fmuls（P243 x87 序）。
    int exp = (int)((float)(unsigned long long)(unsigned int)
                        G_CDataManager()->get_BaseMissionExp_byRank(
                            (unsigned short)m_rankRange.first) * m_expRate);

    user.m_historyLog.pvpMissionClearReward(
        user.getPVPResultRefW()->m_expPoint, exp, GetMissionKind(),
        GetMissionIndex());
    user.getPVPResultRefW()->m_expPoint += exp;

    std::vector<int> outSlots;
    // b28d-50：ORIG 0x085e3393 在 insert_event_items call **前**发
    // `movl $0x0,-0xc(%ebp)`（ret 预清）⇒ 声明与赋值必须两句式（P198①）。
    int ret = 0;
    ret = ((CInventory*)user.getCurCharacInvenW())
              ->insert_event_items(m_rewardItems, outSlots,
                                   (eMoneyAddReason)0x1e,
                                   (eItemAddReason)0x33, true,
                                   "game_server_msg_173",
                                   "game_server_msg_174");
    if ((ret > 0) && (outSlots.size() > 2))
    {
        user.SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0,
                            outSlots[0]);
    }
    _Reward(user);
    return 1;
}