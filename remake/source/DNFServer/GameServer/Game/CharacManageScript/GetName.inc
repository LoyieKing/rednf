// ---------------------------------------------------------------------------
// CharacManageScript::GetName —— ORIG T 0x088e1166
// m_basicExplain[key] 命中 → 拷贝 string，未命中 → 空串（std::string 返回值）。
// ---------------------------------------------------------------------------
std::string CharacManageScript::GetName(unsigned int key)
{
    std::map<int, std::string>::iterator it = m_basicExplain.find((int)key);
    if (it != m_basicExplain.end())
        return it->second;
    return std::string();
}