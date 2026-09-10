bool CCirculationStatistic::AddCirculationStatistic(CIRCULATION_STATISTIC_FIELD field, CUser* user,
                                                       unsigned int value)
{
    // [b28d-34] 复核维持 DIFF：ORIG 0x0860daaa `test %al,%al` 直测 vs 我们
    // bool 槽 `mov %al,-0x19; cmpb`（P0 表达式直用/P168a③b 族）；语义等价。
    bool wrongIndex = !(field >= 0 && field < 0x30 &&
                        user->get_charac_level() > 0 &&
                        user->get_charac_level() < 0x47);
    if (wrongIndex)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x39b, 0)(
            "CIRCULATION_STATISTIC wrong index (%d,%d)", field, user->get_charac_level());
        return false;
    }

    std::map<ENUM_SERVER_GROUP, stCirculationStatistic>::iterator it =
        m_data.find(user->GetServerGroup());
    if (it == m_data.end())
    {
        stCirculationStatistic stat;
        memset(&stat, 0, 0x3540);
        stat.m_values[user->get_charac_level()][field] = value;
        m_data.insert(std::make_pair(user->GetServerGroup(), stat));
    }
    else
    {
        it->second.m_values[user->get_charac_level()][field] += value;
    }
    return true;
