bool WindowsExeFile::MakeCommonWindowsExeConstantSections(
    const IntervalSet& intervals)                            // 0x085741c6
{
    // [b28d-solo-WindowsPackFiles] verdict=DIFF，N 封顶（N82/N95 槽号 + 4.4 C1Ev 展开差）：
    // p167 CLEAR 20/20（调用点多集全等，无发明/缺失调用）；fresh 150/164（我方少 14 条）。
    // 已复现：头部 4 枚构造序（iterator C1Ev×2 + Interval C1Ev×2，ORIG +0x8..+0x2f）、
    // 短路物化守卫 `call size; test; je <mov$1>; call sections.size; test; jne <mov$0>`、
    // 尾部 dead size 具名 store（P171）。残余 = 4.4 槽基恒 ebp-8 起致全帧槽号浅移 4
    // （N95）+ 4.4 对 Interval 死局部 C1Ev 的内联/中转差（ORIG 4.1 逐条发射），无语义缺口。
    // ORIG 0x085741c6 逐段：两枚 Interval 默认构造（local_48 + local_40 槽对）
    // → clear（在合法检查**前**）→ `intervals.size()==0 || sections.size()==0`
    // 的短路物化链（jne 落 mov$0 ⇒ 第二项为 ==0，勿写 !=）→ 双层迭代器循环；
    // src 四字段无条件拷入独立 uint 槽后再判；尾部一次 dead size() 调用。
    // [b28d-solo-WindowsPackFiles] ORIG 头部 4 枚构造（+0x8..+0x2f）：iterator C1Ev、
    // const_iterator C1Ev、Interval C1Ev ×2 ⇒ 两个 Interval 死局部（P232 帧律 +
    // Interval 有用户默认构造 W@0x08575e58）。守卫为短路物化形
    // `call size; test; je <mov$1>; call sections.size; test; jne <mov$0>` ⇒ 合并 ||。
    // 极性判读（0x08574225）：sections.size()!=0 ⇒ jne +0x6a(mov $0=bad false)，
    // 即坏条件为 `intervals.size()==0 || sections.size()==0`（**==**，勿写 !=）。
    std::vector<WindowsExeConstantSection*>::iterator sit0;  // ORIG 头部两枚迭代器
    std::vector<Interval>::const_iterator it0;               // 默认构造 call
    Interval range0;
    Interval range1;
    (void)range0;
    (void)range1;
    m_commonSections.clear();
    if (intervals.m_intervals.size() == 0 || m_sections.size() == 0)
    {
        return false;
    }
    for (std::vector<Interval>::const_iterator it = intervals.m_intervals.begin();
         it != intervals.m_intervals.end(); ++it)
    {
        Interval range = *it;                                // local_40/3c = start/size
        for (std::vector<WindowsExeConstantSection*>::iterator sit = m_sections.begin();
             sit != m_sections.end(); ++sit)
        {
            WindowsExeConstantSection* sec = *sit;           // local_18
            unsigned long s0 = sec->start;                   // local_38
            unsigned long s1 = sec->fileOffset;              // local_34
            unsigned long s2 = sec->size;                    // local_30
            unsigned long s3 = sec->flag;                    // local_2c
            if ((s0 <= (unsigned long)range.m_start) &&
                ((unsigned long)range.m_start < s0 + s2))
            {
                WindowsExeConstantSection* item = new WindowsExeConstantSection;
                item->start      = range.m_start;
                item->size       = range.m_size;
                item->flag       = s3;
                item->fileOffset = s1 + ((unsigned long)range.m_start - s0);
                m_commonSections.push_back(item);
                break;                                       // 首个匹配即止
            }
        }
    }
    // ORIG 尾部 `call size; mov %eax,-0xc(%ebp)`（P171 死写入照抄：4.4 对裸
    // `m_commonSections.size();` 会 DCE，须经具名局部保留 store）。
    unsigned long deadSize = m_commonSections.size();
    (void)deadSize;
    return true;
