CountryCode::CountryCode(char const* a2Code)
{
    int idx;
    if ((signed char)a2Code[0] > 0x60 && (signed char)a2Code[0] <= 0x7a)
    {
        idx = (int)(char)(unsigned char)a2Code[0] * 0x1a - 0x9da;
    }
    else if ((signed char)a2Code[0] > 0x40 && (signed char)a2Code[0] <= 0x5a)
    {
        idx = (int)(char)(unsigned char)a2Code[0] * 0x1a - 0x69a;
    }
    else
    {
        m_idx = 0;
        return;
    }
    if ((signed char)a2Code[1] > 0x60 && (signed char)a2Code[1] <= 0x7a)
    {
        idx += (int)(char)(unsigned char)a2Code[1] - 0x61;
    }
    else if ((signed char)a2Code[1] > 0x40 && (signed char)a2Code[1] <= 0x5a)
    {
        idx += (int)(char)(unsigned char)a2Code[1] - 0x41;
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
