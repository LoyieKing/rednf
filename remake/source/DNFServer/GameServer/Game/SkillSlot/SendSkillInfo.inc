void SkillSlot::SendSkillInfo(int characIdx, ENUM_SKILL_TREE_KIND kind)
{
    if (m_parent == 0)
    {
        return;
    }
    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x1e5);
    guard.put_byte(1);
    guard.put_byte(characIdx);
    guard.put_byte(
        (int)(char)m_parent->m_characList[characIdx].m_job);
    guard.put_byte(
        (int)((const _Charac_info_nibble_field*)&m_parent->m_characList[characIdx])
            ->m_specialNibble);
    int index = guard.get_index();
    int count = 0;
    guard.put_byte(0);
    _Mastered_skill* buf = NULL;
    if (kind == -1 || kind == 0)
    {
        buf = m_skillSlot;
    }
    else
    {
        buf = m_skillSlot2;
    }
    for (int i = 0; i < 204; ++i)
    {
        if (buf[i].m_index != 0)
        {
            guard.put_byte(i);
            guard.put_byte((unsigned char)buf[i].m_index);
            guard.put_byte((int)(char)buf[i].m_level);
            count = count + 1;
        }
    }
    guard.put_byte(index, count);
    guard.finalize(true);
    m_parent->Send(guard);
}