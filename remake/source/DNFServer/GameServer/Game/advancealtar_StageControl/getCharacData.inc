// ORIG W 0x081348a0：return (CharacData*)(this + 0x98)。
// [BigP3D 成员化 2026-09-12] m_characData 由字节槽改真实成员 ⇒ 取址 `&m_characData`
// （等价 this+0x98；sizeof/偏移/mangled 零变）。
CharacData* StageControl::getCharacData()
{
    return &m_characData;
}