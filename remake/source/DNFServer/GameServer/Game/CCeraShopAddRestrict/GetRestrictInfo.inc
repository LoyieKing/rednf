InfoDaily* ScriptLoader::GetRestrictInfo(unsigned int ipgNo)
{
    std::map<unsigned int, InfoDaily*>::iterator it = m_infoMap.find(ipgNo);
    // [b28c-4 P222] ORIG 0x08abf766：`je 零块(fall-through); body; jmp end` 布局
    // ⇒ 正向包裹 `if (it != end) return it->second;`（否定早返会产反向 je 布局）。
    if (it != m_infoMap.end())
        return it->second;
    return 0;
}