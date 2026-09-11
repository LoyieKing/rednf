MSG_CHANGE_AVATAR_OPTION::MSG_CHANGE_AVATAR_OPTION()
    : m_consumeSlot(0xffff), m_consumeItemIdx(-1), m_avatarSlot(0xffff),
      m_equipItemIdx(-1), m_option(0xff)
{
    // ORIG 0x0822e0cc：MSG_BASE C1 后 +0x0d word=-1/+0x0f dword=-1/+0x13
    // word=-1/+0x15 dword=-1/+0x19 byte=0xff（初始列表序一致）。
}