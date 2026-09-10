PvP_Room* CGameManager::FindQuickJoinablePvPRoom(bool quick, CUser* user)
{
    // b28d-solo-CGameManager 判词：控制流已按 ORIG 重写（mode==6 正向守卫 sete 形、
    // (quick==0||!IsExistPassword())&&IsQuickJoinablePVPRoom 短路物化；PvP_Room.h
    // IsQuickJoinablePVPRoom int→bool 由 S09 落定并经 R2 广播）；余 70 vs 69 为
    // `quick==0` 的 xor 物化（N122：4.4 对 bool 局部取反恒 xor，4.1 为 cmpb 内存
    // 直比）——S1 实测 `(int)quick==0`→`quick==0` 两形产物逐条相同，按 N122 封顶；
    // IsExistPassword 已由 Main 落 bool（PvP_Room.h:110，T3 直证 0x082a3263），
    // 裸 `!` 形复现 ORIG 的 `xor $1; test; je` 段。ORIG 0x082a3202。
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.begin();
    // b28d-solo-CGameManager: while 形复现 ORIG 入口 jmp 底部条件并消 goto 形多出的
    // jmp 后 nop（探针 /tmp/solo_probe5.cpp h_while；ORIG 增量段调 ppEv = 前置 ++it）。
    while (it != m_pvpRoomMap.end())
    {
        PvP_Room* room = it->second;
        if (room != 0)
        {
            // b28d-solo-CGameManager: ORIG 0x082a324d `cmp $6; sete; test; jne`
            // ⇒ 正向守卫 `== 6` 跳过（原 `!= 6` 包裹产生反向 je 布局）。
            if (room->get_pvp_battle_mode() == 6)
                goto it_inc;
            // ORIG 0x082a3257..0x082a3293（Ghidra C 逐字）：
            //   false = (quick && IsExistPassword()) || !IsQuickJoinablePVPRoom(user)
            // 本形与原德摩根形 `(quick==0||!pw)&&joinable` 在 4.4 产物**逐条相同**
            // （`movzbl -0x2c; xor $1; test; jne`，quick 进 && 首项即被物化）；
            // ORIG 的 `cmpb $0,-0x2c; je` 内存直比属 4.1 bool 形参消费形
            // ⇒ N122 封顶（两形实测同，保留与 ORIG Ghidra 一致的写法）。
            if (!((quick && room->IsExistPassword()) ||
                  !room->IsQuickJoinablePVPRoom(user)))
                return room;
        }
    it_inc:
        ++it;
    }
    return 0;
