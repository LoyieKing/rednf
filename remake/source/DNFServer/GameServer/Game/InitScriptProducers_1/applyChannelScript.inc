bool applyChannelScript(channel_script_t* script)
{
    std::string token;

    for (;;)
    {
        if (!ScanType(token, true))
            return 1;                                    // +0x778→+0x779 esi=1

        // b28d-lane25：P167 差集实证 ORIG 用 operator==（非 !=）且结构为
        // if==[dungeon]/else if==[server]/else{log;return 1}（0x088dc034 je +0x19a）。
        if (token == "[dungeon]")
        {
            // +0x05a..+0x136：两个 STR 后连读 int 至断流，逐条打日志并入表。
            std::string name;
            channel_dungeon_info_t info;

            if (!ScanStr(&name))
                return 1;                                // ebx=0 → esi=1
            if (!ScanStr(&info.m_keyName))
                return 1;

            bool more = true;
            while (true)
            {
                int value = ScanInt(&more);
                if (!more)
                    break;
                LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__,
                                      0x65, "[DUNGEON LIST] %s\t%d", name.c_str(), value);
                info.m_intList.push_back(value);
            }

            script->m_channelDungeonMap[name] = info;     // operator[] + operator=
            continue;                                     // ebx=1 → 主循环
        }

        // else if (token == "[server]")：+0x1b5..+0x736：先 serverNo；行循环
        // chNo→name→type→分类→bonusRate→float[10]；chNo 断流时把已积累 innerMap
        // 以 serverNo 提交到 +0x18 外层 map。
        else if (token == "[server]")
        {
        std::map<int, channel_info_t> channelInfos;
        int serverNo = 0;
        bool ok = false;

        serverNo = ScanInt(&ok);
        if (!ok)
            return 1;                                     // +0x71f ebx=0 → esi=1

        enum RowState { ROW_INSERT = 0, ROW_ERROR = 1, ROW_NEXT = 2 };

        for (;;)
        {
            channel_info_t info;
            RowState state;

            int chNo = ScanInt(&ok);
            if (!ok)
            {
                state = ROW_INSERT;                       // 频道段结束
            }
            else if (channelInfos.find(chNo) != channelInfos.end())
            {
                LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__,
                                      0x85,
                                      "importChannelScript - invalid channel number. %d already exists",
                                      chNo);
                state = ROW_ERROR;
            }
            else
            {
                if (!ScanStr(&info.m_name))
                {
                    LogManager::logFormat(0, "../RDARScriptChannel.cpp",
                                          __FUNCTION__, 0x8c,
                                          "importChannelScript - invalid channel name (requires STR)");
                    state = ROW_ERROR;
                }
                else
                {
                    info.m_channelType = ScanInt(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(0, "../RDARScriptChannel.cpp",
                                              __FUNCTION__, 0x9c,
                                              "importChannelScript - invalid channel type (requires INT)");
                        state = ROW_ERROR;
                    }
                    else if (!ScanStr(&info.m_dungeonName))
                    {
                        LogManager::logFormat(0, "../RDARScriptChannel.cpp",
                                              __FUNCTION__, 0xa3,
                                              "importChannelScript - invalid dungeon classification (requires STR)");
                        state = ROW_ERROR;
                    }
                    else
                    {
                        LogManager::logFormat(0, "../RDARScriptChannel.cpp",
                                              __FUNCTION__, 0xab,
                                              "[CHANNEL LIST] %d\t%s\t%d", chNo,
                                              info.m_dungeonName.c_str(), info.m_channelType);

                        if (info.m_dungeonName != "[none]")
                        {
                            // b28d-lane25：P167 差集实证 ORIG 走 iterator::C1Ev + find 两段
                            // （P50 反例复活：先声明后赋值，禁初始化式合并）。
                            std::map<std::string, channel_dungeon_info_t>::iterator dit;
                            dit = script->m_channelDungeonMap.find(info.m_dungeonName);
                            if (dit == script->m_channelDungeonMap.end())
                            {
                                LogManager::logFormat(
                                    0, "../RDARScriptChannel.cpp", __FUNCTION__, 0xb6,
                                    "importChannelScript - invalid dungeon classification. %s does not exist",
                                    info.m_dungeonName.c_str());
                                state = ROW_ERROR;
                            }
                            else
                            {
                                // +0x4ec..+0x524：+0x10 存分类区间 vector 地址（别名非拷贝），
                                // +0x0c 复制其键名字符串。
                                info.m_pBonusRates = &dit->second.m_intList;
                                info.m_dungeonKey = dit->second.m_keyName;
                            }
                        }

                        if (state != ROW_ERROR)
                        {
                            // +0x14 字节槽（ORIG 为 4 字节写入，高位属填充不参与读取）
                            info.m_bonusRate = (unsigned char)ScanInt(&ok);
                            if (!ok)
                            {
                                LogManager::logFormat(0, "../RDARScriptChannel.cpp",
                                                      __FUNCTION__, 0xc3,
                                                      "importChannelScript - invalid dungeon bonus rate. (required INT)");
                                state = ROW_ERROR;
                            }
                            else
                            {
                                bool rowOk = true;
                                for (int i = 0; i < 10; ++i)
                                {
                                    float percent = ScanFloat(&ok);   // 先存后检（fstps 序）
                                    info.m_gridPercent[i] = percent;
                                    if (!ok)
                                    {
                                        LogManager::logFormat(
                                            0, "../RDARScriptChannel.cpp", __FUNCTION__,
                                            0xcc,
                                            "importChannelScript - invalid level GridPercent(%d). (required Float)",
                                            i);
                                        rowOk = false;
                                        break;
                                    }
                                }
                                if (rowOk)
                                {
                                    channelInfos[chNo] = info;
                                    state = ROW_NEXT;
                                }
                                else
                                {
                                    state = ROW_ERROR;
                                }
                            }
                        }
                    }
                }
            }

            if (state == ROW_NEXT)
                continue;                                 // 下一频道行

            if (state == ROW_INSERT)
            {
                script->m_channelMap.insert(               // +0x18 外层 map
                    std::make_pair(serverNo, channelInfos));
                break;                                     // ebx=2 → 主循环
            }
            return 0;                                      // ROW_ERROR → esi=0
        }
        }
        else
        {
            LogManager::logFormat(0, "../RDARScriptChannel.cpp", __FUNCTION__,
                                  0xef,
                                  "importChannelScript - invalid type. %s is not acceptable",
                                  token.c_str());
            return 1;                                      // +0x776→+0x779 esi=1
        }
    }
}
