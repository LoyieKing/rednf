bool WindowsExeFileFamily::GenerateChecksum(CodeHackChecksumEx* ckEx,
                                            CodeHackCheckRangeEx* rangeEx) // 0x08574cf0
{
    // ORIG 0x08574cf0 真形（Ghidra C 全文核读）：**恒 return 1**——GenerateRange
    // 失败只是跳过聚合块，无 false 出口（前批 return false 系误转真语义 bug）；
    // rangeEx 两字段回写位于成功块内；循环为迭代器形；sum 对象在函数头部声明。
    // [b28d-solo-WindowsPackFiles] ORIG 槽（深→浅）：sum@-0x48, it0@-0x20,
    // rangeLen@-0x1c, rangeStart@-0x18, secIdx@-0x14（4.4「先声明浅」）。
    long secIdx;
    unsigned long rangeStart;
    unsigned long rangeLen;
    std::vector<WindowsExeFile*>::iterator it0;
    CodeHackChecksum sum;
    ckEx->Clear();
    ckEx->SetFileName(m_name);
    rangeEx->SetFileName(m_name);
    // [b28d-solo-WindowsPackFiles] ORIG 0x08574cf0：① `test %al,%al; je <mov $1>`
    // （无 xor，且失败与成功共用尾块 ⇒ if 正向包裹 + 恒 return true）；
    // ② 循环为迭代器形（C1Ev + begin 临时@-0x4c + operator*@-0xc 具名 f +
    //    前置 ++ + 每次 end()）；③ rangeEx 两字段从 range 槽(-0x28/-0x24)重读回写。
    if (GenerateRange(&secIdx, &rangeStart, &rangeLen))
    {
        CodeHackCheckRange range;
        range.m_start = static_cast<int>(rangeStart);
        range.m_size  = static_cast<int>(rangeLen);
        rangeEx->m_range = range;                            // P247：8 字节视图整体赋值
        it0 = m_exeFiles.begin();
        while (it0 != m_exeFiles.end())
        {
            WindowsExeFile* f = *it0;
            f->GenerateChecksum(&sum, secIdx, rangeStart, rangeLen);
            ckEx->SetCodeHackCheckRange(range);
            ckEx->AddCodeHackChecksum(sum);
            ++it0;
        }
    }
    return true;
}