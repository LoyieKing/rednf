void CHackAnalyzer::ValidateFixMobDiePos(CUser* user, unsigned short mobId,
                                         unsigned short x, unsigned short y)
{
    // ORIG T 0x080f908c（ValidateFixMobDiePos.md）：查 FixMobInfo 白名单表；
    // mobId 命中而坐标出框（或 0xd6ed 特例坐标不等于框边界 xMin/xMax）时
    // addServerHackCnt(user, 0xd2, 1, 0, 0)。
    // 静态表（ORIG nm d @0x0936f9c0 size 0x84 / FixMobInfoSize @0x0936fa44
    // =11；内容 objdump -s .data 实测，第 11 项全零不参与比较循环）。
    struct FixMobInfoRec
    {
        unsigned int m_mid;       // +0x00
        unsigned short m_xMin;    // +0x04
        unsigned short m_xMax;    // +0x06
        unsigned short m_yMin;    // +0x08
        unsigned short m_yMax;    // +0x0a
    };
    static FixMobInfoRec FixMobInfo[] = {
        {0xbba,  0x468, 0x468,  0xed,  0xed },
        {0xd6ed, 0x80,  0x337,  0x9d,  0x139},
        {0x2f0,  0xb,   0xb,    0x82,  0x15e},
        {0x2ef,  0x378, 0x378,  0x82,  0x15e},
        {0xdb29, 0x4f9, 0x4f9,  0xff,  0xff },
        {0xf010, 0xffee, 0xffee, 0x120, 0x120},
        {0xf108, 0x57,  0x57,   0x175, 0x175},
        {0xef9c, 0x384, 0x384,  0xf0,  0xf0 },
        {0x3f2,  0x377, 0x377,  0xca,  0xca },
        {0xdcbb, 0x552, 0x552,  0xfb,  0xfb },
        {0,      0,     0,      0,     0    },
    };
    static unsigned short FixMobInfoSize = 11;   // ORIG 0x936fa44 movzwl 零扩展（unsigned）

    for (int i = 0; i < FixMobInfoSize - 1; ++i)
    {
        if (mobId == FixMobInfo[i].m_mid)
        {
            if (x < FixMobInfo[i].m_xMin ||
                FixMobInfo[i].m_xMax < x ||
                y < FixMobInfo[i].m_yMin ||
                FixMobInfo[i].m_yMax < y)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (ENUM_HACKTYPE)0xd2, 1, 0, 0);
                return;
            }
            if (mobId != 0xd6ed)
                return;
            if (FixMobInfo[i].m_xMin == x)
                return;
            if (FixMobInfo[i].m_xMax == x)
                return;
            user->getHackAnalyzer()->addServerHackCnt(
                user, (ENUM_HACKTYPE)0xd2, 1, 0, 0);
            return;
        }
    }
