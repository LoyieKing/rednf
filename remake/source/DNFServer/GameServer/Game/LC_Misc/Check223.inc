unsigned char Secu_MonsterCheck::Check223()
{
    if (m_field08 == m_curKillCount)
        return 1;
    char result = 1;
    // b28c Main（采纳 C28L7 spec）：m_active223 已 bool 化，此处须**裸测**——
    // `!= 0` 使 4.4 物化 `cmpl $0,-off; setne %al; mov %al,-off2`（ORIG 为
    // `cmpl $0,-0x10; jg` 直分支），并连带局部槽整体平移 4（P219/P22 同族）。
    if (m_active223)
    {
        int dx = static_cast<int>(static_cast<unsigned short>(m_field0a)) -
                 static_cast<int>(static_cast<unsigned short>(m_curX));
        int dy = static_cast<int>(static_cast<unsigned short>(m_field0c)) -
                 static_cast<int>(static_cast<unsigned short>(m_curY));
        // P222/P47 块序翻正 + 三目汇合：ORIG @0x0827ae09..0x0827ae40 三出口
        // 各自 `setle/setne %al` 到**寄存器**再统一 `test %al,%al; je`
        // （dx>0 → 远处 setne 块，内层 `-dx>0xc7`/`dy<=0` 同为 jg 跳走）。
        // 具名 bool moveOk 会被 4.4 落槽（-0x9 槽 + cmpb/ja + 帧漂 4），
        // 故把三出口写成嵌套三目直接作 if 条件，令值留 %al。
        if (dx <= 0 ? (-dx > 0xc7 || dy <= 0 ? (-dy <= 0x95) : (dy != 0))
                    : (dx != 0))
        {
            m_field0e += 1;
            // P196 符号性鉴别：ORIG @0x0827ae60/0x0827aec6 为 `cmp $0xa,%ax;
            // jbe` / `cmp $0x4,%ax; jbe`（**16 位无符号**域），4.4 把 ushort
            // 提升 int 后恒产有符号 `jle`。值域 0..N 不越 0x7fff ⇒ 非真语义
            // bug，但为对齐 ORIG 需把比较留在 unsigned 域。
            // b28d-36（P222 外层翻正）：ORIG 0x0827ae60 `cmp $0xa,%ax;
            // jbe +0x127` ⇒ **else 块（>10，含上报/清零）物理在前、then 块
            // （<=10 的 >4 分支）在 +0x127**。旧写 `if (<=10) {…} else {…}`
            // 使 4.4 把 then 放前（镜像）。外层条件反写 `if (>10) {else-body}
            // else {then}` 后 4.4 直产 `jbe +0x127` 与 ORIG 逐条一致。
            if ((unsigned int)m_field0e > 0xa)
            {
                m_field10 = 3;
                if (m_user != 0)
                {
                    m_user->getHackAnalyzer()->addServerHackCnt(
                        m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xdf),
                        3, 0, 0);
                }
                result = 0;
            }
            else
            {
                if ((unsigned int)m_field0e > 4)
                    m_field10 = 1;
            }
        }
        else
        {
            if (m_field10 == 1 && m_user != 0)
            {
                m_user->getHackAnalyzer()->addServerHackCnt(
                    m_user, static_cast<WongWork::ENUM_HACKTYPE>(0xdf),
                    1, 0, 0);
            }
            m_field08 = m_curKillCount;
            m_field0a = m_curX;
            m_field0c = m_curY;
            m_field0e = 0;
            m_field10 = 0;
        }
    }
    return result;
