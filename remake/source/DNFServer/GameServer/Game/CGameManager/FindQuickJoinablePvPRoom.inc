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
            // [b29x 实测] 德摩根 `!quick` 与正比较 `quick == 0` 两形读数相同（均 4 条
            //   movzbl;xor;test;jne）——那是**负极性物化**所致。
            // [J9-2026-09-11 · 真语义 bug 修复（P224 短路链极性判据）] ORIG 的 bool
            //   物化段 0x082a327c..0x082a3293：`call IsQuickJoinablePVPRoom; test %al,%al;
            //   je 082a328c(mov $0x0 岛)` —— **正极性**直测 ⇒ 第二项是
            //   `room->IsQuickJoinablePVPRoom(user)`（无 `!`），我方旧形多一个 `!`
            //   ⇒ 条件整体反转（可加入的房间被跳过、不可加入的被返回，运行行为相反）。
            //   等价式：`if (!((quick && IsExistPassword()) || IsQuickJoinablePVPRoom(user)))
            //   return room;`（ORIG 首项 0x082a3257 cmpb 直比亦为正极性 `quick`）。
            // [三形读数（本波实测，语义等价）] 复合否定形 ndiff **7**（our 72；最优，采用）；
            //   嵌套 `if (quick&&E) goto; if (C) return;` 形 ndiff 12（our 69）；
            //   旧（错极性）复合形 ndiff 6 但语义反转 ⇒ 不可用。残差 = 4.4 短路 lowering
            //   不发 4.1 的 bool 物化岛（mov $1/$0 + test/je）+ `!E` 的 xor 归一化 ⇒ N122/N3 封顶。
            // [K9GameRestA.CGameMgr12Run 2026-09-11 · 语义轴复核（P224）] 上一形的
            //   `!((quick&&E) || C)` 展开 = `!(quick&&E) && !C` ⇒ **C==0 才返回**，
            //   与 ORIG 0x082a3281..0x082a3298 直证相反（ORIG：`test %al,%al; je FALSE`
            //   正极性 ⇒ C==1 落 `mov $0x1` ⇒ return room）。已按 ORIG 改为
            //   `(!quick || !E) && C`：C 用**正**极性（消我方多出的 `xor $1`），
            //   E 段 `xor $1; je FALSE` 与 ORIG 0x082a3268..0x082a326d 逐形一致。
            if ((quick == 0 || !room->IsExistPassword()) &&
                room->IsQuickJoinablePVPRoom(user))
                return room;
        }
    it_inc:
        ++it;
    }
    return 0;
}