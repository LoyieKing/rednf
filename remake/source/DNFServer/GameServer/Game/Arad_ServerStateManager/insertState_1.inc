// ---------------------------------------------------------------------------
// insertState(ENUM_GLOBAL_EFFECT_ARAD, AradServerStateMessage const&)
// —— ORIG 0x081a936e：Guard 内直接 insertStateNoSync。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::insertState(ENUM_GLOBAL_EFFECT_ARAD code,
                                          const AradServerStateMessage& msg)
{
    Guard<Mutex> guard(&m_lock);
    insertStateNoSync(code, msg);
}