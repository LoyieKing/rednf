bool StringResource::get(int id, const CharString** out) const
{
    std::map<int, CharString>::const_iterator it = m_strings.find(id);
    // [b28c-lane17] ORIG 0x08ad4898：`operator!=` + 成功块直落、失败 mov$0 归尾块
    // ⇒ 正极性包裹（P220 迭代器比较极性）。
    if (it != m_strings.end())
    {
        *out = &it->second;
        return true;
    }
    return false;
}