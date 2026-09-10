void CCreateDisjointStoreCmd::execute()
{
    // [round5-GmCmds_C] P231/P248 第四条件满足仍不可翻：ORIG 0x084a9740 在 GetUser
    // call **之前** `mov -0xc(%ebp),%ebx`（值跨 call 保活入 callee-saved），4.4 恒
    // call 后重读槽入 %edx。纯 register 提示实测 45→46 条（且 push %ebx 前移帧）
    // ⇒ 与历史 bm3 探针同结论：纯槽装载实参不因存储类提示升格（N-P75 封顶）。
    expert_job::CExpertJob* job =
        GlobalData::s_ExpertJobMgr->AcquireExpertJob(
            static_cast<ENUM_EXPERT_JOB_TYPE>(3));
    // ORIG 0x084a9720：GetUser 四独立重调（P48 无 CSE）；posY/posX 值经
    // %esi/%ebx 跨 call 直传（P0 无槽），movzwl=(unsigned short) 拓宽直传；
    // 实参 R2L：y(+0x40) → x(+0x50) → user(+0x66)。
    // ORIG 0x084a9740：job 在 GetUser call 前物化入 %ebx 跨调用保活（4.1 实参
    // 提前物化）；4.4 恒 call 后重读槽（N-P75 跨版本调度，语句表达式 P226 实测
    // 不升格——/tmp/bm3/p13 fSE 同结论），余 2 条为寄存器名差（N7）。
    // [round5-GmCmds_C 判词·N-P75/N7 复核] ORIG 0x084a9740 在 GetUser call **之前**
    // `mov -0xc(%ebp),%ebx` 把 job 值保活入 callee-saved；本轮补测两形均无效：
    // ①纯 `register` 存储类提示（P231/P248 四条件齐备）⇒ 45→46 条（且 push %ebx
    // 前移帧）；②裸语句表达式 `({ job; })`（P226）⇒ 与裸形逐条全同。⇒ 4.4 对纯槽
    // 装载实参恒 call 后重读（N-P75 跨版本实参调度），余 2 条为寄存器名差（N7）封顶。
    GetUser()->SetCurCharacExpertJob(job);
    static_cast<expert_job::CDisjointer*>(job)->OnCreateDisjointStore(
        GetUser(), "test", 500,
        static_cast<int>(static_cast<unsigned short>(GetUser()->get_posX())),
        static_cast<int>(static_cast<unsigned short>(GetUser()->get_posY())));
