void CCommand::executeTemplate()                // 0x084b3f90
{
    // b26 NEAR 封顶:N7(两处虚调用 callee 寄存器 %edx↔%eax 互换)。
    parse();                                    // ORIG：先 vtable[1]
    execute();                                  // 后 vtable[0]
