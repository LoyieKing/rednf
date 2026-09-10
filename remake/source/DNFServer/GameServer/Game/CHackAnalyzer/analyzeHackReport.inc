int CHackAnalyzer::analyzeHackReport(CUser* user, PacketBuf* pkt)
{
    // ORIG 0x080f7a22（analyzeHackReport.md）：头 2 字节标志位驱动各检测分支；
    // 失败读包路径一律 return 0，成功路径 return 1；flags&0x100 时对 param_1
    // 直连其分析器加 0x32c 计数。
    // ORIG 局部 IO 字段统一位于 -0x4c 基址的 packed 缓冲：+0xd flags(u16)、
    // +0xf speedMs(u32)、+0x13 byteVal(u8)、+0x40 memVal(u32)；地址链
    // lea -0x4c;add off 形（帧 0x78）。
    struct __attribute__((packed))
    {
        unsigned char pad0[0xd];
        unsigned short flags;      // +0xd
        unsigned int speedMs;      // +0xf
        char byteVal;              // +0x13
        unsigned char pad1[0x40 - 0x14];
        unsigned int memVal;       // +0x40
    } buf;
    if (!stGetHackTypeOperator<unsigned short, 2>::getHackType(
            pkt, *(unsigned short*)(char*)&buf.pad0[0xd]))
        return 0;

    if (m_baseHackType == 0)
        return 1;

    if ((buf.flags & 0x1) != 0)
    {
        if (!pkt->get_int(*(unsigned int*)(char*)&buf.pad0[0xf]))
            return 0;
        _analyzeSpeedHack(buf.speedMs);
    }
    if ((buf.flags & 0x2) != 0)
    {
        _analyzeDifficultyHack('\0');
        ((CUser*)m_baseHackType)->DisConnSig((DISCONN_SIG)0x18, false, false);
    }
    if ((buf.flags & 0x8) != 0)
    {
        if (!pkt->get_int(*(unsigned int*)(char*)&buf.pad1[0x2c]))
            return 0;
        _analyzeMemoryHack(buf.memVal);
    }
    if ((buf.flags & 0x4) != 0)
    {
        _addHackInfo(REPORT_4_HACK_FLAG_EQUIPMENT_UPGRADE,
                     (ENUM_HACKTYPE)0x3c5, 0);
        ReqDBSaveHackUserPunish(((CUser*)m_baseHackType)->get_acc_id(), 0xa8);
        ((CUser*)m_baseHackType)->DisConnSig((DISCONN_SIG)0x18, false, false);
    }
    if ((buf.flags & 0x80) != 0)
    {
        _addHackInfo(REPORT_4_HACK_FLAG_MONSTER_DIE, (ENUM_HACKTYPE)0x3bd, 0);
    }
    if ((buf.flags & 0x10) != 0)
    {
        if (!pkt->get_byte(*(char*)&buf.byteVal))
            return 0;
        _addHackInfo(REPORT_4_HACK_FLAG_PAYLOAD_BYTE, (ENUM_HACKTYPE)0x3be,
                     (int)*(char*)&buf.byteVal);
    }
    if ((buf.flags & 0x20) != 0)
    {
        _addHackInfo(REPORT_4_HACK_FLAG_MISC, (ENUM_HACKTYPE)0x3c6, 0);
    }
    if ((buf.flags & 0x100) != 0)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (ENUM_HACKTYPE)0x32c,
                                                  1, 0, 0);
    }
    return 1;
