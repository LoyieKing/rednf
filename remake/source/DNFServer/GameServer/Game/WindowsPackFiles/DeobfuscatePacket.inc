int CObfuscator::DeobfuscatePacket(unsigned char* data, int len, int type) // 0x08585938
{
    ReadData(data, len);
    // 同上（ORIG 0x08585938 同型 switch 决策树）
    switch (type)
    {
    case 1:
        Deobfuscate1();
        break;
    case 2:
        Deobfuscate2();
        break;
    default:
        break;
    }
    return reinterpret_cast<int>(m_buffer);
}