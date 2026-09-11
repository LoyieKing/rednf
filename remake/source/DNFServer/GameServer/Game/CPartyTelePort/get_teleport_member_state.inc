char CPartyTelePort::get_teleport_member_state(int idx)
{
    if (m_party == 0)
    {
        return -1;
    }
    if (idx < 0 || idx > 3)
    {
        return -1;
    }
    // [b28-L14] ORIG 0x85bc26x 第三 null 守卫为 `test; je <尾常量块>`（主路径
    // fall-through）⇒ 正向包裹（P222 钥匙），非常规负极早返。
    if (m_party != 0)
    {
        return m_party->GetMember()[idx].m_field12;
    }
    return -1;
}