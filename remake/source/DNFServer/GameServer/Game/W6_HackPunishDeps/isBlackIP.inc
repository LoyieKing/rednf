bool CAutoPunishRuleBlackIPMgr::isBlackIP(char const* ip)
{
    // b28d-solo-isBlackIP: **AE 111/111**（check_tu_routed fresh）。历次修复：
    // ipBuf 0x12（4×movl+movw 内联清零，原 0x10 且 i+2 可越界到 0x11）、lastSeg
    // 无符号化（复现 mem 直比 + ja/jb）、命中判定改 `A && B` 语句表达式
    // （P226 零槽短路物化）。本轮 N98 槽序翻正：把 seg 从循环体内提为**函数头
    // union 视图**（`union { char m_c[4]; unsigned int m_w; }`）并在循环内经
    // `seg.m_w = 0` 写整字 —— 同时满足 ORIG 的三条直证：seg 槽 -0x34（浅于
    // cit1 -0x38/cit2 -0x3c）、`movl $0,-0x34(%ebp)` 单条、且该清零仍在循环体内
    // （命中才清）。对照：块内 `char seg[4]={0}` 被 4.4 排到最深槽 -0x3c；函数头
    // `char seg[4]={0}` 槽号虽对但清零被提到序言（DIFF）。ORIG 0x084eb63e。
    char ipBuf[0x12] = {0};                    // 4×movl + movw 内联清零
    strncpy(ipBuf, ip, 0x10);                  // strncpy(ipBuf, param, 0x10)

    unsigned long netAddr = 0;                 // -0x30
    unsigned char lastSeg = 0;                 // -0xd（ORIG cmp 为 8 位无符号比较）
    // P212/P309 + P246：ORIG 槽序 seg(-0x34) 浅于 it(-0x38)/endIt(-0x3c)，且
    // `movl $0,-0x34` 在循环体内（命中才清零）⇒ 函数头声明 union 视图占槽、
    // 循环内经 `m_w` 写整字（4.4 对块内 `char seg[4]={0}` 给最深槽 -0x3c 且
    // 函数头初始化会把清零提到序言）。ORIG 0x084eb63e。
    union { char m_c[4]; unsigned int m_w; } seg;
    for (int i = 0xf; i >= 0; --i)             // i=-0xc 起始 0xf，not+shr 判 ≥0
    {
        if (ipBuf[i] == '.')                   // cmp $0x2e,%al
        {
            seg.m_w = 0;                       // movl $0,-0x34(%ebp) 单条
            strcpy(seg.m_c, &ipBuf[i + 1]);
            lastSeg = (unsigned char)atoi(seg.m_c); // mov %al,-0xd(%ebp)
            ipBuf[i + 1] = '0';                // movb $0x30
            ipBuf[i + 2] = '\0';               // movb $0x0
            netAddr = inet_addr(ipBuf);        // inet_addr(ipBuf) -> netlong
            break;
        }
    }

    std::multimap<unsigned long, stIPInfo_t>::const_iterator it =
        m_map.lower_bound(netAddr);
    std::multimap<unsigned long, stIPInfo_t>::const_iterator endIt =
        m_map.upper_bound(netAddr);
    for (; it != endIt; ++it)
    {
        // ORIG: ja L_false / jb L_false / mov $1 / L_false: mov $0 / test;je 迭代。
        if (({
                it->second.m_minIp <= lastSeg &&
                it->second.m_maxIp >= lastSeg;
            }))
        {
            return true;                       // 区间覆盖 -> $1
        }
    }
    return false;                              // range 尽 -> $0
