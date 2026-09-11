bool WindowsExeFileFamily::CompleteLoadingWindowsExeFiles()  // 0x08574736
{
    // [b28d-solo-WindowsPackFiles] verdict=DIFF，N 封顶（N95 槽号 + N89 模板实例）：
    // p167 CLEAR 37/37（调用点多集全等）；fresh 277/279（gap=2）。
    // 已复现：头部 3 枚独立 C1Ev（iterator<WindowsExeFile>@-0x3c、
    // iterator<WindowsExeConstantSection>@-0x40、vector<IntervalSet*>@-0x4c —— 迭代器
    // 先声明后赋值 P50）、sets[3] 倒计数构造循环、指针级 swap_value<IntervalSet*>、
    // 双层 while（每轮 end/operator!=/operator*/ppEv）、尾部 operator=+Sort+第二循环。
    // 残余 = 4.4 全帧槽号较 ORIG 浅 4（N95：4.4 槽基恒 ebp-8 起）与
    // vector<IntervalSet*>::C1Ev 的 out-of-line/inline 差（N89），无语义缺口。
    // ORIG 0x08574736 全形转写：sets[3] 逐元素构造 + 三个 IntervalSet* 槽做
    // **指针级** swap_value<IntervalSet*>（非对象 std::swap）；未使用的局部
    // std::vector<IntervalSet*>（ORIG local_50 ctor/dtor，0x08574757/尾部）；
    // 双层迭代器循环（begin 一次入槽 + 每轮 end/operator!=/operator*/++）。
    // [b28d-solo-WindowsPackFiles] ORIG 头部 3 枚独立 C1Ev（+0x8..+0x36）：
    // iterator<WindowsExeFile>@-0x3c、iterator<WindowsExeConstantSection>@-0x40、
    // vector<IntervalSet*>@-0x4c ⇒ 迭代器**先声明后赋值**（P50，for 内直构会消 C1Ev）。
    // [M3S2WinDrop 2026-09-11] 本波落地 2 项（277→279，gap 0；verdict DIFF→**NEAR**，
    // TU 四数 diff 4→3 / near 1→2，identical 112 / ae 242 不变）：
    //   ① 第二循环复用 it0（ORIG 0x085749a8 begin() 经 -0x8c 临时后 `mov -0x8c,%eax;
    //      mov %eax,-0x3c` = 先声明后赋值；for 初始化式会原地直构、少 2 条 mov）；
    //   ② 尾部两句交换为 `m_curOffset` 先、`m_curSectionIndex = 0` 后（ORIG 0x08574a3c
    //      `mov %edx,0x120(%eax)` 先于 `movl $0x0,0x11c(%eax)`）。
    // 余差仅槽基/寄存器（LCS 槽号归一后 ratio=1.0000）。
    // vector<IntervalSet*>::C1Ev 为 ORIG-only out-of-line 模板实例（N89），保留声明。
    // [N] P331 声明序反转（unusedVec/sit0/it0）试过：读数不变（279/279）、严格差异行
    // 仍 115/279 ⇒ 4.4 槽分配不随声明序 ⇒ 回滚。
    // [Q8 2026-09-11 旧N判词重扫] 旧判=NEAR（N89/N95）→ 试=①槽集合比对（ORIG 279=OURS 279，
    //   助记符全同仅操作数异）②prett/串审计：prettchk（Ndx!=UND 修正判据）0 缺口、p167 37/37
    //   ③声明序轴 P331 已试 ④残余为两组不一致平移：C1Ev 组 ORIG -0x3c/-0x40/-0x4c vs OURS
    //   -0x38/-0x3c/-0x48（浅 4）、对象基 ORIG -0x7c vs OURS -0x80（深 4）、bool ORIG -0x19 vs
    //   OURS -0x25（差 0xc）⇒ 4.4/4.1 槽分配器固有差（非单调平移）⇒ 维持 NEAR（gap 0）。
    std::vector<WindowsExeFile*>::iterator it0;
    std::vector<WindowsExeConstantSection*>::iterator sit0;
    std::vector<IntervalSet*> unusedVec;
    IntervalSet sets[3];
    IntervalSet* pCur = &sets[0];
    IntervalSet* pPrev = &sets[1];
    IntervalSet* pInter = &sets[2];
    bool first = true;

    it0 = m_exeFiles.begin();
    while (it0 != m_exeFiles.end())
    {
        WindowsExeFile* exe = *it0;
        pPrev->Clear();
        sit0 = exe->m_sections.begin();
        while (sit0 != exe->m_sections.end())
        {
            WindowsExeConstantSection* sec = *sit0;
            Interval iv(sec->start, sec->size);
            pPrev->AddInterval(iv);
            ++sit0;
        }
        if (first)
        {
            swap_value(pCur, pPrev);
            first = false;
        }
        else
        {
            IntersectIntervalSet(pInter, *pCur, *pPrev);
            if (pInter->m_intervals.size() == 0)
            {
                return false;
            }
            swap_value(pCur, pInter);
        }
        ++it0;
    }

    if (pCur->m_intervals.size() == 0)
    {
        return false;
    }
    this->IntervalSet::operator=(*pCur);                     // IntervalSet::operator=
    Sort();
    // [M3S2WinDrop] ORIG 0x085749a8 第二循环**复用 it0 槽(-0x3c)**：begin() 经临时槽
    // (local_8c) 后 `mov -0x8c,%eax; mov %eax,-0x3c`（先声明后赋值形，+2 条 mov），
    // 非 for 初始化式直构（4.4 会原地构造、0 拷贝）。
    it0 = m_exeFiles.begin();
    while (it0 != m_exeFiles.end())
    {
        WindowsExeFile* exe = *it0;
        exe->MakeCommonWindowsExeConstantSections(*this);
        ++it0;
    }
    // [M3S2WinDrop] ORIG 0x08574a3c 顺序直证：先 `mov %edx,0x120(%eax)`（m_curOffset）
    // 后 `mov 0x8(%ebp),%eax; movl $0x0,0x11c(%eax)`（m_curSectionIndex=0）。
    m_curOffset = m_intervals[0].m_start;                    // operator[](this,0) 首字
    m_curSectionIndex = 0;                                   // this+0x11c
    return true;
}