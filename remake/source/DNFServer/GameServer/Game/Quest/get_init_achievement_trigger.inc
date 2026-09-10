bool Quest::get_init_achievement_trigger(unsigned short& t0,
                                         unsigned short& t1,
                                         unsigned short& t2) const
{
    if (m_type != 7)
    {
        return 0;
    }
    unsigned short* out[3] = { 0, 0, 0 };
    out[0] = &t0;
    out[1] = &t1;
    out[2] = &t2;
    *out[0] = 1;
    *out[1] = 0;
    *out[2] = 0;
    for (unsigned int i = 0;
         i < (m_field90.size() < 3 ? m_field90.size() : 3); ++i)
    {
        *out[i] = (unsigned short)m_field90[i];
    }
    return 1;
