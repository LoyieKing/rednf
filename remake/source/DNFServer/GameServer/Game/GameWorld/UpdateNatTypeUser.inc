// [b28d-solo-GameWorld] _ZN9GameWorld17UpdateNatTypeUserEv verdict=NEAR 173/174
// （b28d-solo 本轮；仅 4.4 循环体前 1 条对齐 nop 差，助记符/操作数全等）。
// 修法：①P283 第一循环 goto 尾条件（ORIG 0x086cd324 `jmp COND`）；②P28 增量
// ppEv 前缀 ⇒ `++it`；③P259 Acquire 内联进 CStreamGuard 实参（0x086cd3f6 段无
// stream 具名槽）+ 具名 type/count（ORIG -0x14/-0x10）；④**第二循环改 for 形**
// —— ORIG 0x086cd52f `test %al; jne <LOOP2>; jmp <exit>`，旧 goto 形 4.4 恒产
// `je exit; jmp body`（极性反）；for 形逐条复现（含每轮 end 调用位置）。
// 余 N112（4.4 循环出口对齐 nop，TU 旗标不可改）；p167 CLEAR 30=30。
// [H8B 2026-09-11 复核·终判] 173/174（我方多 1）+ p167 30/30 CLEAR ⇒ 无真缺口；唯一差 =
// 4.4 循环出口对齐 nop（N112，TU 旗标不可改）。第一循环 goto 尾条件 + `++it`、第二循环 for 形
// 既有修法保留；natType 槽四形（块内/头 natMap 前/后/it 后）已实测均不合 ORIG -0x28（N82/N93）。
void GameWorld::UpdateNatTypeUser()
{
    // [b28d-solo-GameWorld] P232：ORIG 槽基比我们深 4（ORIG natMap=-0x44/guard=-0x54，
    // 我们 -0x40/-0x50）⇒ 4 字节死局部撑帧下移槽基（P190 double 过头：帧 0x60→0x70）。
    char framePad[4];
    (void)&framePad;
    std::map<int, int> natMap;
    // ORIG 0x086cd31c begin(-0x48) → 0x086cd324 `jmp 条件块`（P283）
    std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
    goto COND;
LOOP:
    {
        CUser* user = it->second;
        if (user != NULL)
        {
            // b28d-solo：natType 槽旋钮四形实测（块内 -0x50 / 头 natMap 后 -0x48 /
            // 头 natMap 前 -0x30 / 头 it 后 -0x4c）均不合 ORIG -0x28；块内形保住
            // natMap=-0x44/it=-0x48 与 ORIG 一致 ⇒ N82/N93 封顶（4.4 块级标量槽方向）。
            int natType = (int)user->m_network.get_nat_type();
            natMap[natType]++;
        }
    }
    ++it;
COND:
    if (it != m_UserInWorld.end())
    {
        goto LOOP;
    }

    int time = GlobalData::s_systemTime_.getCurSec();
    // ORIG 0x086cd3c8 begin(-0x4c) → 0x086cd3d0 `jmp 条件块`（P283）
    for (std::map<int, int>::iterator nit = natMap.begin();
         nit != natMap.end(); nit++)
    {
        // ORIG 0x086cd3e0/0x086cd3f0：nit->first/second 在 Acquire 之前读取并落
        // -0x14(type)/-0x10(count)；GetInBuffer 结果落 -0xc(sig)。
        int type = nit->first;
        int count = nit->second;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("world.cpp", 0x14ef), true);
        **guard << 0x84;
        **guard << -1;
        SIG_NAT_TYPE_USER* sig = guard.operator->()->GetInBuffer<SIG_NAT_TYPE_USER>();
        // ORIG 0x086cd486/0x086cd48c `memset(sig,0,0xc)`（call 形，p167 MISSING 实证）
        memset(sig, 0, 0xc);
        sig->m_type = type;
        sig->m_count = count;
        sig->m_time = time;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }
}