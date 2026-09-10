void CRidable::CheckAppearRidableObject(CParty*, int a, int b, PacketGuard* p)
{
    // ORIG 0x082fe9d1 段序（P28/P50「先声明后赋值」）：begin() 最先
    //   （0x082fe9d1..de，it 落 -0x24）→ counter(-0x1c)=0 → idx(-0x28)=get_index()
    //   → `put_byte(counter)`（ORIG `mov -0x1c(%ebp),%edx` 读槽变量，非立即数 0）
    //   → jmp 底部条件。
    std::map<int, DungeonRidableObject>::iterator it = m_objects.begin();
    int counter = 0;
    int idx = p->get_index();
    p->put_byte(counter);
    for (; it != m_objects.end(); ++it)
    {
        if (it->second.m_x == a && it->second.m_y == b)
        {
            if (it->second.m_state == 4 ||
                (it->second.m_state == 3 && m_flag18 != 0))
            {
                it->second.m_state = 1;
                ++counter;
                SendSpawnRidableObject(p, it->second.m_field10,
                                       it->second.m_field14,
                                       it->second.m_field0c,
                                       it->second.m_field18);
            }
        }
    }
    p->put_byte(idx, counter);
