bool importEventUpgradeJarScript(EventUpgradeJarInfo* info, const char* path)
{
    // [b28d-solo-EventUpgradeJar] P140/P171：ORIG 0x089687d1 首条即
    // `loadRDARScriptFile("", path)`，无 info/path 空指针防御 ⇒ 删除多余守卫。
    if (!loadRDARScriptFile("", path))
        return false;

    // b28d-lane25：ORIG 0x089687d1 全文重写。要点：
    // ① 比较形 `"字面量" == token`（const char* 在左，P167 差集实证）；
    // ② 全部 ScanInt/ScanFloat 走 bool* 重载 `x = ScanInt(&ok); if (!ok)`；
    // ③ 两个 string 局部（ORIG C1Ev/D1Ev 各 1）；
    // ④ 主循环 for(;;) 各段尾回头部；失败 return false / EOF return true。
    // [b28d-solo-EventUpgradeJar] ORIG 扫描/比较用的是**第二个** string（-0x70，
    // 0x08968841/0x08968857 实证）；第一个 string（-0x6c）全程未被读取 ⇒ 声明序
    // unusedToken 在前、token 在后。id/recipeIndex 在函数头初始化（0x0896881a
    // `movl $0,-0x78`、0x08968821 `movl $0,-0x2c`），循环内 id 再显式重置。
    // [round5 复测] N95 反推：ORIG 全帧槽号比我方**统一深 4**（token -0x6c vs
    // -0x68、ok -0x71 vs -0x6d、id -0x78 vs -0x74、npc -0x80 vs -0x7c），而两侧
    // `sub` 同为 $0x90 ⇒ 差在槽基（4.1 槽基可顶到 ebp / 4.4 恒留 gap），非帧长。
    // 按 P232（4.4-O0 对未使用局部照样占栈）补 4B 死局部试把槽基整体下移 4。
    int slotBasePad;
    std::string unusedToken;   // ORIG 第一 string 局部（0x08968806 C1Ev / -0x6c，未读）
    std::string token;         // ORIG 第二 string 局部（0x08968811 C1Ev / -0x70）
    (void)unusedToken;
    bool ok = false;
    unsigned int id = 0;       // ORIG -0x78
    // [round5 反例] 同法在标量区（id 与 recipeIndex 之间）再补 4B pad 实测**反效**：
    // 深区被推深 4（ok -0x71→-0x75、id -0x78→-0x7c 偏离 ORIG），而 recipeIndex
    // 仍留 -0x28（不动）⇒ 4.4 的标量槽序非声明序可移，已回退（P212/N98 族）。
    int recipeIndex = 0;       // ORIG -0x2c
    // [P212] 全头部声明：ORIG 卡槽 cardIdx/boosterIdx 深于 pairCount/i
    // （-0x28/-0x24 vs -0x20/-0x1c）；块内声明在 4.4 下先声明浅 ⇒ 反序。
    int cardIdx;               // ORIG 无初始化（无 movl $0）⇒ 裸声明
    int boosterIdx;
    ST_Event_Upgrade_Jar_EventNPCs_Script npc;   // ORIG 函数级对象（0x08968828 C1，-0x80）

    for (;;)
    {
        // [b28d-solo-EventUpgradeJar] P164：ORIG 0x08968851 `jne 08968cfd`（尾部
        // EOF 块 `nop; mov $1,%ebx; jmp 清理`）⇒ 用 break + 循环外统一 return true
        // 复现尾部共享块（就地 `return true` 反产 je 跳过形）。
        if (!ScanType(token, true))
            break;

        if ("[group info]" == token)
        {
            // ORIG 0x08968878：先写 m_npcCount 字段，后查 ok。
            info->m_npcCount = (unsigned short)ScanInt(&ok);
            if (!ok)
                return false;

            // 每组连续读入若干 id，以 -1 结束；insert(make_pair(id, g))（ORIG insert）。
            for (unsigned short g = 0; g < info->m_npcCount; ++g)
            {
                // ORIG make_pair 模板参 <unsigned int&, unsigned short&> ⇒ id 为 unsigned。
                id = 0;
                // [b28d-solo-EventUpgradeJar] 真 bug 修正：ORIG 0x089688d8 `je +0x15b`
                // 落 g++（0x0896892c）——id==-1 只 break **内层 while**、继续下一轮 g；
                // 原码 break 出 for 会终止整个 [group info] 扫描。
                while (true)
                {
                    id = ScanInt(&ok);
                    if (!ok)
                        return false;
                    if (id == -1)
                        break;
                    info->m_boosterIndexMap.insert(std::make_pair(id, g));
                }
            }

            // 配方对：`<cardIdx> <boosterIdx> <recipeIndex>`，共 npcCount*(npcCount+1)/2 组。
            // [b28d-solo-EventUpgradeJar] ORIG 0x08968959 先 `lea 0x1(%eax),%edx`
            // （算 n+1）再读 n 入 eax、`imul %edx,%eax` ⇒ 源把 (n+1) 写在左侧。
            int pairCount = (int)((unsigned int)(info->m_npcCount + 1U) * info->m_npcCount) >> 1;
            for (int i = 0; i < pairCount; ++i)
            {
                cardIdx = ScanInt(&ok);
                if (!ok)
                    return false;
                boosterIdx = ScanInt(&ok);
                if (!ok)
                    return false;
                recipeIndex = ScanInt(&ok);
                if (!ok)
                    return false;
                info->m_recipeMap[std::make_pair(cardIdx - 1, boosterIdx - 1)] =
                    (unsigned long)recipeIndex;
            }
            // [b28d-solo-EventUpgradeJar] ORIG 段尾 `jmp 主循环头`（0x08968a48）、
            // 失败比较 `je 下一段` ⇒ 独立 if + continue（else-if 链被 4.4 排成
            // 「jne body; jmp 循环头」反形）。
            continue;
        }
        if ("[fortune index]" == token)
        {
            // ORIG 0x08968a6e..0x08968b60：固定 4 次（直线，无循环、无 -1 终止），
            // 每次 `push_back((unsigned long)ScanInt(&ok))`（右值临时槽）。
            int idx;
            idx = ScanInt(&ok);
            if (!ok)
                return false;
            info->m_materialIndices.push_back((unsigned long)idx);
            idx = ScanInt(&ok);
            if (!ok)
                return false;
            info->m_materialIndices.push_back((unsigned long)idx);
            idx = ScanInt(&ok);
            if (!ok)
                return false;
            info->m_materialIndices.push_back((unsigned long)idx);
            idx = ScanInt(&ok);
            if (!ok)
                return false;
            info->m_materialIndices.push_back((unsigned long)idx);
            continue;
        }
        if ("[fortune fail rate]" == token)
        {
            // ORIG 0x08968b86：逗号条件 while((groupIdx=ScanInt(&ok)), ok)；
            // 第二个值丢弃、第三个为 rate；首值失败仅回主循环（break），
            // 后两值失败 return false。无 -1 终止检查。
            int groupIdx;
            // [round5 复测] ORIG 0x08968b9x 形为 `movzbl ok; xor $0x1,%eax; test
            // %al,%al; je <body>` + fall-through `nop; jmp 主循环头`（bool 取反
            // 物化 + je 直落 body）；`ok == true` 写法在 4.4 恒产 `test; jne body`
            // **无 xor 段**（P168a②/P292 档①）⇒ 改 `if (!ok) break;` 裸取反形。
            for (;;)
            {
                groupIdx = ScanInt(&ok);
                if (!ok)
                    break;
                ScanInt(&ok);                 // 丢弃（ORIG 0x08968ba5 结果不入字段）
                if (!ok)
                    return false;
                int rate;
                rate = ScanInt(&ok);
                if (!ok)
                    return false;
                info->m_failRateByFortune[groupIdx - 1].push_back((unsigned int)rate);
            }
            continue;
        }
        if ("[event upgrade table]" == token)
        {
            // ORIG 0x08968c35：ScanFloat(&ok) 直存 m_discountRate（fstps 序）。
            // [P0] ORIG 无 float 局部槽（`fstps 0x70(%eax)` 直存成员，无
            // `mov -0xN(%ebp),%edx` 中转）⇒ 表达式内联，勿声明具名局部。
            info->m_discountRate = ScanFloat(&ok);
            if (!ok)
                return false;
            for (;;)
            {
                ScanInt(&ok);                 // 丢弃（ORIG 0x08968c5b 结果不入字段）
                npc.m_failRate = (unsigned int)ScanInt(&ok);
                if (!ok)
                    break;                    // 序列终止：回主循环
                npc.m_meterialCount = (unsigned int)ScanInt(&ok);
                if (!ok)
                    return false;
                info->m_npcScripts.push_back(npc);
            }
            continue;
        }
        if ("[upgrade max]" == token)
        {
            // ORIG 0x08968cd7：先写 m_upgradeMax 字段，后查 ok。
            info->m_upgradeMax = ScanInt(&ok);
            if (!ok)
                return false;
        }
    }
    return true;
