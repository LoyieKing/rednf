CLocalChinaErrorDispatcher::CLocalChinaErrorDispatcher()
{
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x32,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x1b,
                    _PacketErrorCheck(false, false, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x66,
                    _PacketErrorCheck(false, false, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x6,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x9,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x20,
                    _PacketErrorCheck(true, false, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x18,
                    _PacketErrorCheck(true, true, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x1a,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x1c,
                    _PacketErrorCheck(true, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x47,
                    _PacketErrorCheck(false, false, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x4c,
                    _PacketErrorCheck(false, false, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x53,
                    _PacketErrorCheck(true, true, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x54,
                    _PacketErrorCheck(false, false, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x55,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x58,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x61,
                    _PacketErrorCheck(false, false, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x69,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x89,
                    _PacketErrorCheck(false, false, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xac,
                    _PacketErrorCheck(true, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x9c,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xd3,
                    _PacketErrorCheck(true, false, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xba,
                    _PacketErrorCheck(false, true, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x14f,
                    _PacketErrorCheck(false, true, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xbb,
                    _PacketErrorCheck(false, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xbc,
                    _PacketErrorCheck(false, true, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xe1,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xef,
                    _PacketErrorCheck(true, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xf2,
                    _PacketErrorCheck(true, false, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x100,
                    _PacketErrorCheck(true, false, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x129,
                    _PacketErrorCheck(true, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x13c,
                    _PacketErrorCheck(false, true, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x15,
                    _PacketErrorCheck(true, false, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x1f,
                    _PacketErrorCheck(false, false, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x36,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x30,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x10,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x26,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x5d,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x65,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x61,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x9f,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xa0,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x18,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x2e,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x21,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x22,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x23,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x24,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x19,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x1d,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xad,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xee,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xf5,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x130,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x146,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x223,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x224,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x225,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xcd,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x104,
                    _PacketErrorCheck(false, true, true, false)));
