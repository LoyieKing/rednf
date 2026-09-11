// ---- GuildParameterScript::getGuildExpBook（ORIG T 0x08979672）----
// m_map10.find(key) 命中返回 second，否则 0（W7-BigThree increase_status case 0x10 依赖）。
int GuildParameterScript::getGuildExpBook(int key)
{
    std::map<int, int>::iterator it = m_map10.find(key);
    if (it != m_map10.end()) return it->second;
    return 0;
}