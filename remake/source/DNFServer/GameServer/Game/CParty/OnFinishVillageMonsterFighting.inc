// ============================================================================
// CParty::OnFinishVillageMonsterFighting —— ORIG 0x85b99ac：村庄战收尾；
// 结算胜负、giveup_game 并对全员发 0x1ec 复仇地下城资格包。
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧/槽；307 vs 302
    // [G7-sub 2026-09-10 复核·PacketGuard/EH] ORIG 0x085b99ac 的 PacketGuard 站点：
    // C1 ×2（ack -0x5c @0x085b9b21；packet -0x74 @0x085b9c39），两者生命周期重叠
    // （外 try 085b9b2c..085b9c3d / 内 try 085b9c54..085b9d47）⇒ **不是复用同一对象**。
    // 我方 C1 也是 2 个（数量一致，未发明对象）。我方 p167 +1 `_Unwind_Resume` 的真因：
    // ORIG 内层 catch 0x085b9dbf 清 -0x74 后 `jmp` 外层 catch 入口 0x085b9df9，
    // 共享唯一 _Unwind_Resume（0x085b9e0f）；4.4 给内层单独发一条 ⇒ 属 N104
    // （EH 尾块相对序）族，非源码可解。本 .inc 段（members[4] 清零 + 两循环）
    // 与 ORIG 0x085b99bd..0x085b9a76 逐点一致（clear 分支 `je <body>` 阳性 then
    // +0x085b9a9e 已对齐）。⇒ 封顶，不动。
int CParty::OnFinishVillageMonsterFighting(CUser* user, bool flag)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS G_CDataManager + vector operator[]/size +
    // EXTRA const_iterator 6 项（P96 迭代器形，改型回归实测）。@0x85b99ac
    CUser* members[4] = {0, 0, 0, 0};                   // -0x54
    for (int i = 0; i < 4; ++i)                         // -0x3c
    {
        // ORIG 0x085b99f4 `xor $0x1,%eax; test %al,%al; jne <nop 岛>` ⇒ 负极性早退
        //（P168a②/裸形：`if (!f()) continue;`），旧码正向 if 缺 xor 段。
        if (!checkValidUser(i))
            continue;
        members[i] = get_user(i);
    }
    if (user == 0)
    {
        for (int i = 0; i < 4; ++i)                     // -0x38
        {
            if (checkValidUser(i))
            {
                user = get_user(i);
                break;
            }
        }
    }
    // ORIG 0x085b9a7c `mov $0x3,%ebx` + 0x085b9e14 `mov %ebx,%eax` ⇒ 单一 ret 出口。
    int ret = 0;                                        // -0x3c
    if (user == 0)
    {
        ret = 3;
    }
    else
    {
    // ORIG 0x085b9a86/0a8a 先两次 `movb $0x0` 预清，再按 clear 分支各置 1
    //（`test %al,%al; je <else>` 阳性 then = isClear=true；-0x3e=notClear 为死写）。
    bool notClear = false;                              // -0x3e
    bool isClear = false;                               // -0x3d
    // P183：ORIG 各使用点重取址（无引用缓存槽）⇒ 宏直用。
#define bf (*reinterpret_cast<CBattle_Field*>(&m_battleField))
    if (bf.check_grid_clear())
        isClear = true;
    else
        notClear = true;
    // ORIG 0x085b9ab3 `movzbl -0x3d(%ebp),%edx`（=isClear）⇒ 旧码误传 notClear，真 bug。
    ret = GlobalData::s_villageMonsterMgr->OnKillVillageMonster(user, isClear);
    if (ret == 0)
    {

    giveup_game(user, false, isClear, flag);
    // ORIG OnFinishVillageMonsterFighting.md 085b9b12 `movw $0x0,0xc4c(%eax)` =
    // CBattle_Field(this+0xb24)+0x128 → m_villageMonsterId（short）
    reinterpret_cast<CBattle_Field*>(&m_battleField)->m_villageMonsterId = 0;
    {
        PacketGuard ack;                                // -0x60
        ack.clear();
        ack.put_header(1, 0x10c);
        ack.put_byte(1);
        ack.finalize(true);
        user->Send(ack);
    }
    if (!GlobalData::s_villageMonsterMgr->OnEvent() &&
        GlobalData::s_villageMonsterMgr->GetRewardType() == 1)
    {
        // [轴 B，地址自校 0x085b9be0] ORIG `mov -0x2c,%eax; cmp -0x30,%eax;
        // jb +0x24a` = **无符号**比较（我方旧文 int 产 `jl`，符号性相反）。
        // 以 ORIG 为准改 unsigned：`remain` 仍为 int（仅写入 body+2 的 4 字节块）。
        unsigned int nowSec =
            (unsigned int)GlobalData::s_systemTime_.getCurSec();     // -0x34
        unsigned int closeTime =
            (unsigned int)GlobalData::s_revengeDungeonMgr->GetCloseTime();  // -0x30
        int remain =
            (int)(closeTime < nowSec ? 0 : closeTime - nowSec);      // -0x2c
        for (int i = 0; i < 4; ++i)                     // -0x28
        {
            if (members[i] != 0)
            {
                char body[10];                          // -0x6a
                memset(body, 0, sizeof(body));
                *(int*)(body + 2) = remain;             // ORIG body+2 写 remain
                PacketGuard packet;                     // -0x78
                packet.put_header(0, 0x1ec);
                // ORIG 0x085b9c68：size() 先于 attackCount（Ghidra local_24/local_20 序）。
                // [判据③/N18b] ORIG 槽 -0x20(uint size)/-0x1c(uint attackCount，uchar 截断后
                //   存 32 位槽：`movzbl %al,%eax; mov %eax,-0x1c`)/-0x18(循环变量 uint)/
                //   -0x14/-0x10/-0xc 三字段。旧码 attackCount 用 uchar 槽(-0x19)、
                //   循环变量 int（ORIG 尾测 `setb` 无符号）⇒ 全部 32 位无符号化。
                unsigned int condCnt =
                    (unsigned int)
                        G_CDataManager()->m_serverParameter.m_revenge498.size();
                unsigned int attackCount =
                    (unsigned char)reinterpret_cast<CUserCharacInfo*>(members[i])
                        ->GetCurVillageAttackCount();
                if (2 < attackCount)                    // ORIG body+6 条件写 0x1e41
                    *(int*)(body + 6) = 0x1e41;
                packet.put_binary(body, 10);
                packet.finalize(true);
                members[i]->Send(packet);
                members[i]->SetVillageAttackedReward(false);
                // P175：ORIG 走 size()+operator[] 索引形（p167 MISS operator[]/size，
                // EXTRA 迭代器族）；每次下标重取 G_CDataManager()（3 次独立调用）。
                // P183 反向：ORIG 无 CUserCharacInfo* 具名缓存，两处调用点各自从
                //   `-0x50(%ebp,%eax,4)` 重取 members[i]（旧码 ci 缓存槽 ⇒ 实参装载序差）。
                for (unsigned int k = 0; k < condCnt; ++k)
                {
                    unsigned int f0 = G_CDataManager()
                        ->m_serverParameter.m_revenge498[k].m_field0;
                    unsigned int f4 = G_CDataManager()
                        ->m_serverParameter.m_revenge498[k].m_field4;
                    int f8 = G_CDataManager()
                        ->m_serverParameter.m_revenge498[k].m_field8;
                    // P196 折痕方向：ORIG 0x085b9d66 `mov -0x1c(attackCount),%eax;
                    //   cmp -0x14(f0),%eax; jb` ⇒ 写成 attackCount 居左才出该装载序。
                    if (attackCount >= f0 && attackCount <= f4)
                    {
                        reinterpret_cast<CUserCharacInfo*>(members[i])
                            ->SetCurRevengeDungeonCount((unsigned char)f8);
                        reinterpret_cast<CUserCharacInfo*>(members[i])
                            ->VillageAttack_DBUpdate();
                        break;
                    }
                }
            }
        }
    }
        ret = 0;                                        // ORIG 085b9de7 `mov $0x0,%ebx`
    }
    }
    return ret;
}