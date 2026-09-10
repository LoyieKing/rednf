int Dispatcher_ComboSkill::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_COMBO_SKILL& req = reinterpret_cast<MSG_COMBO_SKILL&>(msg);
    std::map<short, std::vector<int>, std::less<short>,
             std::allocator<std::pair<short const, std::vector<int> > > >&
        comboMap = req.m_comboMap;
    std::vector<int> skills;
    comboMap.clear();
    if (!packet.get_byte(req.m_pad0d))
    {
        return LineFunc(0x301,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char treeCount = 0;
    if (!packet.get_byte(treeCount))
    {
        return LineFunc(0x304,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    for (int i = 0; i < (int)treeCount && i < 6; ++i)
    {
        short kind = 0;
        char skillCount = 0;
        if (!packet.get_short(kind))
        {
            break;  // 归 LineFunc(0x30c)
        }
        if (checkComboSkillIndex(kind))
        {
            if (!packet.get_byte(skillCount))
            {
                return LineFunc(0x312,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            skills.clear();
            for (int j = 0; j < (int)skillCount && j < 6; ++j)
            {
                short skillIdx = 0;
                if (!packet.get_short(skillIdx))
                {
                    return LineFunc(0x319,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                skills.push_back((int)skillIdx);
            }
            comboMap.insert(std::make_pair(kind, skills));
        }
    }
    return LineFunc(0x30c,
                    __PRETTY_FUNCTION__,
                    0, 0);
