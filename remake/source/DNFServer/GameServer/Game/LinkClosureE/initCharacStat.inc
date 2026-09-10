void InitializeCharacStat::initCharacStat(_Additioal_info& info,
                                          unsigned int characNo, int job,
                                          int level, int grow1, int grow2)
{
    // ORIG 0x08145e70..0x08145ed3：七条独立守卫各自 jne 直跳函数尾
    // [D08 2026-09-10] C05 范式③试验：七守卫合并为单条 `||` 短路 goto
    // （4.4 对短路 or 逐条 test/jne 直达同一尾标签，企消 7 组 jmp+nop 岛）。
    if (info.m_field08 != 0 || info.m_field0c != 0 || info.m_field0a != 0
        || info.m_field0e != 0 || info.m_field00 != 0 || info.m_field04 != 0
        || info.m_field42 != 0)
        goto L_done;

    info.clear();
    if (job < 0 || job > 10 || level > 0x46 || level < 1)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x1f, 5);
        trace("[ARAD,STAT,RESET] Invalid character job. charac %u, job %d, level %d",
              characNo, job, level);
        goto L_done;
    }

    {
        // ORIG 0x08145f4c..0x08145fb9：结构体拷贝（对齐剥离 + rep movsl + 尾字节）
        _Additioal_info base = *reinterpret_cast<const _Additioal_info*>(
            reinterpret_cast<const char*>(G_CDataManager()->m_characList) +
            job * 0x7dc);

        const void* stat1 =
            G_CDataManager()->m_characList->m_characs[job].get_growtype_stat(0, 0);
        const void* stat2 =
            G_CDataManager()->m_characList->m_characs[job].get_growtype_stat(grow1, 0);
        const void* stat3 =
            G_CDataManager()->m_characList->m_characs[job].get_growtype_stat(grow1, grow2);
        if (stat1 == 0 || stat2 == 0 || stat3 == 0)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x2d, 5);
            trace("[ARAD,STAT,RESET] Invalid character grow type. charac %u, first %d, second %d",
                  characNo, grow1, grow2);
            goto L_done;
        }

        info = base;
        for (int i = 1; i < level; ++i)
        {
            if (i <= 0xe)
                info.add(*reinterpret_cast<const _Additioal_info*>(stat1));
            else if (i <= 0x31)
                info.add(*reinterpret_cast<const _Additioal_info*>(stat2));
            else
                info.add(*reinterpret_cast<const _Additioal_info*>(stat3));
        }

        // ORIG 0x0814616e..0x081461e8：12 个 vararg 全为表达式（右起求值，前 9 个 spill、
        // 末 3 个落 %edi/%esi/%ebx）⇒ 不设具名局部。
        cMyTrace(__PRETTY_FUNCTION__, 0x41, 0)(
            "[ARAD][RESET STAT] Init Character : %d, stat : hp %d, mp %d, pyatt %d, pydef %d, mgatt %d, mgdef %d, speed %d, element %d, element %d, element %d, element %d)",
            characNo, (int)info.m_field00, info.m_field04, (int)info.m_field08,
            (int)info.m_field0a, (int)info.m_field0c, (int)info.m_field0e,
            info.m_field42, (int)(short)info.m_field10[0],
            (int)(short)info.m_field10[1], (int)(short)info.m_field10[2],
            (int)(short)info.m_field10[3]);
    }
L_done:;
