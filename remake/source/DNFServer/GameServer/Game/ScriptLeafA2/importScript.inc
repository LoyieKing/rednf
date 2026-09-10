bool StrikerScript::importScript(const char* path)
{
    // [b28d-solo-ScriptLeafA2] 判词 = N（帧/槽形差，N82；p167 CLEAR 87=87）。
    //   已修：19 处 token 比较改「字面量在左」（P196 族）+ m_skillMap.find 比较改
    //   operator==（反向包裹，P222）。
    //   本波：①引入函数级 int num（ORIG -0x48）供 6 个分支复用——[tag mode]
    //   （0x08a9de49 死写入照抄，P171）、[tag cost time]（0x08a9de70/79 两段中转）、
    //   [striker start time]（0x08a9def9/df02）、[striker dungeon impossible]
    //   （0x08a9e035/53 while(ScanInt(&num))）、[striker skill]（0x08a9e10d）、
    //   [striker skill macro]（0x08a9e2e8）；②[striker skill] 还原 5 个具名 int
    //   局部 a..e（ORIG -0x30..-0x20，0x08a9e129..0x08a9e198 先取值后逐字段拷进
    //   skill，帧差 0x10 的来源）。
    //   读数（本波 fresh）：orig 490 / our 487（gap 21→10→3），ndiff 60→57。
    //   残余 = ①ORIG-only {xor:4, je:3} vs OURS {jne:3}：ORIG 0x08a9e040
    //   `xor $1; test %al,%al; je <body>` 为 4.1 对 `while(ScanInt(&num))`/`if(!ok)`
    //   的跨版本 bool 归一化（4.4 恒折 `test;jne` 顶掉 xor）⇒ Main 已定 N 族
    //   （P292/P168a③，勿恋战）；②OURS {movzwl:2, shl:1, or:1} = [striker skill]
    //   key 打包 `(ushort)a | (uint)(ushort)b<<16` 的零扩展链（ORIG 用形参槽直读）。
    //   语义等价（p167 CLEAR 87=87）。
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;
    std::string s;
    bool ok = false;
    // ORIG 0x08a9dde5 段：`movb $0x0,-0x41(%ebp)`（ok 预清）后紧跟
    // `movl $0x0,-0x48(%ebp)`（num 预清）⇒ 两句式带初值（P198①）。
    int num = 0;                                    // ORIG -0x48：多分支复用（P171 死写入照抄）

    while (ScanType(token, true))
    {
        if ("[tag mode]" == token)
        {
            num = ScanInt(&ok);                     // 仅存局部、后续不读（ORIG 0x08a9de49 死写入）
        }
        else if ("[tag cost time]" == token)
        {
            num = ScanInt(&ok);                     // ORIG 0x08a9de70/0x08a9de79 两段中转
            m_tagCostTime = num;
        }
        else if ("[tag character setting]" == token)
        {
            m_tagCharacterSetting = ScanInt(&ok);
        }
        else if ("[striker combo]" == token)
        {
            m_strikerCombo = ScanInt(&ok);
        }
        else if ("[striker start time]" == token)
        {
            num = ScanInt(&ok);                     // ORIG 0x08a9def9/0x08a9df02 两段中转
            m_strikerStartTime = num;
        }
        else if ("[striker skill data up]" == token)
        {
            importSkillDataUpVector();
        }
        else if ("[striker skill attack penalty]" == token)
        {
            importSkillAttackPenalty();
        }
        else if ("[striker skill cooltime penalty]" == token)
        {
            importSkillCoolTimePenalty();
        }
        else if ("[striker level penalty]" == token)
        {
            importParentLevelPenlaty();
        }
        else if ("[striker module impossible]" == token)
        {
            m_moduleImpossible.clear();
            while (ScanStr(&s))
            {
                m_moduleImpossible.push_back(
                    static_cast<ContentsType::T>(ContentsType::getType(s.c_str())));
            }
        }
        else if ("[striker dungeon impossible]" == token)
        {
            m_dungeonImpossible.clear();
            while (ScanInt(&num))                   // ORIG 复用 -0x48（0x08a9e035/0x08a9e053）
            {
                m_dungeonImpossible.push_back(num);
            }
        }
        else if ("[striker end time]" == token)
        {
            m_strikerEndTime = ScanInt(static_cast<bool*>(0));
        }
        else if ("[position]" == token)
        {
            m_positionType = ScanInt(static_cast<bool*>(0));   // +0xac
            m_field38 = ScanInt(static_cast<bool*>(0));        // +0x38
            m_field3c = ScanInt(static_cast<bool*>(0));        // +0x3c
        }
        else if ("[striker skill]" == token)
        {
            for (;;)
            {
                StrikerSkill skill;
                if (!ScanInt(&num))                 // ORIG 复用 -0x48（0x08a9e10d）
                {
                    break;                              // 扫描失败退出该段
                }
                // ORIG 0x08a9e129..0x08a9e198：5 个具名 int 局部（-0x30..-0x20）
                // 先取值，再逐字段拷进 skill（帧差 0x10 的来源）。
                int a = num;                        // -0x30（0x08a9e129）
                int b = ScanInt(static_cast<bool*>(0));      // -0x2c
                int c = ScanInt(static_cast<bool*>(0));      // -0x28
                int d = ScanInt(static_cast<bool*>(0));      // -0x24
                int e = ScanInt(static_cast<bool*>(0));      // -0x20
                ScanStr(&s);
                skill.m_field00 = c;                // -0x58（0x08a9e176）
                skill.m_field04 = d;                // -0x54
                skill.m_field08 = e;                // -0x50
                skill.m_name = s;                   // -0x4c（0x08a9e198 operator=）
                // map 键 = 两个截断 ushort 打包（栈 -0x5c/-0x5a 对）
                int key = static_cast<int>(
                    static_cast<unsigned short>(a) |
                    (static_cast<unsigned int>(static_cast<unsigned short>(b))
                     << 16));
                std::map<int, std::vector<StrikerSkill>*>::iterator it =
                    m_skillMap.find(key);
                // p167：ORIG 调 _Rb_tree_iterator::operator==（非 !=）⇒ 反向包裹。
                if (it == m_skillMap.end())
                {
                    std::vector<StrikerSkill>* vec = new std::vector<StrikerSkill>;
                    vec->push_back(skill);
                    m_skillMap[key] = vec;
                }
                else
                {
                    it->second->push_back(skill);
                }
            }
        }
        else if ("[striker skill macro]" == token)
        {
            m_macroCommands.clear();
            for (;;)
            {
                if (!ScanInt(&num))                 // ORIG 复用 -0x48（0x08a9e2e8）
                {
                    break;
                }
                StrikerSkillAddCommand cmd;
                cmd.m_field00 = num;
                cmd.m_field04 = ScanInt(static_cast<bool*>(0));
                cmd.setType(ScanInt(static_cast<bool*>(0)));
                m_macroCommands.push_back(cmd);
            }
        }
        else if ("[common cool time]" == token)
        {
            m_commonCoolTime = ScanInt(static_cast<bool*>(0));
        }
        else if ("[direction type]" == token)
        {
            m_direction.setType(ScanInt(static_cast<bool*>(0)));
        }
        else if ("[contents revision]" == token)
        {
            m_contentsRevision.clear();
            while (ScanStr(&s))
            {
                ContentsRevision rev;
                rev.m_type = ContentsType::getType(s.c_str());
                rev.m_field04 = ScanInt(static_cast<bool*>(0));
                rev.m_field08 = ScanInt(static_cast<bool*>(0));
                rev.m_field0c = ScanInt(static_cast<bool*>(0));
                m_contentsRevision.push_back(rev);
            }
        }
        else if ("[upgrade revision]" == token)
        {
            m_upgradeRevision = ScanInt(static_cast<bool*>(0));
        }
    }

    m_loaded = 1;
    return true;
