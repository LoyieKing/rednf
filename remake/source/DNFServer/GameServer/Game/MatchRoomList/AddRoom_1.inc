bool CMultiMatchRoomList::AddRoom(IMatch* room)
{
    char name[255] = {0};
    // b28d-solo-MatchRoomList VERDICT: NEAR（447 167=167；446 167=165）| 修法：
    // if/else 正极性 + find 具名临时拷贝（tmpIt→it，复现 ORIG sret 中转）|
    // N7（虚调 GetPlayerCount 目标 %eax↔%edx）+ N82（name[255] 槽基 -0x12f vs
    // ORIG -0x127 ⇒ sret 临时 disp8/disp32 长度差 6B，帧布局族）
    // b28d-M2S3：①P139/§6.5 手写签名串 → `__PRETTY_FUNCTION__`（ORIG 0x85679e9
    // 实参 = `_ZZN19CMultiMatchRoomList7AddRoomEP6IMatchE19__PRETTY_FUNCTION__`
    // 具名局部静态数组，旧手写串令该符号「仅 ORIG」）；②复测 167/167 且助记符
    // 序列全等（NEAR），残余 19 块全为帧内槽基差（name[255] -0x12f vs ORIG
    // -0x127 / CSwitchLog -0x20 vs -0x24 / sret 临时 -0x28 vs -0x12c）⇒ N82 维持。
    // b28d-L3：同 DelRoom（ORIG 0x8567c35 形）——GetUserName 返回值经 %ebx 跨
    // CSwitchLog ctor 保活并作 log 的 %s 实参；register 提示令 4.4.6 免落槽。
    // [N1C·2026-09-11 · 负结论] 残余 15 块全为帧内槽基差（name[255] -0x12f vs
    // ORIG -0x127 / CSwitchLog -0x20 vs -0x24 / begin sret -0x30 vs -0x28 /
    // find sret -0x28 vs -0x12c / playCount -0x24 vs -0x10），指令数与助记符序列
    // 全等（167/167 NEAR）。试：tmpIt 声明前置（167→172 条、13 块）、tmpIt+it
    // 双前置（175 条、15 块）均劣化 ⇒ 判 N82（GCC 帧布局族，源码不可达），维持现状。
    register char* pName = GetUserName(room, name);
    // b28d-M2S3（P139/§6.5）：ORIG 0x85679e9 实参 =
    // `&_ZZN19CMultiMatchRoomList7AddRoomEP6IMatchE19__PRETTY_FUNCTION__`
    // 具名局部静态数组；旧源手写串令该符号「仅 ORIG」缺失。
    CSwitchLog log(__PRETTY_FUNCTION__, 0x11ea, 0, 0);
    log("pvp@log match(%x), %s ", room, pName);

    for (std::map<int, CWaitingRoomList>::iterator it = m_roomMap.begin();
         it != m_roomMap.end(); ++it) {
        if (it->second.find(room)) {
            LogManager::logFormat(1, "MatchingSystem.cpp",
                __PRETTY_FUNCTION__, 0x11f3,
                "error find line(%d)", 0x11f3);
            return false;
        }
    }
    int playCount = room->GetPlayerCount();
    // b28d-solo-MatchRoomList: ORIG 为 4.1 初始化式形（find sret 临时 -0x12c 拷回
    // -0x28）；4.4 直接 sret 到 it 槽（少 1 条 mov+临时槽，P112/N82 族封顶）。
    // 「先声明后赋值」反而引入 _Rb_tree_iterator::C1Ev call（ORIG 无）⇒ 不可用。
    std::map<int, CWaitingRoomList>::iterator tmpIt = m_roomMap.find(playCount);
    std::map<int, CWaitingRoomList>::iterator it = tmpIt;
    // b28d-solo-MatchRoomList: 正极性 if（ORIG test%al;je <error>，then=insert 直落）
    if (it != m_roomMap.end()) {
        it->second.insert(room);
    }
    else {
        LogManager::logFormat(1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__, 0x11ff,
            "error find line(%d), playCount(%d)", 0x11ff,
            room->GetPlayerCount());
    }
    return true;
}