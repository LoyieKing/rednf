// ============================================================================
// GetGrowthWeaponEvent（ORIG T 0x08447ea6）
// ============================================================================

bool DB_Login::GetGrowthWeaponEvent(SIG_LOGIN_DATA* sig)
{
    // P1 组件视图：ORIG 清块/写块单指令 disp（movb/movw/movl $imm,0x414x(%eax)）
    struct __attribute__((packed)) GWView {
        char m_pad[0x4140];
        char m_has;                            // +0x4140
        char m_pad1[3];
        unsigned int m_characNo;               // +0x4144
        unsigned int m_weaponId;               // +0x4148
        unsigned int m_infWeaponId;            // +0x414c
        unsigned short m_timepiece;            // +0x4150
        unsigned short m_serverId;             // +0x4152
    };
    bool ok;
    MySQL* db;
    // [b28c-L9] **P0 清绑**：原 `register CEventBase* evt asm("eax") = …` 系历史寄存器
    // 绑定（N96 已登记全域禁用），删绑后按 P226/BM11 正向路径改**单语句内联形**——
    // ORIG +0x1b 的 GetRepeatEvent 结果无槽往返（`mov (%eax),%edx; …; mov %eax,(%esp);
    // call *%edx`）⇒ 按 P0 原码本无具名局部，直调虚槽 +0x34 写在条件表达式内。
    if (!(GlobalData::s_event_manager->GetRepeatEvent(100))->IsEventing(0))
    {
        return 1;
    }
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect server_id, charac_no, growthweapon_id, timepiece, infinityweapon_id from event_1207_growthweapon where m_id = %d",
        sig->m_accId);
    ok = db->exec(true);                       // ORIG %al→-0xd 槽回读
    if (!ok)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__, 0xb487,
                              "LoadGrowthWeaponEvent Query Error : m_id(%d)",
                              sig->m_accId);
        return 0;
    }
    if ((bool)(db->get_n_rows() != 0))
    {
        if (!db->fetch())
        {
            return 0;
        }
        ((GWView*)sig)->m_has = 1;
        if (!db->get_short(0, SGFC(short, sig, OFF_GROWTH_WEAPON + 0x12))) return 0;   // server_id
        if (!db->get_int(1, SGFC(int, sig, OFF_GROWTH_WEAPON + 4))) return 0;          // charac_no
        if (!db->get_int(2, SGFC(int, sig, OFF_GROWTH_WEAPON + 8))) return 0;          // growthweapon_id
        if (!db->get_short(3, SGFC(short, sig, OFF_GROWTH_WEAPON + 0x10))) return 0;   // timepiece
        if (!db->get_int(4, SGFC(int, sig, OFF_GROWTH_WEAPON + 0xc))) return 0;        // infinityweapon_id
    }
    else
    {
        // ORIG 清块写入序：每 store 重读 sig（无跨 store 缓存）
        ((GWView*)sig)->m_has = 0;                           // +0x20f movb
        ((GWView*)sig)->m_serverId = 0;                      // +0x219 movw
        ((GWView*)sig)->m_characNo = 0;                      // +0x225 movl
        ((GWView*)sig)->m_weaponId = 0;                      // +0x232 movl
        ((GWView*)sig)->m_timepiece = 0;                     // +0x23f movw
        ((GWView*)sig)->m_infWeaponId = 0;                   // +0x24b movl
    }
    return 1;
}
