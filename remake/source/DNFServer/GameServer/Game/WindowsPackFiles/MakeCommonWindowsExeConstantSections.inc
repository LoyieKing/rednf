// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82/N95）→ 试=槽集合比对 + 帧/寄存器轴：
//   ORIG `push %esi; push %ebx; sub $0x60`（callee-saved 保 sec/range 中转）vs OURS
//   `sub $0x78`（无 push，全落栈槽）；两枚 Interval ORIG -0x20/-0x24 vs OURS -0x30/-0x34
//   （整体深 0x10）；余差 8 条全为 4.4 栈槽 vs 4.1 callee-saved 的取数/回写（0x085741c6 段）
//   ⇒ 维持 N82/N95（ours 156 / ORIG 164，gap +8；P331 声明序轴已试）。
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
    // [M3S2WinDrop 2026-09-11] 本波落地 3 项（150→155，gap 14→9；四数 112/242/near2/diff3 不变）：
    //   ① 两处迭代器改「先声明后赋值」（ORIG 0x08574243/0x0857427f begin() 经临时槽后
    //      `mov -0x4c,%eax; mov %eax,-0x24/-0x20`）+4 条 mov；`++` 移到体尾；
    //   ② 新增未初始化 `bool matched`（ORIG 0x08574337 独有 `movb $0x1,-0xd`，
    //      无初始化 store、从未被读）+1 条。
    //   残余 9 条 = 4.4 指令/寄存器选择，判 N：③ ORIG 匹配体用 `%ecx/%ebx/%esi`
    //      中转算 `s1+(start-s0)`（0x0857430c..0x1430 八条）vs 4.4 `sub mem,%edx;
    //      add mem,%edx`（2 条）；④ 因用 callee-saved ⇒ ORIG 有 push %esi/%ebx +
    //      epilogue `lea S(%esp); add $0x0,%esp; pop %ebx/%esi/%ebp`（6 条）vs 4.4 `leave`；
    //      ⑤ ORIG 四字段读取共用 `%eax=sec`、载入目标 `%edx`（0x085742ac），4.4 逐语句
    //      用 `%eax` 载入致 3 条 sec reload（试过逗号表达式单语句：读数不变，N）；
    //      ⑥ 首条件 ORIG `mov s0,%edx; cmp %eax,%edx; ja`（0x085742c6）vs 我方 `cmp mem; jb`（等价）。
    // 极性判读（0x08574225）：sections.size()!=0 ⇒ jne +0x6a(mov $0=bad false)，
    // 即坏条件为 `intervals.size()==0 || sections.size()==0`（**==**，勿写 !=）。
    std::vector<WindowsExeConstantSection*>::iterator sit0;  // ORIG 头部两枚迭代器
    std::vector<Interval>::const_iterator it0;               // 默认构造 call
    Interval range0;
    Interval range1;
    (void)range0;
    (void)range1;
    // [M3S2WinDrop] ORIG 头内连续 4 个 uint 槽（-0x38/-0x34/-0x30/-0x2c，P339）⇒
    // src 四字段槽提到函数头（深→浅 = s0,s1,s2,s3；赋值仍在循环体内）。
    unsigned long s0, s1, s2, s3;
    // [M3S2WinDrop] ORIG -0xd 槽：匹配体尾 `movb $0x1,-0xd(%ebp)`（0x08574337，
    // 无初始化 store、从未被读）⇒ 未初始化 bool 局部 + 命中置位。
    bool matched;
    m_commonSections.clear();
    if (intervals.m_intervals.size() == 0 || m_sections.size() == 0)
    {
        return false;
    }
    // [M3S2WinDrop] ORIG 0x08574243/0x0857427f：两处迭代器均为**先声明后赋值**
    // （begin() 经临时槽 -0x4c 后 `mov -0x4c,%eax; mov %eax,-0x24/-0x20`，各 +2 条 mov），
    // 非 for 初始化式直构；`++` 位于循环体尾（ORIG +0x1b4/+0x177）。
    it0 = intervals.m_intervals.begin();
    while (it0 != intervals.m_intervals.end())
    {
        Interval range = *it0;                               // local_40/3c = start/size
        sit0 = m_sections.begin();
        while (sit0 != m_sections.end())
        {
            WindowsExeConstantSection* sec = *sit0;          // local_18
            s0 = sec->start;                                 // local_38
            s1 = sec->fileOffset;                            // local_34
            s2 = sec->size;                                  // local_30
            s3 = sec->flag;                                  // local_2c
            // [N] 逗号表达式（单语句连续赋值）试过：读数不变（154），4.4 仍逐条
            // reload sec 指针（`mov S(%ebp),%eax`），不采用 ORIG 的 %edx 载入形。
            if ((s0 <= (unsigned long)range.m_start) &&
                ((unsigned long)range.m_start < s0 + s2))
            {
                WindowsExeConstantSection* item = new WindowsExeConstantSection;
                item->start      = range.m_start;
                item->size       = range.m_size;
                item->flag       = s3;
                item->fileOffset = s1 + ((unsigned long)range.m_start - s0);
                m_commonSections.push_back(item);
                matched = true;                              // ORIG movb $1,-0xd
                break;                                       // 首个匹配即止
            }
            ++sit0;
        }
        ++it0;
    }
    // ORIG 尾部 `call size; mov %eax,-0xc(%ebp)`（P171 死写入照抄：4.4 对裸
    // `m_commonSections.size();` 会 DCE，须经具名局部保留 store）。
    unsigned long deadSize = m_commonSections.size();
    (void)deadSize;
    return true;
}