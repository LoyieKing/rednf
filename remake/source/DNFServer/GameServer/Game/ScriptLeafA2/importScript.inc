// ============================================================================
// StrikerScript::importScript —— ORIG T 0x08a9ddb2
// 报告 StrikerScript/importScript.md 全文转写。成功置 m_loaded=1 并返回 true。
// ============================================================================
bool StrikerScript::importScript(const char* path)
{
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82）→ 试=①n1_v 复核 497/498（gap −1）；
    //   ②prett（q3_gap2 修正判据）③strall 均 CLEAR；④ORIG 0x08a9e1cd 段
    //   `mov 0x8(%ebp),%eax; lea 0x78(%eax),%edx; lea -0x5c(%ebp),%eax;
    //   mov %eax,0x4(%esp); mov %edx,(%esp); call find`（先算对象地址、lea 形）vs 我方
    //   `lea -0x5c(%ebp),%eax; …; mov 0x8(%ebp),%edx; add $0x78,%edx`（先算实参、add 形）
    //   = 求值序 + 地址计算形，4.4 前端固定，源码不可控（具名 int/int& 中转旧实测 +2）。
    //   ⇒ 维持 N82。
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

    // [b28d-L4a 2026-09-11] ORIG 0x08a9de09 段为 **top-tested** 形：
    // `+0x051 jmp +0x57 / +0x054 jmp +0x57`（双空块入口）→ +0x57 cond →
    // `xor $1; test %al; jne +0x6be`（**失败跳 END**）→ body → 尾部 `jmp +0x57`
    // ⇒ 原码是 `for (;;) { if (!ScanType(...)) break; ... }`，非 `while(...)`
    // （我方旧式产 bottom-tested `jmp cond / body / cond: jne body`）。
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }
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
            // ORIG 0x08a9dfc0 段同形：`xor $1; test; je <body>` + fall-through
            // `nop; jmp +0x57`（失败 break 回外层 cond）⇒ top-tested。
            for (;;)
            {
                if (!ScanStr(&s))
                {
                    break;
                }
                m_moduleImpossible.push_back(
                    static_cast<ContentsType::T>(ContentsType::getType(s.c_str())));
            }
        }
        else if ("[striker dungeon impossible]" == token)
        {
            m_dungeonImpossible.clear();
            // ORIG 0x08a9e035/0x08a9e053 段同形（失败 break 回外层 cond）。
            for (;;)
            {
                if (!ScanInt(&num))                 // ORIG 复用 -0x48
                {
                    break;
                }
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
                // ORIG 0x08a9e19d..0x08a9e1b4 逐条：`mov -0x30,%eax; mov %ax,-0x5c`
                // （+0x00 写 2 字节）→ `mov -0x2c,%eax; mov %ax,-0x5a`（+0x02 写 2 字节）
                // → `lea -0x5c(%ebp),%edx; mov %edx,0x8(%esp)` 传该 4 字节对象**地址**给
                // find（无 32 位读）⇒ 原码 = 4 字节具名结构（两 ushort 成员，成员必相邻）
                // + 以 int 引用传参。（旧位移/或打包在 4.4 多出 movzwl/shl/or 链 +3 条；
                // 两个独立 ushort 局部实测 4.4 不保证相邻 ⇒ 槽布局错。）
                struct StrikerSkillKey
                {
                    unsigned short lo;      // +0x00
                    unsigned short hi;      // +0x02
                };
                StrikerSkillKey keyParts;
                keyParts.lo = static_cast<unsigned short>(a);
                keyParts.hi = static_cast<unsigned short>(b);
                // [b28d-L4a 2026-09-11] 负结论：具名 `int& keyRef = *(int*)&keyParts`
                // 实测 4.4 为引用发额外槽（our 499，+2）⇒ 保持直接 (int*)&keyParts
                // 表达式形（our 497 = ORIG 497）。
                std::map<int, std::vector<StrikerSkill>*>::iterator it =
                    m_skillMap.find(*reinterpret_cast<int*>(&keyParts));
                // p167：ORIG 调 _Rb_tree_iterator::operator==（非 !=）⇒ 反向包裹。
                if (it == m_skillMap.end())
                {
                    std::vector<StrikerSkill>* vec = new std::vector<StrikerSkill>;
                    vec->push_back(skill);
                    m_skillMap[*reinterpret_cast<int*>(&keyParts)] = vec;
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
            for (;;)
            {
                if (!ScanStr(&s))
                {
                    break;
                }
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
}