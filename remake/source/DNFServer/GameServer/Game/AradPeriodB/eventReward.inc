bool ARAD::Arad_EventPeriodDataManager::eventReward(ENUM_REPEAT_EVENT_CODE eventCode,
                                                    unsigned int time,
                                                    CUser* user)
{
    unsigned int curTime;      // ORIG 0x819acfa：time 具名缓存槽（-0xc）

    // 入口门禁：user 空 或 当前角色编号为 0 → 直接失败返 false（ORIG 0xabd0..0xac07）。
    if (user == 0 || user->get_charac_no(-1) == 0)
        return false;

    F fn(user);                                        // ORIG call C1(user)
    EventRewardItemArad data = fn.make_data((int)eventCode);
    if (isRewarded(data))                              // ORIG 0x081958b6（按值传参）
        return false;

    typename std::vector<SCRIPT::SRewardPeriodData>::iterator it =
        std::find_if(m_rewardPeriodList.begin(), m_rewardPeriodList.end(),
                     SearchEventPeriod(eventCode));    // ORIG SearchEventPeriodC1
    // ORIG 生成 __gnu_cxx::operator!=（find_if 尾部 it != end 判定）
    if (it != m_rewardPeriodList.end())
    {
        curTime = time;
        // ORIG 0x819ad0b：elem+0xc(begin) <= time && elem+0x10(end) > time
        if (it->m_beginTime <= curTime && it->m_endTime > curTime)
        {
            // ORIG 0x819ad77：fn(time, elem+4(itemIndex), sw(elem+8)(count), elem+10(endTime))
            if (fn((int)eventCode, it->m_field4, it->m_field8, it->m_endTime))
            {
                addUserRewardData(user, data);         // ORIG 0x08195aa4（引用重载）
                return true;
            }
        }
    }
    return false;
}