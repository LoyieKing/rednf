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
    // vector<IntervalSet*>::C1Ev 为 ORIG-only out-of-line 模板实例（N89），保留声明。
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
    for (std::vector<WindowsExeFile*>::iterator it = m_exeFiles.begin();
         it != m_exeFiles.end(); ++it)
    {
        WindowsExeFile* exe = *it;
        exe->MakeCommonWindowsExeConstantSections(*this);
    }
    m_curSectionIndex = 0;                                   // this+0x11c
    m_curOffset = m_intervals[0].m_start;                    // operator[](this,0) 首字
    return true;
