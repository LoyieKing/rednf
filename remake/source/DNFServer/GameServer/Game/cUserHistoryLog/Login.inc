void cUserHistoryLog::Login(const char* ip, const char* param2, int param3, int param4,
                            unsigned char loginType, int param6)
{
    // 声明序=栈槽序（P28）：region/money 先声明得 -0x5c/-0x58 槽，余者顺延。
    int region = 0;                              // local_60 回归分类（ORIG 顶部零初始化）
    int money = 0;                               // ORIG 亦先零初始化后覆写（+0x019/+0x05d）
    int upgradeFirst = 0;                        // local_58（首件强化）
    int firstItemId = 0;                         // local_54（10 号槽 itemId）
    int upgradeSum = 0;                          // local_50（其余件强化和）
    int rareCount = 0;                           // local_4c（rarity 0/1 计数）
    int petCount = 0;                            // local_48（invenType=2 计数）

    money = reinterpret_cast<CInventory*>(
                reinterpret_cast<char*>(const_cast<_Charac_info*>(m_user->getCurCharacR()))
                + 0xda)->get_money();

    for (int slot = 10; slot < 0x14; ++slot)
    {
        // ORIG 每循环迭代重复 getCurCharacInvenR（P48 去缓存，禁循环外缓存）
        const Inven_Item* it = m_user->getCurCharacInvenR()->GetInvenRef(0, slot);
        // [b28d-solo-cUserHistoryLog 2026-09-09] ORIG 0x08683e19/0x08683e27 两守卫分落
        // +0x118 nop 岛 / +0x119 增量段，++rareCount 后 `jmp +0x119`；4.4 对 continue
        // 恒生成 nop+jmp 岛（多 1 条 jmp）。实测：拆两 continue（+2）、goto 多源共享尾标签
        // （ndiff 同 2）、循环2 改 || continue（+2）均不闭合 ⇒ N108/P223 封顶。
        if (it == 0)
            continue;
        if (it->m_addInfo != 0)
        {
            if (slot == 10)
            {
                upgradeFirst = it->GetUpgrade() & 0xff;
                firstItemId = it->m_addInfo;
            }
            else
            {
                upgradeSum += it->GetUpgrade() & 0xff;
            }
            CItem* item = G_CDataManager()->find_item(it->m_addInfo);
            if (item != 0)
            {
                // ORIG 单次 get_rarity 缓存后判 0/1（P183 反向警告：二次访问须具名缓存）
                int rarity = item->get_rarity();
                if (rarity == 0 || rarity == 1)
                    ++rareCount;
            }
        }
    }
    for (int slot = 0; slot < 0x74; ++slot)
    {
        // ORIG 每循环迭代重复 getCurCharacInvenR（P48 去缓存）
        const Inven_Item* it = m_user->getCurCharacInvenR()->GetInvenRef(2, slot);
        if (it == 0)
            continue;
        if (it->m_addInfo != 0)
            ++petCount;
    }

    // +0x7b 处回归用户标记 = _Charac_info::m_field7b（CUser.h:546，int）。
    // ORIG 0x08683dxx 单条 `mov 0x7b(%eax),%eax`（具名成员 disp 折叠）；
    // 裸偏移 `*(int*)(char*+0x7b)` 使 4.4 发 `add $0x7b; mov (%eax),%eax` 两步（§4.3 禁裸偏移）。
    int returnUserFlag = m_user->getCurCharacR()->m_field7b;
    // ORIG 先取 GetFirstLogin 落字节槽(-0x39)再判（勿内联 bool 物化）
    unsigned char isFirstLogin = m_user->GetFirstLogin();
    if (returnUserFlag == 0 && isFirstLogin != 0)
        region = 0;
    else if (loginType == 0 && returnUserFlag != 0)
        region = 1;
    else if (loginType == 1 && returnUserFlag != 0)
        region = 2;
    else if (loginType == 2 && returnUserFlag != 0)
        region = 3;
    // else：ORIG 无 else 写，region 保持初值 0（勿补 -1）

    int expertJobLevel = 0;                      // local_3c
    STExpertJobScript* script =
        reinterpret_cast<STExpertJobScript*>(
            G_CDataManager()->GetExpertJobScript(m_user->GetCurCharacExpertJobType()));
    if (script != 0)
        expertJobLevel = script->GetLevel((unsigned)m_user->GetCurCharacExpertJobExp());

    m_user->LogHistory(
        "IP+,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%p",
        param2, param3, param4, (int)loginType, param6,
        m_user->get_charac_job(), m_user->get_charac_level(),
        (int)m_user->getCurCharFirstGrowType(), (int)m_user->getCurCharSecondGrowType(),
        region, money, upgradeFirst, firstItemId, upgradeSum, rareCount, petCount,
        m_user->GetCurCharacExpertJobType(), expertJobLevel,
        reinterpret_cast<void*>(m_user));
