// ORIG _ZN13user_creature12CDBMsgSender4InitE19ENUM_INTERNALPACKETP5CUser W 0x833f7a8
//（objdump 逐条转写）：type==0x89 走 BigStreamPool::Acquire("Creature.cpp",0xa7)，
// 其余走 StreamPool::Acquire("Creature.cpp",0xa9)；stream 空→false；
// m_user=user；CStreamGuard(m_stream,false) 后经 operator*()（de 0x80c8c46）链式
// 依次 << (int)type、<< (int)GetUID()；return true 落在 guard 作用域内
//（ORIG：mov $1,%ebx 先物化 → guard D1 → mov %ebx,%eax 共享出口）。
// 池记录名取 ORIG .rodata 0x8c2d76a 字面量 "Creature.cpp"（AE 口径归 <A>，语义等价）。
bool user_creature::CDBMsgSender::Init(ENUM_INTERNALPACKET type, CUser* user)
{
    if ((int)type == ENUM_INTERNALPACKET_0x89)
        m_stream = GlobalData::s_big_stream_pool->Acquire("Creature.cpp", 0xa7);
    else
        m_stream = GlobalData::s_stream_pool->Acquire("Creature.cpp", 0xa9);
    if (m_stream == 0)
        return false;
    m_user = user;
    CStreamGuard guard(m_stream, false);
    (*guard)->operator<<((int)type);
    (*guard)->operator<<((int)m_user->GetUID());
    return true;
}