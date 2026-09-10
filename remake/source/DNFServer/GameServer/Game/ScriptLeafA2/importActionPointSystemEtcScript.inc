int CActionPointEtcParameter::importActionPointSystemEtcScript(
    const char* path, CActionPointEtcParameter& out)
{
    // [b28d-solo-ScriptLeafA2] 判词 = N（残余迭代器计数/EH 清理差；p167 110=112）。
    //   已修：①EOF 段改 const_iterator + 后置 ++ + const vector&（P96/P28）；
    //   ②itemList 缓冲改 c_str() 视图（p167 原 operator[] EXTRA/c_str MISSING）；
    //   ③vector<int>::push_back 改具名 int 左值（ORIG const& 版）；④find 比较改
    //   operator==（反向包裹，P222）。残余 EXTRA = _TodayRewardItem::~_TodayRewardItem
    //   ×2（4.4 对含非平凡析构局部恒布 EH 清理，ORIG 消除，N91 族）+ const_iterator
    //   operator->/operator[] const 各 1 计数差；语义等价。
    // [轴 B 2026-09-10 · a_S->U 清单件 = **N（表内配对假阳性）**] 清单行
    //   `orig_addr=0x08894bc1 setle` 与我方 `seta` 系按出现序号交叉配对。按地址自校：
    //   ORIG 0x08894bc1 = `addl $0x1,-0x20(%ebp); cmpl $0x4,-0x20(%ebp); setle`
    //   （days 循环尾测，有符号）↔ 我方 0x1d73 同构
    //   `addl $0x1,-0x1c(%ebp); cmpl $0x4,-0x1c(%ebp); setle`（已一致）；
    //   我方 seta（0x1b3c `… call size(); cmp -0x3c(%ebp),%eax; seta`）对应
    //   ORIG 0x08895564 `addl $0x1,-0xc(%ebp); mov -0xc(%ebp),%eax;
    //   cmp -0x10(%ebp),%eax; setb` —— 两侧同为**无符号序**（仅比较方向不同）⇒ 本就一致，
    //   **禁改**（P196）。实测该件 794/811（gap −17）不变，未动一行源码。
    std::string token;
    bool ok = false;

    out.ClearData();
    if (!loadRDARScriptFile("", path))
    {
        return 0;   // ORIG +0x04c 文件缺失静默返回 0
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            // EOF：把各组元素指针登记进 +0x18 引用表（ORIG 尾段循环）
            // p167：ORIG 用 const_iterator（转换构造 ×2）+ operator++(int) +
            // operator->/operator[] const ⇒ 按 P96/P28 改写。
            for (std::map<unsigned short, std::vector<CActionPointEx> >::const_iterator
                     it = out.m_actionPoints.begin();
                 it != out.m_actionPoints.end(); it++)
            {
                const std::vector<CActionPointEx>& vec = it->second;
                for (size_t i = 0; i < vec.size(); ++i)
                {
                    out.m_actionPointRefs.insert(
                        std::make_pair(it->first,
                                       static_cast<const CActionPointEx*>(&vec[i])));
                }
            }
            // ↑ [P330] ORIG EOF 正常出口：ebx=0
            return 0;
        }

        if (token == "[action point list]")
        {
            for (;;)
            {
                CActionPointEx ex;
                int v1 = ScanInt(&ok);
                if (!ok)
                {
                    break;                              // 回主循环
                }
                ex.m_field00 = static_cast<unsigned short>(v1);

                int v2 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ex.m_field2c = static_cast<unsigned char>(v2);

                int key = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                *reinterpret_cast<unsigned short*>(&ex.m_bytes02[0x0a]) =
                    static_cast<unsigned short>(key);              // +0x0c map 键

                int v4 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }

                *reinterpret_cast<int*>(&ex.m_bytes02[0]) = v4;   // +0x02 int（组索引）
                if (!IsValidActionGroupIndex((_ActionGroupIndex)v4))
                {
                    return 0x62;                        // ORIG 错误码 0x62
                }

                int v5 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x65;                        // 0x65
                }
                ex.m_bytes02[6] = static_cast<unsigned char>(v5);   // +0x08（后被覆写）

                for (int i = 0; i <= 4; ++i)            // days 区 B+0x06..+0x0a
                {
                    int day = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x6d;                    // 0x6d
                    }
                    if (day != 0)
                    {
                        ex.m_bytes02[4 + i] =
                            static_cast<unsigned char>(-static_cast<char>(day));
                    }
                }

                std::string itemList;
                if (!ScanStr(&itemList))
                {
                    return 0x76;                        // 0x76
                }
                {
                    // strtok_r 按 "," 切分 atoi 入 vector<int>（B+0x14）
                    char* context = 0;
                    // p167：ORIG 调 string::c_str()（我方原为 operator[]）。
                    char* buf = const_cast<char*>(itemList.c_str());
                    for (char* tok = strtok_r(buf, ",", &context); tok != 0;
                         tok = strtok_r(0, ",", &context))
                    {
                        // ORIG 调 push_back(int const&)（具名 int 局部），非右值版。
                        int value = atoi(tok);
                        ex.m_itemList.push_back(value);
                    }
                }

                std::string weekdayStr;
                if (!ScanStr(&weekdayStr))
                {
                    return 0x9c;                        // 0x9c
                }
                unsigned int weekday = 0;
                if (!ConvertWeekdayStringToInt(weekdayStr, weekday))
                {
                    weekday = 7;
                }
                ex.m_weekday = static_cast<int>(weekday);

                int b9 = ScanInt(&ok);
                if (!ok)
                {
                    return 0xa4;                        // 0xa4
                }
                ex.m_bytes02[7] = static_cast<unsigned char>(b9);   // +0x09（覆盖 days[3]）

                int b10 = ScanInt(&ok);
                if (!ok)
                {
                    return 0xa7;                        // 0xa7
                }
                ex.m_bytes02[0x0e] = static_cast<unsigned char>(b10);   // +0x10

                if (!ScanStr(&ex.m_str24))
                {
                    return 0xaa;                        // 0xaa
                }
                if (!ScanStr(&ex.m_str28))
                {
                    return 0xad;                        // 0xad
                }

                std::map<unsigned short, std::vector<CActionPointEx> >::iterator fit =
                    out.m_actionPoints.find(*reinterpret_cast<unsigned short*>(&ex.m_bytes02[0x0a]));
                // p167：ORIG 调 _Rb_tree_iterator::operator==（非 !=）⇒ 反向包裹。
                if (fit == out.m_actionPoints.end())
                {
                    std::vector<CActionPointEx> vec;
                    vec.push_back(ex);
                    out.m_actionPoints.insert(
                        std::make_pair(
                        *reinterpret_cast<unsigned short*>(&ex.m_bytes02[0x0a]), vec));
                }
                else
                {
                    fit->second.push_back(ex);
                }
            }
        }
        else if (token == "[action point user message]")
        {
            for (;;)
            {
                int msgId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                std::string msg;
                if (!ScanStr(&msg))
                {
                    return 0xc7;                        // ORIG 错误码 199(0xc7)
                }
                out.m_rewardMessages.insert(std::make_pair(msgId, msg));
            }
        }
        else if (token == "[action point system message]")
        {
            for (;;)
            {
                int msgId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                std::string msg;
                if (!ScanStr(&msg))
                {
                    return 0xd5;                        // ORIG 错误码 0xd5
                }
                out.m_rewardMessages2.insert(std::make_pair(msgId, msg));
            }
        }
        else if (token == "[action point medal reward]")
        {
            for (;;)
            {
                _MedalRewardItem medal;
                medal.m_field00 = static_cast<unsigned short>(ScanInt(&ok));
                if (!ok)
                {
                    break;
                }
                medal.m_field02 = static_cast<unsigned short>(ScanInt(&ok));
                medal.m_field04 = ScanInt(&ok);
                medal.m_field08 = static_cast<unsigned short>(ScanInt(&ok));
                out.m_medalRewards.push_back(medal);
            }
        }
        else if (token == "[action point today reward]")
        {
            for (;;)
            {
                APSystem::_TodayRewardItem today;   // Wave5：表行=顶层类(0xe)
                int raw = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                today.m_itemIdx = raw - 1;
                if (!IsValidTodayRewardItem(*reinterpret_cast<const _SIG_LOAD_ACTION_POINT::_TodayRewardItem*>(&today)))  // 嵌套形参按值4B,首int同位(顶层0xe表行,ORIG import 五列中间态见 ScriptLeafA2 批)
                {
                    return 0xfa;                        // ORIG 错误码 0xfa
                }
                ScanInt(&ok);                           // 四个丢弃列（ORIG 存栈即弃）
                ScanInt(&ok);
                ScanInt(&ok);
                ScanInt(&ok);
                out.m_todayRewards.push_back(today);
            }
        }
    }
