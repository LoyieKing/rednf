// [b28d-solo-CDeathTower] verdict=DIFF（orig 252 / our 249，ndiff 223）：P167 报 MISSING
// _Unwind_Resume ×4，按符号边界计数（compare_tu 口径）**ORIG 6 处 vs 我们 2 处**。
// ORIG 的 4 个额外 landing 各自先调 CPacketHandlerD1Ev（0x8465cfe/0x8465d80/0x8465f8b/
// 0x8466004）再 _Unwind_Resume；4.4 把 try 块内多个 CPacketHandler 局部合并为共享
// cleanup 出口（N104 EH 块合并/相对序族）。Main A/B 实测：CPacketHandler ctor/dtor
// 加 throw() 对本函数零影响（两态均同计数），throw() 已回退 ⇒ N91/N104 封顶。
// 语义/异常路由（catch(int) → makeCommonError(0x30,e)）与 ORIG 一致。
// [J10 2026-09-11 复核] orig 252 / our 246（gap +6）；p167 49/45：MISSING 全为
// CPacketHandler C2/D2 与 _Unwind_Resume（前两者=P93 同址别名；Unwind 为 tail-merge 代价）。
// ORIG 四个 landing 各自收尾 `mov %eax,(%esp); call _Unwind_Resume`（0x08465d10/0x08465d92/
// 0x08465f9d/0x08466016）vs 4.4.6 以 `jmp <cmp $0x1,%edx 共享段>` 合并 ⇒ N104 EH tail-merge 封顶。
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
}