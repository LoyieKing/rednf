int CNPCScriptList::initScript(char* dir, char* list, char* common)
{
    // [b28d-sweepC 判词·DIFF→NEAR，残余 N82 槽号] ORIG 0x08581314：
    // 历史修复：P304 去 reinterpret_cast 寄存器物化；`it->second.c_str()` 内联
    // （P157/P48）。
    // 本轮修（gap 13→0、助记符序列全等）：①`if (!isExistNPCScript(...))` 极性
    // ——ORIG 0x085813bf `call; xor $1; test %al; je <body>` ⇒ 原码否定式 + 失败
    // 块 fall-through（P47/P220/P292），旧正向 `if (isExist) {body} else {trace}`
    // 块序相反；②两处 cMyTrace 改**全表达式临时链**（P227/P328⑩）——ORIG
    // 0x085813c6/0x0858148c 先把 `c_str()` 求值入 %ebx 再构造 trace，两语句形
    // 把 ctor 提前致块序失配；③for 改**预计算 end() + while**（P13）——ORIG
    // 0x0858136e 循环前算 itEnd，旧 for 每轮重调 end()。
    // 残余：4.1 不复用栈槽（begin/end sret 落 -0x80/-0x7c）vs 4.4 复用
    // ⇒ 全帧槽号错位（N82），助记符序列已全等（NEAR）。
    // ORIG 0x08581314。
    if (!initNpcScript(dir, list))
        return 0x1a1;

    // [b28d-solo-CtorProducers_1] P304：旧 `reinterpret_cast<STFileListMapMirror*>(
    // &g_npcScriptFileList)` 使 begin/end 的 this 经寄存器物化；STScriptFileList
    // 的 m_fileMap@+0 是权威同型成员 ⇒ 直接成员访问发 `movl $0x94fe780,0x4(%esp)`。
    // P157/P48：p167 差集 MISSING 3×`operator->`+3×`c_str` ⇒ ORIG 每个使用点重读
    // `it->second.c_str()`（4 处），禁具名 name 缓存。
    // ORIG 0x08581343/0x0858136e：begin()→const_iterator(it) 后**预计算** end()
    // →const_iterator(itEnd)（P13 end 缓存；循环尾仅 ++it 再比 it!=itEnd，
    // 不是每轮重调 end()）。故写 while 形而非 for 形。
    std::map<int, std::string>::const_iterator it =
        g_npcScriptFileList.m_fileMap.begin();
    std::map<int, std::string>::const_iterator itEnd =
        g_npcScriptFileList.m_fileMap.end();
    while (it != itEnd)
    {
        // ORIG 0x085813bf：`call isExistNPCScript; xor $0x1,%eax; test %al,%al;
        // je <body>` ⇒ 原码是否定式 `if (!isExist…)`，失败块 fall-through、body 在
        // je 目标（P47/P220/P292）。
        if (!isExistNPCScript(it->second.c_str()))
        {
            // ORIG 0x085813c6：c_str 在 cMyTrace ctor 前求值并以 %ebx 保活
            // ⇒ 全表达式临时链（P227/P328⑩）。
            cMyTrace(__PRETTY_FUNCTION__, 0x1a9, 5)(
                "%s file not exists\n", it->second.c_str());
        }
        else
        {
            CNPCScript* npc = new CNPCScript;
            if (!npc->load(it->second.c_str()))
            {
                // ORIG 0x0858148c：同全表达式临时链（c_str 先求值 → %ebx）。
                cMyTrace(__PRETTY_FUNCTION__, 0x1b0, 5)(
                    "%s load failed\n", it->second.c_str());
                return 0x1b4;
            }
            m_npcMap.insert(
                std::make_pair(static_cast<unsigned int>(npc->getIndex()), npc));
            if (npc->isFavorableNPC())
                G_CDataManager()->m_npcDynamicInfo->add(npc);
        }
        ++it;
    }

    if (loadNPCCommonData(m_npcCommon, common) != 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1be, 5);
        trace("%s load failed\n", common);
        return 0x1bf;
    }
    if (m_npcCommon.m_favorableNpcList.size() > 0x32)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1c3, 5);
        trace("%s Favorable npc count over\n", common);
        return 0x1c4;
    }
    TimerNPCMoodChange::registNextTimer(0);
    return 0;
