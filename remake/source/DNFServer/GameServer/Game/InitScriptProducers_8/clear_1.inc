// ORIG clear 0x08962606：+0x04 置 7（无层哨兵），+0x08/+0x0c/+0x10 清零；
// 不触碰 m_name。
void seriaRoomDecoAniInfo::clear()
{
    m_layer = 7;
    m_a = 0;
    m_b = 0;
    m_c = 0;
}