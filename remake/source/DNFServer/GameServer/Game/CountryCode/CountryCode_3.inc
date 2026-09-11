CountryCode::CountryCode(wchar_t const* a2Code)
{
    int idx;
    if (a2Code[0] > 0x60 && a2Code[0] <= 0x7a)
    {
        idx = a2Code[0] * 0x1a - 0x9da;
    }
    else if (a2Code[0] > 0x40 && a2Code[0] <= 0x5a)
    {
        idx = a2Code[0] * 0x1a - 0x69a;
    }
    else
    {
        m_idx = 0;
        return;
    }
    if (a2Code[1] > 0x60 && a2Code[1] <= 0x7a)
    {
        idx += a2Code[1] - 0x61;
    }
    else if (a2Code[1] > 0x40 && a2Code[1] <= 0x5a)
    {
        idx += a2Code[1] - 0x41;
    }
    else
    {
        m_idx = 0;
        return;
    }
    m_idx = ccTable2[idx];
    if (m_idx == 0xff)
    {
        m_idx = 0;
    }
}