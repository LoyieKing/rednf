// ---- SendUsers（ORIG T 0x85c7adc，报告 SendUsers.md L15-130 全文）----
// 围观列表广播，跳过 excludeA 与空指针；尾部对 excludeB 非空补发一次。返回发送数。
int CPrivateStore::SendUsers(PacketGuard* packet, CUser* excludeA, CUser* excludeB)
{
    int sentCount = 0;
    // ORIG 0x85c7adc 槽集：sentCount@-0x10、it@-0x1c（**默认构造 C1Ev + 后赋值**，
    // begin() 经 sret 临时 -0x2c 中转拷贝）、user@-0xc（函数头预置 0，循环内复用）。
    std::list<CUser*>::iterator it;
    CUser* user = 0;
    it = m_users.begin();
    while (it != m_users.end())
    {
        user = *it;
        if (user != excludeA && user != 0)
        {
            user->Send(*packet);
            ++sentCount;
        }
        it++;
    }
    if (excludeB != 0)
    {
        excludeB->Send(*packet);
        ++sentCount;
    }
    return sentCount;
}