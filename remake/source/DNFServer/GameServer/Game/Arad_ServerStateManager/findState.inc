// ---------------------------------------------------------------------------
// findState —— ORIG 0x081a9c2a（非 const 方法）：
// map::find 命中 → out = it->second 返回 true；否则 memset(out,0,0xc) 返回 false。
// ---------------------------------------------------------------------------
bool Arad_ServerStateManager::findState(ENUM_GLOBAL_EFFECT_ARAD code,
                                        AradServerStateMessage& out)
{
    std::map<ENUM_GLOBAL_EFFECT_ARAD, AradServerStateMessage>::iterator it =
        m_stateMap.find(code);
    if (it != m_stateMap.end())
    {
        out = it->second;
        return true;
    }
    memset(&out, 0, sizeof(out));
    return false;
}