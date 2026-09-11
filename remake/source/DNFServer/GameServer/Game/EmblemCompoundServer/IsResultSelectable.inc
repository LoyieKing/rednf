bool EmblemCompoundServer::IsResultSelectable(int grade)
{
    // P0：ORIG 将 m_result[grade]（map::operator[] 返回）右值直传解引用，无
    // 具名局部槽（0x08191af1 `mov (%eax),%eax` 直连 `movzbl 8(%eax)`）。
    return m_result[grade]->m_selectable != 0;
}