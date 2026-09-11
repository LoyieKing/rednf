// ============================================================================
// CParty::OnFightVillageMonster —— ORIG 0x85b9596：村庄受攻战入场；管理器
// 受理后走 game_start 全流程并广播 0x1c/0x1d 地图包。
// ============================================================================
// [SweepCParty] 补真缺口：_getMemberNames 两次调用（第二次在循环内取返回值作
// EnterDungeon 实参，旧码仅 1 次）+ names 改 `={0}` 消 EXTRA memset。
// p167 CLEAR（余 C1/C2、D1/D2 别名）。
int CParty::OnFightVillageMonster(CUser* user, int param)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS _getMemberNames / EXTRA PacketGuard C1+D1+memset
    //（PacketGuard 临时链形）。@0x85b9596
    // b28d-solo-CParty-2：按 ORIG 0x85b95a2..0x85b9618 去引用缓存
    //（bf/world/wml/bd 改表达式直用，P183；早退形 `ret = call(); if (ret) return ret;`）。
    int ret = 0;                                        // -0x30
    ret = GlobalData::s_villageMonsterMgr->OnFightVillageMonster(
        user, (unsigned short)param);
    // [P225] ORIG 0x085b95b5 `cmpl $0x0,-0x30; setne %al; test %al,%al; je`：
    // 4.1 bool 物化形，4.4 复现形唯一为 while(cond){return;}（probe /tmp/probe_setne.cpp t3）。
    while (ret != 0)
    {
        return ret;
    }
    game_start(user);
    int dungIdx = reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_index();
    int diff = 0;                                       // -0x28
    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungIdx);   // -0x24
    if (dungeon == 0)
        return 0x15;
    int wmIdx = G_GameWorld()->GetWorldMapIndex(user);  // -0x20
    if (0 < wmIdx)
        m_hellWorldMap = G_CDataManager()->m_worldMapList.find_world_map(wmIdx);  // +0x1868
    if (!reinterpret_cast<CBattle_Field*>(&m_battleField)->SelectDungeon(
            dungIdx, dungeon, diff, (ENUM_DUNGEON_TYPE)0,
            get_member_count(), 0, 0, 0))
        return 1;
    {
            // ORIG 0x085b96fc + 0x085b9736 两次 _getMemberNames（第二次在循环内取返回值
            // 作 EnterDungeon 实参）；names 清零为内联 rep stos（={0}，非 call memset）。
            char names[0x100] = {0};                    // -0x140
            _getMemberNames(names);
            for (int i = 0; i < 4; ++i)                 // -0x20
            {
                // ORIG 085b9724 `jne 085b9788(nop)` + 085b9786 `jmp`：
                // 4.1 if-join 岛（P223）⇒ 否定条件 goto 循环体尾空标签。
                if (!_checkValidUser(i))
                    goto next_member;
                m_member[i].m_pUser->m_historyLog.EnterDungeon(
                    dungeon->GetDungeonName(), diff, _getMemberNames(names), 0);
            next_member:;
            }
            PacketGuard packet;
            packet.clear();
            packet.put_header(0, 0x1c);
            packet.put_short(dungIdx);
            packet.put_byte(diff);
            reinterpret_cast<CBattle_Field*>(&m_battleField)->MakePacket(0x1c, &packet);
            send_to_party(packet);
            packet.clear();
            reinterpret_cast<CBattle_Field*>(&m_battleField)->MakePacket(0x1d, &packet);
            send_to_party(packet);

            // ORIG 085b98xx `mov $0,%edx; mov this,%ecx; mov %eax,0x34c(%ecx);
            // mov %edx,0x350(%ecx)` = 零扩展 64 位整体存（P246/P180 u64 别名），
            // 非两条 int 赋值（后者两次重载 this 且无 edx 零扩展）。
            m_battleData.m_clearEnterTime =
                (unsigned long long)GlobalData::s_systemTime_.getCurTickCount();
            // ORIG 085b9874 段三组同址双 store（33c/340、344/348、358/35c）
            // ⇒ P206 8 字节 union 视角成对清零。
            m_battleData.m_baseTick = 0;                // +0x10/+0x14
            m_battleData.m_enterTick = 0;               // +0x18/+0x1c
            m_battleData.m_accTick = 0;                 // +0x2c/+0x30
            m_battleData.m_moveTick = m_battleData.m_clearEnterTime;  // +0x34/+0x38
            set_state(2);
            SetEPLPState(1);
            send_loadingcheck_msg();
            // [2026-09-10 试形] ORIG p167: PacketGuard C2×2 / D2×1（仅 1 个析构点）
            // vs 我方 C1×2 / D1×4 ⇒ 疑原码复用同一 packet 实例（无 ack 具名对象）。
            packet.clear();
            packet.put_header(1, 0x10b);
            packet.put_byte(1);
            packet.finalize(true);
            user->Send(packet);
            return 0;
    }
    return 1;
}