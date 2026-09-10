bool CMultiMatchRoomList::AddRoom(IMatch* room)
{
    char name[255] = {0};
    // b28d-solo-MatchRoomList VERDICT: NEAR（447 167=167；446 167=165）| 修法：
    // if/else 正极性 + find 具名临时拷贝（tmpIt→it，复现 ORIG sret 中转）|
    // N7（虚调 GetPlayerCount 目标 %eax↔%edx）+ N82（name[255] 槽基 -0x12f vs
    // ORIG -0x127 ⇒ sret 临时 disp8/disp32 长度差 6B，帧布局族）
    // b28d-L3：同 DelRoom（ORIG 0x8567c35 形）——GetUserName 返回值经 %ebx 跨
    // CSwitchLog ctor 保活并作 log 的 %s 实参；register 提示令 4.4.6 免落槽。
    register char* pName = GetUserName(room, name);
    CSwitchLog log("virtual bool CMultiMatchRoomList::AddRoom(IMatch*)", 0x11ea, 0, 0);
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
