// ORIG C1 0x8371694：构造 vector +0x0/map +0xc 后各自 clear()（+0x27/+0x35 call）。
stBroadCastItemScript::stBroadCastItemScript() : m_vec0(), m_mapC()
{
    m_vec0.clear();
    m_mapC.clear();
}