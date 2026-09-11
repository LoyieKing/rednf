// GetMemberPosition ORIG 0x848fd7c W（objdump 逐 switch 分支核对）：
// 返回 (int)(this+off)，size 写入 *size，found 写入 m_memberFlag[idx] 字节值。
// 映射表（ENUM 值 → flag idx / off / size）：
//   0x07→0/+0x00000/0x703  0x08→1/+0x00703/0x1544c  0x0a→3/+0x015b4f/0x7608
//   0x0b→4/+0x01d157/0x19c 0x0c→5/+0x01d2f3/0x1810  0xb3→7/+0x01eb03/0x10
//   0x11→8/+0x01eb13/0xcf1 0x19→a/+0x01f804/0x51c  0x20→b/+0x01fd20/0x14b60
//   0x282→9/+0x034884/0xafcc   未命中→return 0（不写 size/found）。
int CACHE_CHARACTER_TYPE::GetMemberPosition(ENUM_INTERNALPACKET type,
                                            int* size, bool& found)
{
    switch (static_cast<int>(type))
    {
    case 0x07:
        found = m_memberFlag[0];
        *size = static_cast<int>(sizeof(m_charac));
        return reinterpret_cast<int>(&m_charac);
    case 0x08:
        found = m_memberFlag[1];
        *size = static_cast<int>(sizeof(m_inventory));
        return reinterpret_cast<int>(&m_inventory);
    case 0xb3:
        found = m_memberFlag[7];
        *size = static_cast<int>(sizeof(m_blackList));
        return reinterpret_cast<int>(&m_blackList[0]);
    case 0x0a:
        found = m_memberFlag[3];
        *size = static_cast<int>(sizeof(m_quest));
        return reinterpret_cast<int>(&m_quest);
    case 0x0b:
        found = m_memberFlag[4];
        *size = static_cast<int>(sizeof(m_ticket));
        return reinterpret_cast<int>(&m_ticket[0]);
    case 0x0c:
        found = m_memberFlag[5];
        *size = static_cast<int>(sizeof(m_dungeonClear));
        return reinterpret_cast<int>(&m_dungeonClear[0]);
    case 0x11:
        found = m_memberFlag[8];
        *size = static_cast<int>(sizeof(m_actionPoint));
        return reinterpret_cast<int>(&m_actionPoint);
    case 0x282:
        found = m_memberFlag[9];
        *size = static_cast<int>(sizeof(m_expand));
        return reinterpret_cast<int>(&m_expand);
    case 0x19:
        found = m_memberFlag[0xa];
        *size = static_cast<int>(sizeof(m_altar));
        return reinterpret_cast<int>(&m_altar);
    case 0x20:
        found = m_memberFlag[0xb];
        *size = static_cast<int>(sizeof(m_etc));
        return reinterpret_cast<int>(&m_etc);
    default:
        return 0;
    }
}