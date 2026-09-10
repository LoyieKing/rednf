void ProcessAntiBotInputWrapper()
{
    // [b28d-28] ORIG 0x080eddb7 `movl $0x1f4,-0x10(%ebp)`：死局部（P171 照抄，
    // 后续比较仍用立即数 `cmpl $0x1f4,-0xc`）。
    long limit = 500;
    (void)limit;
    timeval t0, t1;
    gettimeofday(&t0, 0);

    // [b28d-28] ORIG 0x080eddd1：无判空、无具名 fn/checker 局部——单表达式虚调
    // `mov 0x941f7e4,%eax; mov (%eax),%eax; add $0x10,%eax; mov (%eax),%edx;
    // mov 0x941f7e4,%eax; mov %eax,(%esp); call *%edx`（P282：vptr+0x10 = word[4]，
    // 外部 SDK 类未建模故保留函数指针形）。
    typedef void (*CheckerFn)(void*);
    // 函数指针值直接在调用点 cast（P0 无具名局部）：`(...)[4]` 已取到 fn 值，
    // 再经 `CheckerFn*` 解引用会多发一条 `mov (%eax),%eax`（我方 4 条 mov 链 vs ORIG 3）。
    reinterpret_cast<CheckerFn>(
        (*reinterpret_cast<void***>(GlobalData::s_antibotChecker_))[0x10 / 4])(
        GlobalData::s_antibotChecker_);

    gettimeofday(&t1, 0);
    long ms = (t1.tv_sec - t0.tv_sec) * 1000 + (t1.tv_usec - t0.tv_usec) / 1000;
    if (ms > 500)
    {
        LogManager::logFormat(1, "localchina/localchina_App.cpp",
                              __PRETTY_FUNCTION__, 42,
                              "Delay notice, ProcessAntiBotInput : %d, %ld, %ld, %ld, %ld",
                              ms, t0.tv_sec, t0.tv_usec, t1.tv_sec, t1.tv_usec);
    }
