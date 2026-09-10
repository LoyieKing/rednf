int GameWorld::find_session(unsigned int accId)
{
    std::map<unsigned int, unsigned short>::iterator it;
    it = m_sessionList.find(accId);
    if (it == m_sessionList.end())
    {
        return 0;
    }
    // b28c-lane3：历史 register asm("ax") 绑定按 P0/N96 清除。
    // ORIG 0x86c4ca0-0x86c4ca4 双 movzwl（movzwl 0x4(%eax),%eax; movzwl %ax,%eax）。
    return it->second;
