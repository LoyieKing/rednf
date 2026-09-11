// ---------------------------------------------------------------------------
// move_map —— ORIG T 0x085a8f4e
// 报告：docs/class_func_reports/CParty/move_map.md
// ---------------------------------------------------------------------------
bool CParty::move_map(MSG_MOVE_MAP& info)
{
    // [b28d-solo-CPartyDepsC2 判词] 封 N7/N107（ORIG 0x085a8f4e）：p167 36=36
    // CLEAR；P183 去 CBattle_Field 引用缓存（逐点 reinterpret_cast 重算
    // this+0xb24）、P212 全头部声明（槽 -0x30/-0x24/-0x20/-0x1c/-0x18/-0x14/
    // -0x10/-0x31/-0xc 全等）、P47 极性（MakePacket >0 / levelGap >6 块序）、
    // P225 拆嵌套 if（drop-test 两条件免 bool 物化岛）已逐条归位；残差 =
    // ①CheckMoveMapFitInPathHackCnt 调用点实参寄存器轮换（ORIG ecx/edx/eax/
    //   esi/ebx ↔ 4.4 ebx/ecx/edx/eax/esi，N7 族，长度助记符全等仅寄存器名差）；
    // ②`&m_padSecu[0]` 取址 ORIG `lea 0x1abc(%ebx),%esi` vs 4.4 `add $0x1abc,%eax`
    //   （N107 六形探针不可翻）。语义等价。
    PacketGuard packet;   // ORIG 在状态检查前构造，所有出口析构
    int ret = 0;          // ORIG 0x085a8f61 `movl $0x0,-0x24(%ebp)`
    // [b28d-solo-CPartyDepsC2] P212 全头部声明：ORIG 槽深序
    //   dungeon -0x20 / curPosX -0x1c / curPosY -0x18 / fromMapIdx -0x14 /
    //   toMapIdx -0x10 / firstVisit -0x31 / levelGap -0xc。
    CDungeon* dungeon;
    int curPosX;
    int curPosY;
    int fromMapIdx;
    int toMapIdx;
    bool firstVisit;
    int levelGap;

    if (m_state != 2)
    {   // 非地图移动状态 → 0x30 号 cmd 包错误码 0x13
        send_to_party_cmd_error((ENUM_CMDPACKET)0x30, 0x13);
        return false;
    }

    dungeon = m_battleField.m_pDungeon;
    if (dungeon != 0 && dungeon->IsEnterEachMap())
    {   // 逐张进入型副本：直接改写当前座位号
        reinterpret_cast<CBattle_Field*>(&m_battleField)->SetCurSeatNo(
            (int)(char)info.m_39);                      // SetCurSeatNo W 0x082a43cc
    }
    else if (reinterpret_cast<CBattle_Field*>(&m_battleField)
                 ->IsClearDungeon())                    // W 0x0822d08c
    {
        send_to_party_cmd_error((ENUM_CMDPACKET)0x30, 0x13);
        return false;
    }

    curPosX =
        reinterpret_cast<CBattle_Field*>(&m_battleField)->GetCurPosX();  // W 0x085bf32c
    curPosY =
        reinterpret_cast<CBattle_Field*>(&m_battleField)->GetCurPosY();  // W 0x085bf382
    fromMapIdx = reinterpret_cast<CBattle_Field*>(&m_battleField)
                     ->get_map_index(curPosX, curPosY);                  // W 0x085bf260

    if (info.m_field17 == 1)
        reinterpret_cast<CBattle_Field*>(&m_battleField)
            ->checkLayeredMap((int)info.m_x, (int)info.m_y);   // T 0x0830e32e

    // ORIG 第二次 get_map_index 结果存栈后未再消费，保留调用形态
    toMapIdx = reinterpret_cast<CBattle_Field*>(&m_battleField)
                   ->get_map_index((int)info.m_x, (int)info.m_y);
    (void)toMapIdx;

    // 反作弊路径校验（[b28d-solo-CParty2] +0x1abc 已字段化为 m_secu；
    // 取址 4.4 恒 add vs ORIG lea 属 N107 封顶）
    m_secu.CheckMoveMapFitInPathHackCnt(m_host, this, info.m_fieldF,
                                        info.m_field13, fromMapIdx);

    firstVisit = false;
    ret = reinterpret_cast<CBattle_Field*>(&m_battleField)
              ->MoveMap((int)info.m_x, (int)info.m_y, firstVisit);  // T 0x08308ae6
    if (ret > 0)
    {   // MoveMap 失败码 > 0：错误码原样回发
        send_to_party_cmd_error((ENUM_CMDPACKET)0x30, (unsigned char)(ret & 0xff));
        return false;
    }

    checkMoveMapHackCnt(info);

    // ORIG 0x085a919c `jle 0x1d 块` ⇒ 条件写 >0（0xd3 块在前，P47 块序）。
    if ((char)m_battleField.m_pDungeon->m_bloodDungeonType > 0)
        reinterpret_cast<CBattle_Field*>(&m_battleField)->MakePacket(0xd3, &packet);
    else
        reinterpret_cast<CBattle_Field*>(&m_battleField)->MakePacket(0x1d, &packet);
    send_to_party(packet);

    check_passive_skill();
    send_loadingcheck_msg();
    resetFinishLoadingVar();
    changePartyMemberInfoOnMoveMap(info, firstVisit);

    // 队伍统计：人数>1 时按等级差分桶计数
    levelGap = _getMemberLevelGap();
    if (get_member_count() > 1)
    {
        // ORIG 0x085a9257 `jle 3 块` ⇒ 条件写 >6（GetPartyType 块在前，P47 块序）。
        if (levelGap > 6)
            GetInstanceCommonStatisticsMgr()->IncreaseQuantity(
                (CCommonStatisticsMgr::ENUM_STATISTIC_CONTAINER)0, GetPartyType());
        else
            GetInstanceCommonStatisticsMgr()->IncreaseQuantity(
                (CCommonStatisticsMgr::ENUM_STATISTIC_CONTAINER)0, 3);
    }

    changeBattleDataOnMoveMap(firstVisit);

    // GM 掉落测试：记录途经地图（+0x1af4 vector<MapInfo>，push_back W 0x084b472c）。
    // ORIG 两条件各自独立 test/je（P225 推广：拆嵌套 if 免 bool 物化岛）。
    if (getDungeonDropTestFlag())
    {
        if (!firstVisit)
        {
            // [b28d-solo-CParty2] +0x1af4 已字段化为 m_passedMap（vector<MapInfo>）
            m_passedMap.push_back(*reinterpret_cast<CBattle_Field*>(&m_battleField)
                                      ->GetCurrentMapInfo());  // W 0x0822cbcc
        }
    }
    return true;
}