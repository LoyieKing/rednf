// ============================================================================
// setEventData @ 0x08166752（setEventData.md 全文转写；定义归本 TU，
// 与 HeroMissionLink4.cpp 旧定义同符号，由 Main 整合去重）。
// 判活 → charac_no 非 0 → 空数据按条件表生成初始单（level==70 时任务 4/6
// 置完成态）；非空按 "," 切分逐 token setStringToken；最后
// find 已存在 → log(0x24b)+erase，再 insert(make_pair(charac_no, values))。
// ============================================================================
void HeroMissionEvent::setEventData(CUser* user, char const* data)
{
    // ORIG 0x08166775 `call *%edx`：虚调用 CEventBase::IsEventing(NULL)（§4.5）。
    if (IsEventing(0) == 0)
    {
        return;
    }
    if (user->get_charac_no(-1) == 0)
    {
        return;
    }

    HeroMissionValue value;
    std::vector<HeroMissionValue> values;
    std::string scriptData(data);

    if (scriptData.length() == 0)
    {
        // 空数据：按条件表生成初始值单（满级 70 时任务 4/6 直接置完成态）
        for (std::map<HeroMissionCondition::MissionNo::T,
                      BaseHeroMissionCondition*>::iterator cit =
                 m_conditions.begin();
             cit != m_conditions.end(); ++cit)
        {
            value.clear();
            value.m_mission = cit->first;
            if (cit->first == HeroMissionCondition::MissionNo::T_4 &&
                user->get_charac_level() == 70)
            {
                value.m_field4 = 1;
                value.m_field6 = 1;
                value.m_field8 = 70;
                value.m_fieldA = 1;
            }
            if (cit->first == HeroMissionCondition::MissionNo::T_6 &&
                user->get_charac_level() == 70)
            {
                value.m_field4 = 1;
            }
            values.push_back(value);
        }
    }
    else
    {
        // ORIG: boost::char_separator(",") 切分，逐 token setStringToken
        boost::char_separator<char> separator(",");
        boost::tokenizer<boost::char_separator<char> > tokens(scriptData,
                                                              separator);
        for (boost::tokenizer<boost::char_separator<char> >::iterator tit =
                 tokens.begin();
             tit != tokens.end(); ++tit)
        {
            // [b28d-solo-HeroMissionEvent] ORIG 0x08166a5c 走 boost
            // iterator_facade::operator->() + std::string::C1(char const*,
            // allocator const&) ⇒ 源码传 c_str()（隐式构造 string），非 *tit。
            value.setStringToken(tit->c_str());
            values.push_back(value);
        }
    }

    // [b28d-solo-HeroMissionEvent] ORIG 0x08166b37/0x08166ba0/0x08166be3/
    // 0x08166c0e 各独立调用 user->get_charac_no(-1)（无 characNo 具名局部，
    // 临时槽 -0x40/-0x38/-0xc；make_pair 的 T=int）。
    std::map<unsigned int, std::vector<HeroMissionValue> >::iterator it =
        m_values.find(user->get_charac_no(-1));
    if (it != m_values.end())
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x24b,
            "[HeroMission] Already exist character data. (charac:%u)",
            user->get_charac_no(-1));
        m_values.erase(user->get_charac_no(-1));
    }
    m_values.insert(std::make_pair(user->get_charac_no(-1), values));
}