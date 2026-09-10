void CSeniorGameMaster::HandleRequest(CCommand* cmd)  // 0x084a8c44
{
    // 链尾：校验通过即执行，失败静默丢弃。
    // b26 NEAR 封顶:N7(虚调用 callee 寄存器 %ecx↔%eax 互换)。
    if (cmd->HasSeniorPermission())             // ORIG：vtable[3]，test %al
    {
        cmd->executeTemplate();
    }
