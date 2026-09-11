// ORIG 0x08110386：按已用疲劳档位发放事件物品邮件。
// 遍历 m_ruleMails1c（[eventitem send] 规则-邮件对），内层阈值列表任一 t 满足
// 「(当前已用疲劳 - 增量) < t 且 当前已用疲劳 >= t」即发该对邮件；
// 发信成功读 m_mail.m_itemIdx 记日志后返回（ORIG 单次发放语义）。
// 全程不缓存脚本指针（与 ORIG 同）。
// [b28d-solo-CEventScriptMng_entitiesB] AE（compare_tu 口径，138=138）。修法：
// P0 去 usedBase/threshold/hit 具名局部（ORIG 无槽往返）⇒ 帧 0x50→0x40；
// CEventActionMng mng 移入内层循环作用域复现 ORIG 槽布局（mng -0x21/it -0x1c/
// vit -0x20）；第二条件改写 `*vit <= used` 保左→右求值序（*vit→%ebx 先于
// getCurCharacUsedFatigue→%eax）；外层 it->/ptEv 替 (*it)./deEv（P167）。
void CObjectBringUp::sendEventItemFromFatigue(CUser* user, unsigned int fatigue)
{
    if (static_cast<ObjectBringUpScript*>(m_script1c)->m_ruleMails1c.empty())
        return;
    // P0：ORIG 无 usedBase 槽往返（movzwl; sub 0x10(%ebp),%eax; mov %eax,-0x10）
    // ⇒ 原码是内联右值，remaining 为唯一具名局部（32 位回绕忠实）。
    unsigned int remaining =
        ((unsigned int)user->getCurCharacUsedFatigue() & 0xffffu) - fatigue;
    for (std::vector<std::pair<std::vector<unsigned int>, ::Action_SendMail> >::iterator it =
             static_cast<ObjectBringUpScript*>(m_script1c)->m_ruleMails1c.begin();
         it != static_cast<ObjectBringUpScript*>(m_script1c)->m_ruleMails1c.end(); ++it)
    {
        // ORIG 外层访问走 operator->（ptEv），内层走 operator*（deEv）；
        // threshold 无具名槽（每次重发 *vit，P157/P183）。
        for (std::vector<unsigned int>::iterator vit = it->first.begin();
             vit != it->first.end(); ++vit)
        {
            CEventActionMng mng;
            // ORIG 第二条件先求 *vit（→%ebx）后求 getCurCharacUsedFatigue（→%eax）
            // ⇒ 写成 `*vit <= used` 保持左→右求值序（等价无符号比较）。
            if ((*vit > remaining) &&
                (*vit <= ((unsigned int)user->getCurCharacUsedFatigue()
                          & 0xffffu)))
            {
                int sent = mng.process_action_send_mail(
                    user, EVENT_MAIL_REF(it->second), true);
                if (sent != 0)
                {
                    LogManager::logFormat(1, "EventClassify.cpp",
                        __PRETTY_FUNCTION__,
                        0x803,
                        "CObjectBringUp::sendEventItemFromFatigue(m_id:%d, charac_no:%d, reward:%d)",
                        user->get_acc_id(), user->getCurCharacNo(),
                        it->second.m_itemIdx);
                    return;
                }
            }
        }
    }
}