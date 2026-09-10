bool WindowsExeFile::ReadPeHeader()
{
    // [SweepC 2026-09-09 修复判词] 0x085738f6：P165 双份物理展开 + 头部具名局部已还原。
    // ① 可执行（减入口头 10B）与非可执行（dir/import 解剖）两分支各持独立
    //    `IntervalSet workSet; Interval base; AddInterval; Interval range;
    //    iterator 遍历 new/push_back`（分支顺序照 ORIG：!=0 在前，==0 在后）；
    // ② 遍历走迭代器且每迭代仅 1 次 operator*（`const Interval& iv = *it;`
    //    P183 引用绑定；ORIG 08573aaa/08573fbb 各 1 次）——补回 ORIG-only
    //    callee `_ZN8IntervalC1Ev` + `__normal_iterator::operator*`；
    // ③ 头部 img@-0x68 / p@-0x60 / pe@-0x5c / sig@-0x64 / fileHdr@-0x58 /
    //    opt@-0x54 具名局部中转；节数循环条件每次重读 fileHdr+2（ORIG 085740b7）。
    // 结果：p167 calls 44/44，MISSING/EXTRA 仅 C1↔C2、D1↔D2 别名对（豁免）；
    // gap 91 → -29（我方多 29 条 = 4.4 槽基/寻址形态差 N82/N95，非语义缺口）；
    // TU identical 106→111 / ae 239→242 / near 1 / diff 3（无回退）。
    if (m_image == 0)
    {
        return false;
    }
    // [b28d-ReadPeHeader 修形②] PE 头常量偏移访问用结构视图（4.4.7 对
    // `*(T*)(base+K)` 恒 add+mov，对 `view->m_field` 折 disp，同 sh/opt 判据）。
    struct PeImageDosHeader
    {
        unsigned char m_pad[0x3c];
        unsigned long m_lfanew;                              // +0x3c e_lfanew
    };
    struct PeCoffFileHeader
    {
        unsigned short m_machine;                            // +0x00
        unsigned short m_numberOfSections;                   // +0x02
    };
    unsigned char* img = m_image;                            // -0x68
    unsigned char* p = img;                                  // -0x60 节表游标
    PeImageDosHeader* pe = reinterpret_cast<PeImageDosHeader*>(p); // -0x5c
    p = img + pe->m_lfanew;                                  // e_lfanew
    unsigned int sig = *reinterpret_cast<unsigned int*>(p);  // -0x64 PE 签名
    (void)sig;
    p += 4;
    PeCoffFileHeader* fileHdr = reinterpret_cast<PeCoffFileHeader*>(p); // -0x58 COFF 头
    p += 0x14;
    // [b28d-ReadPeHeader 修形] 常量下标必须走「数组引用」：4.4.7 对 `unsigned long*`
    // 的 `p[K]` 恒发 `mov p,%eax; add $4K,%eax; mov (%eax)`（+1 条/处），ORIG 4.1.2
    // 为 `mov p,%eax; mov 4K(%eax)`（disp 折叠）。探针 /tmp/shp2.cpp g1/g2(数组引用/
    // 结构视图→disp) vs g3(指针下标→add) 实证；本函数 20+ 处常量访问即 gap -29 主体。
    unsigned long (&opt)[56] = *reinterpret_cast<unsigned long (*)[56]>(p); // -0x54 可选头
    p += 0xe0;

    for (int s = 0;
         s < static_cast<int>(fileHdr->m_numberOfSections);
         ++s, p += 0x28)
    {
        unsigned long (&sh)[10] = *reinterpret_cast<unsigned long (*)[10]>(p);
        unsigned long vaDelta = sh[5] - sh[3];
        if (static_cast<long>(sh[9]) < 0 || (sh[9] & 0x40000000UL) == 0)
        {
            continue;
        }
        unsigned long rawLen;
        if (sh[2] < sh[4])
        {
            rawLen = sh[2];
        }
        else
        {
            rawLen = sh[4];
        }
        if ((sh[9] & 0x20000000UL) != 0)               // 可执行：仅减入口头 10B
        {
            IntervalSet workSet;
            Interval base(sh[3], rawLen);
            workSet.AddInterval(base);
            Interval entryHead(opt[4], 10);             // AddressOfEntryPoint
            workSet.MinusInterval(entryHead);
            std::vector<Interval>::iterator it;
            Interval range;
            for (it = workSet.m_intervals.begin();
                 it != workSet.m_intervals.end(); ++it)
            {
                // [b28d-ReadPeHeader 修形③] ORIG 08573aaa 一次 operator* + 两读两写
                // （`mov 0x4(%eax),%edx; mov (%eax),%eax` 后双存）= 结构整体赋值；
                // 写 `const Interval& iv = *it;` 会引入引用槽（我方 -0x58 多 4 条）。
                range = *it;
                WindowsExeConstantSection* item = new WindowsExeConstantSection;
                item->start      = range.m_start;
                item->fileOffset = range.m_start + vaDelta;
                item->size       = range.m_size;
                item->flag       = sh[9];
                m_sections.push_back(item);
            }
        }
        else                                            // 非可执行 → 解剖大分支
        {
            IntervalSet workSet;
            Interval base(sh[3], rawLen);
            workSet.AddInterval(base);
                for (long d = 0; d < static_cast<long>(opt[0x17]); ++d)
                {
                    if (IsOverlappedInterval(
                            sh[3], sh[2],
                            opt[(d + 12) * 2], opt[(d + 12) * 2 + 1]) != 0 &&
                        d != 2 && d != 6)
                    {
                        Interval dir(opt[(d + 12) * 2], opt[(d + 12) * 2 + 1]);
                        workSet.MinusInterval(dir);
                    }
                }
                bool hasImport = false;
                if (opt[0x17] >= 2)
                {
                    hasImport =
                        IsOverlappedInterval(sh[3], sh[2], opt[0x1a], opt[0x1b]) != 0;
                }
                if (hasImport)
                {
                    unsigned char* desc = img + opt[0x1a];
                    for (int e = 0;
                         *reinterpret_cast<unsigned int*>(desc + e * 0x14) != 0 ||
                         *reinterpret_cast<unsigned int*>(desc + e * 0x14 + 0x10) != 0;
                         ++e)
                    {
                        unsigned long thunkRva =
                            *reinterpret_cast<unsigned long*>(desc + e * 0x14);
                        unsigned char* thunk = img + thunkRva + vaDelta;
                        int n = 0;
                        for (; *reinterpret_cast<unsigned int*>(thunk + n * 4) != 0; ++n)
                        {
                            long hintVal =
                                *reinterpret_cast<long*>(thunk + n * 4);
                            if (hintVal < 0)
                            {
                                // ordinal 导入：ORIG 取低 16 位入槽（local_2a），
                                // 不做减法
                                unsigned short ord =
                                    static_cast<unsigned short>(hintVal);
                                (void)ord;
                            }
                            else
                            {
                                unsigned long nameOff =
                                    static_cast<unsigned long>(hintVal) + vaDelta;
                                (void)*reinterpret_cast<unsigned short*>(
                                    img + nameOff);                     // hint 字
                                unsigned long total = 2;
                                char* nm = reinterpret_cast<char*>(img + nameOff + 2);
                                if (nm != 0)
                                {
                                    size_t sl = strlen(nm);
                                    unsigned long len = static_cast<unsigned long>(sl) + 1;
                                    if ((static_cast<char>(len) +
                                         static_cast<char>(nameOff) & 1) != 0)
                                    {
                                        len = static_cast<unsigned long>(sl) + 2;
                                    }
                                    total = 2 + len;
                                    Interval name(hintVal, total);
                                    workSet.MinusInterval(name);
                                }
                            }
                        }
                        unsigned long dllRva =
                            *reinterpret_cast<unsigned long*>(desc + e * 0x14 + 0x0c);
                        if (dllRva != 0)
                        {
                            unsigned long dl = 0;
                            char* dn = reinterpret_cast<char*>(img + dllRva);
                            size_t sdl = strlen(dn);
                            dl = static_cast<unsigned long>(sdl) + 1;
                            if ((static_cast<char>(dl) +
                                 static_cast<char>(thunkRva + vaDelta) & 1) != 0)
                            {
                                dl = static_cast<unsigned long>(sdl) + 2;
                            }
                            Interval dll(dllRva, dl);
                            workSet.MinusInterval(dll);
                        }
                        Interval table(thunkRva,
                                       static_cast<unsigned long>((n + 1) * 4));
                        workSet.MinusInterval(table);
                    }
                }
            std::vector<Interval>::iterator it;
            Interval range;
            for (it = workSet.m_intervals.begin();
                 it != workSet.m_intervals.end(); ++it)
            {
                range = *it;                             // 同上：结构整体赋值（ORIG 08573fbb）
                WindowsExeConstantSection* item = new WindowsExeConstantSection;
                item->start      = range.m_start;
                item->fileOffset = range.m_start + vaDelta;
                item->size       = range.m_size;
                item->flag       = sh[9];
                m_sections.push_back(item);
            }
        }
    }
    return true;
