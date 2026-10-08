// [b28d-P330 int] ORIG 返回 = **错误行号 int**（callee 尾 `mov %ebx,%eax; ret`，
// ebx 写点含 `mov $0x57..$0xfa,%ebx` 14 处行号常量、0 引用 ×2；caller 4B 全
// store 无 movzbl）。旧 bool 声明把行号截成 0/1 ⇒ trace "%d line" 日志值错。
int CActionPointEtcParameter::importActionPointSystemEtcScript(
    const char* path, CActionPointEtcParameter& out)
{
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N91）→ 试=④P342（triviality 轴）：
    //   `APSystemTypes.h:48` 用户空析构使 `APSystem::_TodayRewardItem` 非平凡 ⇒ 4.4 对
    //   栈对象 `today` 布 EH 清理（p167 EXTRA `_TodayRewardItemD*Ev`×2）；删声明+定义后
    //   812→**792**（ORIG 794，gap −18→**+2**），D1/D2 仍由 odr-use 发射（nm W 定义）。
    //   本 TU 由 Q8OldNAgain 同轮落地核账（659/1087/0/13/319 → 661/1087/0/13/318）。
    //   余 +2 = ORIG 0x088949af/0x08894a2f 两个**死局部** `movl $0x0,-0x40/-0x38`（各仅
    //   1 次写、无使用，P232）；补同样 pad 可达 794=794 但帧 0x160→0x170 槽整体平移
    //   ⇒ 未采用。⇒ 部分推翻（N91 项已消），残余归 N82/N126。
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
            // [b28d-L4a 2026-09-11] ORIG 0x08894a0b `jne 0x08895481 (+0xaeb)`：
            // 扫描失败直接跳**函数尾部**的 EOF 段（非就地展开）⇒ 原码是
            // `for (;;) { if (!ScanType(...)) break; ... }`，EOF 登记段位于循环之后。
            break;
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
                ex.m_ap.m_actionNo = static_cast<unsigned short>(v1);

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
                ex.m_mapKey = static_cast<unsigned short>(key);    // +0x0c map 键

                int v4 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }

                ex.m_ap.m_groupIndex = v4;                        // +0x02 int（组索引）
                if (!IsValidActionGroupIndex((_ActionGroupIndex)v4))
                {
                    return 0x62;                        // ORIG 错误码 0x62
                }

                int v5 = ScanInt(&ok);
                if (!ok)
                {
                    return 0x65;                        // 0x65
                }
                // [DNF-FIX 2026-10-08 c31] ORIG 0x08894b67 `mov %al,-0x146` = ex(-0x154)+0x0e = m_field0e
                ex.m_field0e = static_cast<unsigned char>(v5);

                for (int i = 0; i <= 4; ++i)            // days 区 B+0x06..+0x0a
                {
                    int day = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x6d;                    // 0x6d
                    }
                    if (day != 0)
                    {
                        ex.m_ap.m_days[i] =
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

                // [DNF-FIX 2026-10-08 c31] ORIG 0x08894d5a `mov %al,-0x145` 紧跟 ScanInt（先存后判 ok）：
                //   -0x145 = ex+0x0f = m_levelMin（旧码误写 m_ap.m_days[3]）。
                ex.m_levelMin = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0xa4;                        // 0xa4
                }

                int b10 = ScanInt(&ok);
                if (!ok)
                {
                    return 0xa7;                        // 0xa7
                }
                ex.m_levelMax = static_cast<unsigned char>(b10);   // +0x10

                if (!ScanStr(&ex.m_str24))
                {
                    return 0xaa;                        // 0xaa
                }
                if (!ScanStr(&ex.m_str28))
                {
                    return 0xad;                        // 0xad
                }

                std::map<unsigned short, std::vector<CActionPointEx> >::iterator fit =
                    out.m_actionPoints.find(ex.m_mapKey);
                // p167：ORIG 调 _Rb_tree_iterator::operator==（非 !=）⇒ 反向包裹。
                if (fit == out.m_actionPoints.end())
                {
                    std::vector<CActionPointEx> vec;
                    vec.push_back(ex);
                    out.m_actionPoints.insert(
                        std::make_pair(
                        ex.m_mapKey, vec));
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
                // [DNF-FIX-AUDIT 2026-10-07] ORIG 0x0889541a..0x0889545d：四列分别
                //   `mov %ax,-0x120` / `mov %ax,-0x11e` / `mov %eax,-0x11c` / `mov %ax,-0x118`
                //   写入栈对象 today(-0x124) 的 +4/+6/+8/+0xc，随后 0x08895477
                //   push_back(&today)。旧注释「存栈即弃」误判：丢弃后 m_minAp/m_maxAp/
                //   m_rewardItemIdx/m_count 恒 0（GetTodayRewardAndSendMail 区间判定与
                //   奖励物品/数量全失效）。
                today.m_minAp = static_cast<unsigned short>(ScanInt(&ok));
                today.m_maxAp = static_cast<unsigned short>(ScanInt(&ok));
                today.m_rewardItemIdx = ScanInt(&ok);
                today.m_count = static_cast<unsigned short>(ScanInt(&ok));
                out.m_todayRewards.push_back(today);
            }
        }
    }

    // EOF：把各组元素指针登记进 +0x18 引用表（ORIG 0x08895481 尾段循环）
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