// ORIG 0x084ed222 (W)：map.insert(make_pair(letterId,true))，返回 insert 的 bool
//（新插入 true / 已存在 false；ORIG movzbl insert 返回 pair.second）。
bool CMailBox::InsertLetterKeepCount(unsigned int letterId)
{
    return m_letterKeepCount
        .insert(std::make_pair(letterId, true)).second;
}