void CUser::reqHumanCertify4ClearMap(bool flag)
{
    // ORIG 0x086802c6：`movzbl -0x1c(%ebp),%eax; xor $0x1,%eax; test %al,%al;
    //   jne <mov $1 岛>` ⇒ 非具名 bool 局部，而是短路链首项取反；且短路求值序为
    //   getPoint() 先（%ebx ← 0x086802e1）、get_rand_int() 后（0x086802ef cmp）。
    //   正向包裹复现 fall-through 主体（P214①/P226）。
    if (!flag || m_mcap.getPoint() > (unsigned int)get_rand_int(0x2710))
    {
        // b28d-solo-CUser p167：ORIG 0x08680312 调 getPoint 3 次（条件
        //   `(double)getPoint() >= 100.0` 与体内 `point = getPoint()` 分开）⇒ 条件直调。
        if ((double)m_mcap.getPoint() >= 100.0)
        {
            unsigned int point = m_mcap.getPoint();
            // ORIG 0x08680350..0x08680364：`mul $0x51eb851f`(无符号 /100) → `mov $0x64;
            // sub` → 结果直入 max 首实参槽（-0x14），**无额外 (int) 中转槽** ⇒ cast 外移。
            int rate = std::max((int)(100 - point / 100), 0);
            if (rate < getStdDropRate())
            {
                if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                        RestrictCategory::Enum_3, 1))
                {
                    setStdDropRate(rate);
                    // ORIG 0x086803d8..0x08680400：getStdDropRate 结果留 %ebx 直跨
                    // get_acc_id 调用后入实参槽（**无具名 nowRate 槽**）⇒ P183 内联。
                    // 4.4 右→左实参求值顺序使 getStdDropRate 先算，与 ORIG 一致。
                    DB_SaveUserHandicap::makeRequest(get_acc_id(), 0x9c41, 0x65,
                                                     getStdDropRate(), 1, 0);
                }
            }
        }

        if (reqHumanCertify())
        {
            setHumanCertified(false);
            if (0x7ffffffe < m_humanCertifyTimerKey)
                m_humanCertifyTimerKey = 0;
            m_humanCertifyTimerKey += 1;
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, GetUID(),
                                        (TIMER_MESSAGE)9, 0x3c,
                                        m_humanCertifyTimerKey, (unsigned int)-1);
        }
    }
