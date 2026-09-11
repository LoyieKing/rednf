int CObfuscator::ObfuscatePacket(unsigned char* data, int len, int type) // 0x085858ea
{
    ReadData(data, len);
    // ORIG 0x085858ea +0x1b..+0x46：`mov 0x14(%ebp),%eax; cmp $0x1,%eax; je; 
    // cmp $0x2,%eax; je; jmp 尾` ⇒ 值升序 switch 的 4.1 决策树形（P165②）；
    // 旧 if/else-if 链被 4.4 做成 `cmpl $1,mem; jne` + 第二比较后置（少 2 insns）。
    switch (type)
    {
    case 1:
        Obfuscate1();
        break;
    case 2:
        Obfuscate2();
        break;
    default:
        break;
    }
    return reinterpret_cast<int>(m_buffer);
}