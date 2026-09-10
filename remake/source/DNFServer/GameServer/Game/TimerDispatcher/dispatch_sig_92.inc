int TimerScheduleTenSecond::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    // ORIG 0x8637936：遍历 +0x22c 用户 map；get_state()>=2 且 getAntibotKey()>=0
    // 时经 s_antibotChecker_ 虚槽 +0x14（antibot 检查，签名 (void*, seq, 0, accId)）
    // 发起防机器人校验；循环完 registNextTimer(10) 返回 1。
    // [b28d-solo-TimerDispatcher2] P167 差集 MISSING = const_iterator 族
    // （C1ERKS_ ×2 = begin()/end() 的 iterator→const_iterator 转换 + const 版
    // ptEv/ppEv/ne）、EXTRA = 非 const 版 ⇒ 原码是**非 const map 引用 + const_iterator
    // 循环变量**（若 map 引用本身 const 则 begin/end 直返 const_iterator、C1ERKS_ 不再
    // 发射，故旧「改 const 引用」形计数恶化）。ORIG 0x08637936。
    // P0/P183：ORIG 无 users 引用局部（08637936 `call G_CGameManager; lea 0x22c(%eax),%edx`
    // 每站点重发）⇒ 全表达式内联（旧具名引用多 -0x18 槽 + 2 insn）。
    // [b28e-TimerDispatcher] P13：ORIG 在循环初始化区**一次**求值 end（08637982 endEv +
    // 08637997 C1ERKS_，槽 -0x10/-0x1c），cond 只用缓存值（08637a3a ne 直比）⇒ 源码
    // 必须把 end 迭代器具名缓存；旧码 cond 内每轮重发 endEv+C1ERKS_（多 6 insn/轮）。
    std::map<unsigned int, CUser*>::const_iterator it =
        G_CGameManager()->m_userByAccId.begin();
    std::map<unsigned int, CUser*>::const_iterator itEnd =
        G_CGameManager()->m_userByAccId.end();
    for (; it != itEnd; ++it)
    {
        CUser* user = it->second;
        if (user->get_state() >= 2 && user->getAntibotKey() >= 0)
        {
            // 第二形参 int：与 CEnvironment::getProcessSequence() 返回类型同型
            // （ORIG 08637a11 `mov %eax,0x4(%esp)` 无转换中转；unsigned 形多 1 条
            // `mov %eax,%edx`，P211/P248 档③）。
            typedef void (*CheckFn)(void*, int, int, unsigned int);
            // [b28e-TimerDispatcher] 真 bug 修正：ORIG 086379e8 `add $0x14,%eax;
            // mov (%eax),%esi` = vtable[5] 本体即函数指针（单层解引用）；旧码
            // `*(CheckFn*)((*(void***)checker)[0x14/4])` 多一次解引用（我方多 1 条
            // `mov (%eax),%eax`，调用目标错）。按 PacketDispatcher.cpp 同族单层形 +
            // 去 checker 具名槽（ORIG 调用点重发 `mov 0x941f7e4,%edx`）。
            (*reinterpret_cast<CheckFn*>(
                reinterpret_cast<char*>(
                    *reinterpret_cast<void**>(GlobalData::s_antibotChecker_)) + 0x14))(
                GlobalData::s_antibotChecker_,
                G_CEnvironment()->getProcessSequence(), 0, user->get_acc_id());
        }
    }
    registNextTimer(10);
    return 1;
