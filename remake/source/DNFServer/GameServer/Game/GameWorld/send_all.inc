// ============================================================================
// 广播
// ============================================================================
void GameWorld::send_all(PacketGuard& packet)
{
    // ORIG 0x86c8c1a-0x86c8c35：无默认构造 call、begin sret 直落唯一迭代器槽
    // -0x18 ⇒ for-init 内声明初始化形（外声明+赋值会产 ctor call+双槽拷贝）
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            user->Send(packet);
        }
    }
}