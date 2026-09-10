void CCalcCharacStat::InitializeStat(_Additioal_info* out, int job)
{
    out->clear();
    switch (job)
    {
    case 0:                                   // +0x042 分支
        out->m_field00 = 0x5dc;
        out->m_field04 = 0x5dc;
        out->m_field08 = 0x32;
        out->m_field0a = 0x32;
        out->m_field0c = 0x3c;
        out->m_field0e = 0x32;
        out->m_field10[2] = 200;
        out->m_field10[3] = 0xff38;
        out->m_field3a = 400000;
        out->m_field40 = 0x96;
        out->m_field42 = 0x2134;
        out->m_field46 = 0x2134;
        out->m_field48 = 7000;
        out->m_field4a = 6000;
        out->m_field4c = 0x10cc;
        out->m_field4e = 680000;
        break;
    case 1:                                   // +0x0db 分支
        out->m_field00 = 0x708;
        out->m_field04 = 0x5dc;
        out->m_field08 = 0x3c;
        out->m_field0a = 0x3c;
        out->m_field0c = 0x32;
        out->m_field0e = 0x32;
        out->m_field10[2] = 0xff38;
        out->m_field10[3] = 200;
        out->m_field3a = 300000;
        out->m_field40 = 0x82;
        out->m_field42 = 0x238c;
        out->m_field46 = 0x251c;
        out->m_field48 = 10000;
        out->m_field4a = 6000;
        out->m_field4c = 0x125c;
        out->m_field4e = 500000;
        break;
    case 2:                                   // +0x174 分支
        out->m_field00 = 0x4e2;
        out->m_field04 = 0x708;
        out->m_field08 = 0x28;
        out->m_field0a = 0x28;
        out->m_field0c = 0x46;
        out->m_field0e = 0x3c;
        out->m_field3a = 330000;
        out->m_field40 = 0xaf;
        out->m_field42 = 0x2008;
        out->m_field46 = 0x251c;
        out->m_field48 = 8000;
        out->m_field4a = 6000;
        out->m_field4c = 0x1324;
        out->m_field4e = 600000;
        break;
    case 3:                                   // +0x1fb 分支
        out->m_field00 = 1000;
        out->m_field04 = 2000;
        out->m_field08 = 0x1e;
        out->m_field0a = 0x1e;
        out->m_field0c = 0x50;
        out->m_field0e = 0x46;
        out->m_field3a = 280000;
        out->m_field40 = 200;
        out->m_field42 = 8000;
        out->m_field46 = 10000;
        out->m_field48 = 10000;
        out->m_field4a = 5000;
        out->m_field4c = 0xdac;
        out->m_field4e = 400000;
        break;
    default:
        break;                                // 其余值：仅 clear 后返回
    }
