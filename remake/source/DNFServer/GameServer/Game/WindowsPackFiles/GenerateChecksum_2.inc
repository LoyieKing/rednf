bool WindowsDataFileFamily::GenerateChecksum(CodeHackChecksumEx* ckEx,
                                             CodeHackCheckRangeEx* rangeEx) // 0x08575302
{
    // ORIG 0x08575302：GenerateRange 失败 → return 0（与 ExeFileFamily 恒 1 不同，
    // 原码两版行为各异，逐版照抄）；rangeEx 回写在成功块内。
    // [b28d-solo-WindowsPackFiles] 三处照 ORIG 改形：
    // ① 0x08575361 `call GenerateRange; test %al,%al; je <mov $0,%eax>`（无 xor）
    //    ⇒ 正向 `if (GenerateRange(...)) { … return true; } return false;`；
    // ② 循环是迭代器形（C1Ev@0x08576bb6 + begin 临时@-0x4c + end 临时@-0x10 +
    //    operator*@-0xc 具名 f + 前置 ++）——索引 for 缺 4 个模板调用点（P167）；
    // ③ rangeEx 两字段从 range 槽(-0x24/-0x20)重读回写（非直读 rangeStart/Len）。
    // [b28d-solo-WindowsPackFiles] ORIG 槽（深→浅）：sum@-0x44, it0@-0x1c,
    // rangeLen@-0x18, rangeStart@-0x14。4.4「先声明浅」⇒ it0 须声明在标量之后。
    unsigned long rangeStart;
    unsigned long rangeLen;
    std::vector<WindowsDataFile*>::iterator it0;
    CodeHackChecksum sum;
    ckEx->Clear();                                           // CodeHackChecksumData::Clear
    ckEx->SetFileName(m_name);
    rangeEx->SetFileName(m_name);
    if (GenerateRange(&rangeStart, &rangeLen))
    {
        CodeHackCheckRange range;
        range.m_start = static_cast<int>(rangeStart);
        range.m_size  = static_cast<int>(rangeLen);
        rangeEx->m_range = range;                            // P247：8 字节视图整体赋值
        it0 = m_files.begin();
        while (it0 != m_files.end())
        {
            WindowsDataFile* f = *it0;
            f->GenerateChecksum(&sum, rangeStart, rangeLen);
            ckEx->SetCodeHackCheckRange(range);
            ckEx->AddCodeHackChecksum(sum);
            ++it0;
        }
        return true;
    }
    return false;
}