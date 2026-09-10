int CLocalChinaErrorDispatcher::doErrorDispatcher(CUser* user, unsigned short cmd) const
{
    if (user == 0) return 0;
    // ORIG 0x0812af87：null 岛后 ushort→int 物化具名槽（-0x18），find key 与四次
    // Proc 实参均自该槽 lea/reload（枚举 lvalue 直传免临时槽，ORIG `lea -0x18` 实证）。
    const ENUM_CMDPACKET cmdVal = (ENUM_CMDPACKET)cmd;
    std::map<ENUM_CMDPACKET, _PacketErrorCheck>::const_iterator it =
        m_checks.find(cmdVal);
    if (it == m_checks.end()) return 0;
    int result = 0;
    const _PacketErrorCheck& chk = it->second;
    if (chk.m_antiAddiction)
        { result = ProcAntiAddiction(*user, cmdVal); if (result != 0) return result; }
    if (chk.m_safeMode)
        { result = ProcSafeModeUser(*user, cmdVal); if (result != 0) return result; }
    if (chk.m_safeModeAfterLogin)
        { result = ProcSaefModeAfterLogin(*user, cmdVal); if (result != 0) return result; }
    if (chk.m_lockSeriaRoom)
        { result = ProcLockInSeriaRoom(*user, cmdVal); if (result != 0) return result; }
    return 0;
