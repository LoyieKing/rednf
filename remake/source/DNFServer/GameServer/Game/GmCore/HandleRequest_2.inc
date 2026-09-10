void CJuniorGameMaster::HandleRequest(CCommand* cmd)  // 0x084a8c06
{
    // b26 NEAR 封顶:N7(虚调用 callee 寄存器 %edx↔%eax 互换)。
    if (cmd->HasJuniorPermission())             // ORIG：vtable[2]，test %al
    {
        cmd->executeTemplate();
    }
    else
    {
        IGameMaster::HandleRequest(cmd);        // 非虚转发 m_next（ORIG 直呼基类实现）
    }
