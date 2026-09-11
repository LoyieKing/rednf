// ORIG 0x0810fb34：按购买数遍历 m_ruleMails24 规则-邮件对发奖励邮件。
// user 为空仅记 LogManager 告警返回 false；命中规则且发信成功读
// mail.m_itemIdx 记日志并将返回置 false（ORIG 返回值语义：true=本轮未发放）。
// [b28d-solo-CEventScriptMng_entitiesB] AE（compare_tu 口径，97=97；sent 槽 -0x9）。
// 修法：P198① result 循环外预清零（ORIG movl $0x0,-0x10）；删 rules 引用缓存，
// 每轮重发 `m_script1c->m_ruleMails24` 取址链（P157/P183）；it->/ptEv 替
// (*it)./deEv 并对 verifyGiveBonusReward/process_action_send_mail 逐次重发（P167）。
bool CPurchaseCashItemBonus::sendPurchaseBonusReward(CUser* user, int count)
{
    if (user == 0)
    {
        LogManager::logFormat(1, "EventClassify.cpp",
            __PRETTY_FUNCTION__,
            0x755,
            "CPurchaseCashItemBonus::sendPurchaseBonusReward(pUser(NULL))");
        return false;
    }
    // ORIG local_14 在循环前预清零（movl $0x0,-0x10）⇒ 声明于循环外（P198①）。
    int result = 0;
    bool sent = true;                       // ORIG local_d
    CEventActionMng mng;                    // ORIG 循环外构造一次
    // ORIG 每次 begin()/end() 重发完整 `m_script1c->m_ruleMails24` 取址链
    // （无 rules 引用缓存，P157/P183）；迭代器访问走 operator->（ptEv）。
    for (std::vector<std::pair<PurchaseCashItemBonusScript::PurchaseBonusRule,
                               ::Action_SendMail> >::iterator it =
             static_cast<PurchaseCashItemBonusScript*>(m_script1c)
                 ->m_ruleMails24.begin();
         it != static_cast<PurchaseCashItemBonusScript*>(m_script1c)
                   ->m_ruleMails24.end();
         ++it)
    {
        if (verifyGiveBonusReward(it->first, count))
        {
            result = mng.process_action_send_mail(
                user, EVENT_MAIL_REF(it->second), true);
            if (result != 0)
            {
                LogManager::logFormat(1, "EventClassify.cpp",
                    __PRETTY_FUNCTION__,
                    0x763,
                    "CPurchaseCashItemBonus::sendPurchaseBonusReward(m_id:%d, charac_no:%d, reward:%d)",
                    user->get_acc_id(), user->getCurCharacNo(),
                    it->second.m_itemIdx);
                sent = false;
            }
        }
    }
    return sent;
}