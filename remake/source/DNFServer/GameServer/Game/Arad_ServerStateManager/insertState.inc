// ---------------------------------------------------------------------------
// insertState(arad::SigServerState*) —— ORIG 0x081a92ca：
// Guard 内遍历 sig->count 条目，逐条 insertStateNoSync(tag.code, tag.msg)。
// ORIG 键读取自 sig + i*20 + 0x9 == tagData.m_code；消息位于 sig+i*20+0xd == tagData.m_msg。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::insertState(arad::SigServerState* sig)
{
    Guard<Mutex> guard(&m_lock);
    for (int i = 0; i < sig->m_count; ++i)
    {
        insertStateNoSync((ENUM_GLOBAL_EFFECT_ARAD)sig->m_data[i].m_code,
                          sig->m_data[i].m_msg);
    }
}