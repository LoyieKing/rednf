// ----------------------------------------------------------------------------
// setHackCnt —— ORIG W 0x084eb854 (0x17f)：HackTypeInfo_t(type1,u16(value1),
// type2,u16(value2),u16(v5),u16(v6),v7) 以 key=type1 insert；
// 若 type2 != 0 再以 key=type2 insert 第二条。
// ----------------------------------------------------------------------------
void CAutoPunishRuleHackTypeMgr::setHackCnt(ENUM_HACKTYPE type1, unsigned int value1,
                                            ENUM_HACKTYPE type2, unsigned int value2,
                                            unsigned int v5, unsigned int v6,
                                            unsigned int v7)
{
    // b28d-solo-setHackCnt: ORIG make_pair 实例第二参为 HackTypeInfo_t&&（mangled OS9_）
    // ⇒ 原码为匿名右值临时；具名 info 使 4.4 把对象排到最深处。ORIG 0x084eb854。
    m_hackTypeMap.insert(std::make_pair(
        type1, HackTypeInfo_t(type1, (unsigned short)value1, type2,
                              (unsigned short)value2, (unsigned short)v5,
                              (unsigned short)v6, v7)));

    if (type2 != (ENUM_HACKTYPE)0)             // test 0x14(ebp)
    {
        m_hackTypeMap.insert(std::make_pair(
            type2, HackTypeInfo_t(type1, (unsigned short)value1, type2,
                                  (unsigned short)value2, (unsigned short)v5,
                                  (unsigned short)v6, v7)));
    }
}