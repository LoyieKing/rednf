bool WindowsCodeProtectorServer::GenerateChecksum(CodeHackChecksumEx* ckEx,
                                                  CodeHackCheckRangeEx* rangeEx) // 0x08575a3c
{
    // [b28d-solo-WindowsPackFiles] 按 ORIG 0x08575a3c 逐段重写：
    // ① prologue 后立即预清两 bool（`movb $0,-0xa`/`-0x9`，在 m_loaded 检查之前）；
    // ② m_loaded==0 与函数尾共享 `mov $0x0,%eax`（+0x1df）⇒ 整体用
    //    `if (m_loaded != 0) { 主体 }` 包裹 + 单一 return false；
    // ③ 四路分支为**扁平 else-if**（ORIG 每支显式写 wantExe/wantData 两字节）；
    // ④ wantData 段 `call size; cmp idx; setle`（有符号）⇒ (int)size() <= idx。
    bool wantExe = false;
    bool wantData = false;
    if (m_loaded != 0)
    {
        if (m_exeFamily == 0 && m_dataFamilies.empty())
        {
            return false;
        }
        if (m_exeFamily != 0 && m_dataFamilies.empty())
        {
            wantExe = true;
            wantData = false;
        }
        else if (m_exeFamily == 0 && !m_dataFamilies.empty())
        {
            wantExe = false;
            wantData = true;
        }
        else if (m_retryCount < m_cfgLong)
        {
            wantExe = true;
            wantData = false;
            m_retryCount += 1;
        }
        else
        {
            wantExe = false;
            wantData = true;
            m_retryCount = 0;
        }

        // ORIG 0x08575b36 `cmpb $0,-0xa; je +0x13c` + `call; test %al; je +0x13c`
        // 两守卫同落点、无 mov$1/mov$0 物化岛 ⇒ 拆嵌套 if（P133/P225）。
        if (wantExe)
        {
            if (m_exeFamily->GenerateChecksum(ckEx, rangeEx))
            {
                rangeEx->SetFileName("");
                return true;
            }
        }
        if (wantData)
        {
            // ORIG：m_dataFamilies[m_curDataFamily] 各取一次（不缓存指针）
            if (m_dataFamilies[m_curDataFamily]->GenerateChecksum(ckEx, rangeEx))
            {
                if (m_dataFamilies[m_curDataFamily]->IsScanningAddressReset())
                {
                    m_curDataFamily += 1;
                    if (static_cast<int>(m_dataFamilies.size()) <=
                        m_curDataFamily)
                    {
                        m_curDataFamily = 0;
                    }
                }
                return true;
            }
        }
    }
    return false;
}