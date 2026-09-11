// ============================================================================
// importGuildParameterScript —— ORIG 0x08978d48
// 报告 _lt_global_gt_/importGuildParameterScript.md；调用点 init.md +0x1d53：
//   importGuildParameterScript(this+0xa50c, "Etc/Guild.etc")。
// 返回 bool：仅当读到 [guild parameter end] 为 true；加载失败 / ScanType 失败 /
// [guild exp weight] 遇 m_field8==0 均返回 false（ORIG 一致）。
// ============================================================================
// [b28d-solo-InitScriptProducers_3 N 封顶] 507/505、P167 CLEAR(76/76)；残差 =
// 4.1 nop 占位岛 + 两步寻址伪影（详见文件头判词表）。
bool importGuildParameterScript(GuildParameterScript* script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    // ORIG 0x08978d77：`mov 0x8(%ebp),%eax; mov %eax,-0x34(%ebp)` —— 形参缓存
    // 进具名局部 p（P0），全部访问走 p->。
    GuildParameterScript* p = script;
    p->clear();

    std::string token;
    int dead30 = 0;                // ORIG -0x30 `movl $0`（dead 局部，P31/P232）
    bool done = false;

    while (true)
    {
        while (true)
        {
            while (true)
            {
                while (true)
                {
                    while (true)
                    {
                        if (!ScanType(token, true))
                            goto scan_end;

                        if (token == "[guild create cost]")
                        {
                            p->m_field0 = ScanInt((bool*)0);
                            continue;
                        }
                        break;
                    }
                    if (token == "[guild levelup cost]")
                    {
                        // 最多 17 行：level cost1 cost2；level>0x11 放弃余下行。
                        // ORIG 写序（asm +0x107/+0x126）：cost1→[level-1][1]、
                        // cost2→[level-1][0]，m_levelUpCost 首址 +0x5c。
                        int level = 0;      // ORIG -0x2c：循环外声明 + 预清
                        for (int count = 0; count < 0x11; ++count)
                        {
                            level = ScanInt((bool*)0);
                            if (level > 0x11)
                                break;
                            // ORIG 0x08978e3a..：ScanInt 结果直存数组（无 cost 具名局部）。
                            p->m_levelUpCost[level - 1][1] = ScanInt((bool*)0);
                            p->m_levelUpCost[level - 1][0] = ScanInt((bool*)0);
                        }
                        continue;
                    }
                    break;
                }
                if (token == "[guild point acquirement ratio]")
                {
                    p->m_field4 = ScanInt((bool*)0);
                    continue;
                }
                break;
            }
            if (token == "[guild exp prob]")
            {
                p->m_field8 = static_cast<char>(ScanInt((bool*)0));
                continue;
            }
            break;
        }

        if (token == "[guild exp weight]")
        {
            // asm +0x1c7：prob==0 时直接以 false 结束整个函数。
            if (p->m_field8 == 0)
                goto scan_end;
            // ORIG 0x08978e??：`addl $1,-0x18(%ebp)` + `movsbl` ⇒ 循环变量是
            // int（char 变量恒产 addb，与 ORIG 不符）。
            // ORIG 0x08978ffa 前 `movl $0,-0x24/-0x20/-0x8c/-0x1c`：rangeMin/
            // rangeMax/ratio/lv 均在 w 循环外声明并预清 0，循环内再赋值。
            int rangeMin = 0;
            int rangeMax = 0;
            int ratio = 0;
            int lv = 0;            // 保留预清：实测 505 vs 504（ORIG 507）不回退
            for (int w = 0; w < p->m_field8; ++w)
            {
                rangeMin = ScanInt((bool*)0);
                rangeMax = ScanInt((bool*)0);
                ratio = ScanInt((bool*)0);
                // asm +0x24d..+0x295：insert(make_pair(lv,ratio))，
                // .second==false 时终止内层循环。
                for (lv = rangeMin; lv <= rangeMax; ++lv)
                {
                    if (!p->m_map44.insert(std::make_pair(lv, ratio)).second)
                        break;
                }
            }
            continue;
        }

        if (token == "[guild mail]")
        {
            p->m_mail = ScanInt((bool*)0);
            continue;
        }
        if (token == "[guild level demand for agit]")
        {
            p->m_fieldC = ScanInt((bool*)0);
            continue;
        }
        if (token == "[guild exp book]")
        {
            // 固定 3 对 key/value 插入 m_map10（asm +0x36f..+0x3f3）。
            // ORIG 0x08979132 前 `movl $0,-0x94/-0x98`：key/value 循环外预清。
            int key = 0;
            int value = 0;
            for (int i = 0; i < 3; ++i)
            {
                key = ScanInt((bool*)0);
                value = ScanInt((bool*)0);
                p->m_map10.insert(std::make_pair(key, value));
            }
            continue;
        }
        if ("[guild level demand for buying guild exp book]" == token)
        {
            p->m_field28 = ScanInt((bool*)0);
            continue;
        }
        if ("[agit upgrade info]" == token)
        {
            STAgitUpgradeInfo entry;
            for (int i = 0; i < 5; ++i)
            {
                entry.m_field0 = ScanInt((bool*)0);
                entry.m_field4 = ScanInt((bool*)0);
                entry.m_field8 = ScanInt((bool*)0);
                entry.m_fieldC = ScanInt((bool*)0);
                p->m_vec2c.push_back(entry);
            }
            continue;
        }
        if ("[guild storage upgrade info]" == token)
        {
            STGuildStorageUpgradeInfo entry;
            while ((entry.m_field0 = ScanInt(&done), done))
            {
                done = false;
                entry.m_field4 = ScanInt((bool*)0);
                entry.m_field8 = ScanInt((bool*)0);
                entry.m_fieldC = ScanInt((bool*)0);
                entry.m_field10 = ScanInt((bool*)0);
                p->m_vec38.push_back(entry);
            }
            done = false;
            continue;
        }
        if ("[guild exp member weight]" == token)
        {
            int memberCount;
            double weight;
            while (((memberCount = ScanInt(&done)), done))
            {
                done = false;
                weight = ScanFloat((bool*)0);
                // ORIG 0x08979363：`pair<int,double>::pair<int&,double&>` 直构
                // （C1IRiRdEEOT_OT0_）而非 make_pair（P167 差集 EXTRA make_pair）。
                p->m_vecE4.push_back(std::pair<int, double>(memberCount, weight));
            }
            done = false;
            continue;
        }
        if ("[guild levelup price]" == token)
        {
            int guildLevel;
            while (((guildLevel = ScanInt(&done)), done))
            {
                done = false;
                std::vector<int> prices;
                prices.reserve(0x11);
                for (int j = 0; j < 0x11; ++j)
                {
                    // ORIG 0x0897942e：push_back(const int&) —— 实参是具名左值
                    // 局部（先 `mov %eax,-0xd0(%ebp)` 再取址），非右值 prvalue。
                    int price = ScanInt((bool*)0);
                    prices.push_back(price);
                }
                // ORIG 0x089793??：pair<int,vector<int>>::pair<int&,vector<int>&> 直构。
                p->m_vecF0.push_back(
                    std::pair<int, std::vector<int> >(guildLevel, prices));
            }
            done = false;
            continue;
        }
        if ("[today guild member weight]" == token)
        {
            p->m_fieldFC = ScanFloat((bool*)0);
            continue;
        }
        if ("[guild fund condition]" == token)
        {
            p->m_field104 = ScanInt((bool*)0);
            p->m_field108 = ScanInt((bool*)0);
            p->m_field10c = ScanInt((bool*)0);
            p->m_field110 = ScanFloat((bool*)0);
            continue;
        }
        if (token == "[guild parameter end]")
        {
            done = true;
            break;
        }
    }

scan_end:
    return done;
}