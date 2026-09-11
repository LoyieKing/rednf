void CGameMasterMgr::HandleRequest(CCommand* cmd)      // 0x084a8c6e
{
    // b26 NEAR 封顶:N7(虚调用 callee 寄存器 %ecx↔%eax 互换)。
    m_junior->HandleRequest(cmd);                       // ORIG：junior vtable[0]
}