int StageTimeLineParameter::importPhase(TimeLine& timeLine)
{
    // b28d-solo-advancealtar_misc：ORIG @0x0889a930 关键源形证据——
    //   ①`bool ok` 为**函数级单槽**（0x0889a938 `movb $0x0,-0x9(%ebp)` 只在函数头
    //   初始化一次，其余全是 ScanInt(&ok) 传址 + movzbl 回读；原多分支各自
    //   `bool ok = false;` 生成 -0x29/-0x41 多个独立槽）；
    //   ②内层循环**无 phaseDone 标志**（全函数仅一条 movb 写 -0x9）⇒ 原码用
    //   `for(;;){... if (tag=="[/phase]") break; ...}`，原 while+标志多一个 bool 槽。
    bool ok = false;                                               // [objdump 0x0889a938 -0x9]
    for (;;)
    {
        std::string token;
        if (!ScanType(token, true))
        {
            return 0x2b1;                                          // token 流耗尽
        }
        if (token == "[/time line]")
        {
            return 0;
        }
        if (token == "[phase]")
        {
            Phase phase;
            for (;;)
            {
                std::string tag;
                if (!ScanType(tag, true))
                {
                    return 0x2be;                                  // [objdump 0x889a9ee 路径]
                }
                if (tag == "[/phase]")
                {
                    break;
                }
                else if (tag == "[duration]")
                {
                    phase.m_field0 = ScanInt(&ok);                 // [objdump 0x889aa36 -0x24]
                    if (!ok)
                    {
                        return 0x2c6;                              // [objdump 0x889aa48]
                    }
                }
                else if (tag == "[type]")
                {
                    std::string name;
                    if (!ScanStr(&name))
                    {
                        return 0x2cc;                              // [objdump 0x889aa8f]
                    }
                    if (name == "normal")
                    {
                        phase.m_field4 = 0;                        // [objdump 0x889aac? -0x20]
                    }
                    else if (name == "named")
                    {
                        phase.m_field4 = 1;
                    }
                    else if (name == "boss")
                    {
                        phase.m_field4 = 2;                        // [objdump 0x889aaf5]
                    }
                    else
                    {
                        return 0x2dc;                              // [objdump 0x889aafe]
                    }
                }
                else if (tag == "[action assign]")
                {
                    for (;;)
                    {
                        _ActionAssign assign;                      // [objdump 0x889ab61 ctor 每次]
                        assign.m_field0 = ScanInt(&ok);            // [objdump 0x889ab77 -0x3c]
                        if (!ok)
                        {
                            break;                                 // [objdump 0x889ab83 jne→esi=2]
                        }
                        assign.m_minAliveCount = (short)ScanInt(&ok);   // [objdump 0x889ab90 -0x34]
                        if (!ok)
                        {
                            return 0x2ed;                          // [objdump 0x889ab9f]
                        }
                        assign.m_maxAliveCount = (short)ScanInt(&ok);   // [objdump 0x889abb6 -0x32]
                        if (!ok)
                        {
                            return 0x2f0;                          // [objdump 0x889abc5]
                        }
                        phase.m_assigns.push_back(assign);         // [objdump 0x889abe1]
                    }
                }
            }
            timeLine.m_phases.push_back(phase);                    // [objdump 0x889ac27 区段]
        }
    }
