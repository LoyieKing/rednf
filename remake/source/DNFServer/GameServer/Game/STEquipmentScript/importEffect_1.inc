bool importEffect(effect::STEffect& effect)
{
    // [b28d-solo-STEquipmentScript 判词·N] 0x088c1a0d：帧/槽号平移 + STL 模板内联差，
    // p167 CLEAR（40/40）⇒ 语义闭合，byte 层 N82/N89 封顶。
    // [SweepSTEquipScript 2026-09-09 重判] 本轮两处修正：①`[/module]` 由 break 改
    // `continue`（ORIG 0x088c1de1 `jne 088c1a36` 回循环头）并补末尾 `else break`
    // （ORIG 0x088c1dfc `je 088c1e11` 是循环唯一出口）——旧译文遇到 `[/module]`
    // 即提前返回，跳过其后所有标签（真语义缺口）；②[option] 子循环改
    // `while(true){ if(!ScanStr(&value)) break; ... }` 复现 ORIG 0x088c1c2a
    // `xor $1; test; je` 取反物化（P292）。gap 4 → 0（314/314）。
    std::string tag;
    std::string value;
    while (true)
    {
        ScanType(tag, true);

        if (tag == "[attach target]")
        {
            ScanStr(&value);
            if (value == "delegator")
                effect.m_a = 0;
            else if (value == "target")
                effect.m_a = 1;
        }
        else if (tag == "[type]")
        {
            ScanStr(&value);
            if (value == "animation")
                effect.m_b = 0;
            else if (value == "particle")
                effect.m_b = 1;
        }
        else if (tag == "[attach pos]")
        {
            ScanStr(&value);
            if (value == "bottom")
                effect.m_c = 2;
            else if (value == "middle")
                effect.m_c = 1;
            else if (value == "top")
                effect.m_c = 0;
        }
        else if (tag == "[z revision]")
        {
            effect.m_4 = ScanInt((bool*)0);
        }
        else if (tag == "[index]")
        {
            effect.m_8 = ScanInt((bool*)0);
        }
        else if (tag == "[option]")
        {
            // 位标志子循环：ScanStr 失败即回到外层标签循环（ORIG 88c1c32）。
            // ORIG 0x088c1c2a 形 = `call; xor $0x1,%eax; test %al,%al; je <循环体>`
            // （P292 bool 取反物化）⇒ 源形是 `if (!ScanStr(&value)) break;`。
            while (true)
            {
                if (!ScanStr(&value))
                    break;
                if (value == "keep direction")
                    effect.m_c_ |= 0x01;
                else if (value == "floor")
                    effect.m_c_ |= 0x02;
                else if (value == "auto remove")
                    effect.m_c_ |= 0x04;
                else if (value == "apply custom color")
                    effect.m_c_ |= 0x08;
                else if (value == "sync speed")
                    effect.m_c_ |= 0x10;
                else if (value == "copy horizon")
                    effect.m_c_ |= 0x20;
                else if (value == "copy vertical")
                    effect.m_c_ |= 0x40;
                else if (value == "resize")
                    effect.m_c_ |= 0x80;
            }
        }
        else if (tag == "[module]")
        {
            effect.m_10 = scanModuleType();
        }
        else
        {
            // [2026-09-10] ORIG 0x088c1de1 `test %al,%al; jne <循环头>`：匹配
            // "[/module]" → 回循环头（continue 语义），不匹配直落 [file name] 检查。
            // 两条 continue 路径都写成**落到链尾**（无显式 continue）⇒ 复现
            // ORIG 的「匹配→跳循环头 / 不匹配→直落」正条件布局（显式 continue
            // 会被 4.4 编成 `je <跳过>; nop; jmp <循环头>`）。
            if (tag == "[/module]")
            {
                // 空体：匹配后落到链尾（= continue）
            }
            else if (tag == "[file name]")
            {
                ScanStr(&effect.m_str14);
            }
            else
            {
                // ORIG 0x088c1dfc `je 088c1e11`：未知标签 → 退出循环（0x404 析构 +
                // `mov $0x1,%ebx` 返回 true）。
                break;
            }
        }
    }
    return true;
