void Avatar_Item::setEmblemSocketData(const stAvatarEmblemInfo_t& emblem)
{
    // ORIG 0x1d 字节 = 7×mov 链 + 尾 ushort（4.1 分块启发式）；结构整体赋值
    // 试 rep movsl 对比（N90/N84 族）。
    m_emblemInfo = emblem;
