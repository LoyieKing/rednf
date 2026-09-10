int CDeathTower::handleMoveMap()
{
    try
    {
        if (_checkMemberDie())
        {
            CPacketHandler packet;
            packet.makeCommonError((ENUM_CMDPACKET)0x30, (ENUM_ERROR)0x16);
            m_party->send_to_party(packet);
            return 0;
        }

        if (m_stage.getState() < 2)
        {
            CPacketHandler packet;
            packet.makeCommonError((ENUM_CMDPACKET)0x30, (ENUM_ERROR)0x13);
            m_party->send_to_party(packet);
            return 0;
        }

        // ORIG 0x08465d97..0x08465db2：getEndStage 结果直接作实参（无槽，P0）
        // [判据①] ORIG 0x08465dd0 `test %al,%al; je 0x08465fb4`（无 xor、失败块在后）
        // ⇒ 原码是 `if (moveNextStage(...)) { 成功 } else { 失败 }`；守卫否定式（旧形）
        // 在 4.4 生 `xor $1; test; je <成功路径>`，失败块被提前且落点错。
        if (m_stage.moveNextStage(m_dungeonMgr.getEndStage()))
        {
            CMap* map = m_dungeonMgr.getStageMap(m_stage.getCurrentStage());
            if (!map)
            {
                // [P6] ORIG 0x08465e04 `movl $&_ZTI10ENUM_ERROR,0x4(%esp)`（0x8b75f40
                // = typeinfo for ENUM_ERROR，nm 实证）⇒ 抛的是 ENUM_ERROR 而非 int。
                throw (ENUM_ERROR)0x15;
            }

            m_stage.reset();
            if (!m_stage.consistMap(m_dungeonMgr, map, this))
            {
                throw (ENUM_ERROR)1;
            }

            CPacketHandler packet;
            packet.put_header(0, 0x8f);
            m_stage.makeStagePacket(packet);
            packet.finalize(true);
            m_party->send_to_party(packet);

            _checkMemberReady();
            return 1;
        }

        CPacketHandler packet;
        packet.makeCommonError((ENUM_CMDPACKET)0x30, (ENUM_ERROR)0x16);
        m_party->send_to_party(packet);
        return 0;
    }
    catch (ENUM_ERROR e)
    {
        // ORIG：catch 后广播 makeCommonError(0x30, e)
        CPacketHandler packet;
        packet.makeCommonError((ENUM_CMDPACKET)0x30, (ENUM_ERROR)e);
        m_party->send_to_party(packet);
        return 0;
    }

    // [b28d-G11 试形·已回滚] 曾按「ORIG 槽分配序 die@-0x1c → state@-0x28 →
    // 成功尾段@-0x34 → catch@-0x40 → 失败块@-0x4c + 失败块物理位于 landing pad
    // 0x08465e8c..0x08465f13 与成功尾段 0x08465f18 之后」把 makeCommonError(0x16)
    // 失败块移出 try/catch（B24D-CDeathTower 块位置还原）⇒ fresh 读数
    // ours 246→248、行级残差 250→**276 劣化** ⇒ 回滚到 try 内 then 之后原位置。
    // 维持既有判词：残差 = N104（EH 尾块相对序）+ N82（帧/槽），语义 CLEAR（p167）。
