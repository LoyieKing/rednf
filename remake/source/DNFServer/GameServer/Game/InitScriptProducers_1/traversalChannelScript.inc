void traversalChannelScript(channel_script_t* script)
{
    char buf[0x200];

    // b28d-lane25：P167 差集实证 ORIG 增量段走 ppEi（后置++，P28 增量侧推论）+
    // 各迭代器独立 C1Ev 且三条 C1Ev 在函数头连续发射（ORIG 0x088dc846..0x088dc86c，
    // 槽 -0x40/-0x44/-0x48）⇒ 三个迭代器一律"先声明后赋值"且全部声明在函数头，
    // 声明序 it → inner → outer。
    std::map<std::string, channel_dungeon_info_t>::iterator it;
    std::map<int, channel_info_t>::iterator inner;
    std::map<int, std::map<int, channel_info_t> >::iterator outer;
    // b28d-lane25 复测：ORIG 两个内层循环下标槽 -0x20（dungeon 段）/ -0x1c（channel
    // 段）——块内 for-init 声明在 4.4 下槽方向相反（P212），提至函数头按 ORIG 槽深序
    // 声明即可复现。
    unsigned int jDungeon;
    unsigned int jChannel;

    for (it = script->m_channelDungeonMap.begin();
         it != script->m_channelDungeonMap.end(); it++)
    {
        snprintf(buf, sizeof(buf), "[DUNGEON] %s as %s : ",
                 it->second.m_keyName.c_str(), it->first.c_str());
        for (jDungeon = 0; jDungeon < it->second.m_intList.size(); ++jDungeon)
            snprintf(buf, sizeof(buf), "%s%d ", buf, it->second.m_intList[jDungeon]);
        LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__, 0x107, buf);
    }

    for (outer = script->m_channelMap.begin();
         outer != script->m_channelMap.end(); outer++)
    {
        LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__, 0x10c,
                              "[CHANNEL] Server %d", outer->first);
        for (inner = outer->second.begin();
             inner != outer->second.end(); inner++)
        {
            channel_info_t& ci = inner->second;
            snprintf(buf, sizeof(buf), "\t%d : %s type(%d) name(%s) tag(%s) ",
                     inner->first, ci.m_name.c_str(), ci.m_channelType,
                     ci.m_dungeonKey.c_str(), ci.m_dungeonName.c_str());
            if (ci.m_pBonusRates != 0)
            {
                for (jChannel = 0; jChannel < ci.m_pBonusRates->size(); ++jChannel)
                    snprintf(buf, sizeof(buf), "%s%d ", buf, ci.m_pBonusRates->at(jChannel));
            }
            LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__, 0x11a,
                                  buf);
        }
    }
}
