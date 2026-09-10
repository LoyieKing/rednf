void checkNewMemberHacklog(unsigned int hackType, CUser* user,
                           unsigned int p3)
{
    if (user == 0)
        return;

    // [b28d-solo-GameSecurity] ORIG 0x8275315 → NEAR（200=200，助记符序列全等）。
    // 修法三则：① year/mon/mday 先算入具名局部（槽 -0x2c/-0x28/-0x24），tm 字段
    // 随后按 isdst→year→mon→mday→hour→min→sec 逐条 store；② 五个 hack 分支为
    // if / else-if 链（正极性 `days>0 && days<=3` 等），else-if 守卫各落独立
    // nop+jmp 岛；③ 无 etc 具名槽（P0）：SetETC 结果 `mov %eax,%ebx` 直接升格
    // callee-saved 跨 getHackAnalyzer 保活 ⇒ 写 addServerHackCnt 第 4 实参内联。
    // 残余 N-P75：ORIG `difftime(now,t)` 先载 t→%edx 再载 now→%eax 再压栈；
    // 4.4 恒逐个求值即压（`mov t,%eax; mov %eax,0x4(%esp); mov now,%eax; …`）。
    // 探针穷举（time_t/int/double cast/具名 double）均不翻，压栈序/调用约定/语义全同。
    struct tm tmv;
    int year = static_cast<int>(hackType / 10000);
    unsigned int mon = (hackType / 100) % 100;
    int mday = static_cast<int>(hackType % 100);

    tmv.tm_isdst = -1;
    tmv.tm_year = year - 0x76c;
    tmv.tm_mon = static_cast<int>(mon) - 1;
    tmv.tm_mday = mday;
    tmv.tm_hour = 0;
    tmv.tm_min = 0;
    tmv.tm_sec = 0;

    time_t t = mktime(&tmv);
    time_t now = time(0);
    // ORIG `mov t,%edx; mov now,%eax; mov %edx,0x4(%esp); mov %eax,(%esp);
    // call difftime` —— 直接传 time_t（无 double cast）；返回 double 先
    // fstpl -0x18 具名槽再 fldl 参与除法（P252 槽往返）。
    double diff = difftime(now, t);
    int days = static_cast<int>(diff / 86400.0);

    if (days < 0)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, static_cast<WongWork::ENUM_HACKTYPE>(0x134), 1,
            user->SetETC(0x134, static_cast<int>(p3)), hackType);
    }
    else if (days == 0)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, static_cast<WongWork::ENUM_HACKTYPE>(0x134), 1,
            user->SetETC(0x134, static_cast<int>(p3)), hackType);
    }
    else if (days > 0 && days <= 3)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, static_cast<WongWork::ENUM_HACKTYPE>(0x135), 1,
            user->SetETC(0x135, static_cast<int>(p3)), hackType);
    }
    else if (days > 3 && days <= 7)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, static_cast<WongWork::ENUM_HACKTYPE>(0x136), 1,
            user->SetETC(0x136, static_cast<int>(p3)), hackType);
    }
    else if (days > 7 && days <= 0x1e)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, static_cast<WongWork::ENUM_HACKTYPE>(0x137), 1,
            user->SetETC(0x137, static_cast<int>(p3)), hackType);
    }
