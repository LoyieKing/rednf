// ORIG execute 0x084a9720：取分解师职业槽挂到 GM 角色，随后在其脚下
// 以固定名 "test"/容量 500 创建分解商店（坐标取 GM 角色当前位置低 16 位）。
// 【b28 判词 N-P75】job 槽(-0xc)值供 SetCurCharacExpertJob 实参：ORIG 4.1 在
// GetUser call 前物化入 %ebx（`mov -0xc(%ebp),%ebx` 前置），4.4 恒 call 后
// 重读槽入 %edx（参数装载时刻跨版本调度，P75；P226 语句表达式形在纯槽装载
// 场景不升格，探针 /tmp/bm3/p13 fSE），余 25 条逐条相等。
// [round5-GmCmds_C] 上述 N-P75 封顶**已推翻**（根因是调用点 static_cast 中间
// 表达式，见下函数内注释；改成派生类指针局部后 45=45 逐条等同 ⇒ AE）。
void CCreateDisjointStoreCmd::execute()
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
{
    // [round5-GmCmds_C 落地·推翻旧 N-P75 封顶] ORIG 0x084a9740 在 GetUser call
    // **之前** `mov -0xc(%ebp),%ebx`（值跨 call 保活入 callee-saved），4.4 原用
    // call 后重读槽入 %edx。根因是调用点的 **static_cast 中间表达式**（原码
    // `static_cast<CDisjointer*>(job)->OnCreate...` 使 4.4 分两次求值 lvalue）；
    // 把 job 的声明类型直接写成 expert_job::CDisjointer*（cast 收进初始化式）后，
    // 4.4 复现 ORIG 的 `mov -0xc(%ebp),%ebx` 前置保活 + 末尾 `mov -0xc(%ebp),%eax`
    // 直传形，45 条与 ORIG 逐条等同（verdict DIFF→**AE**；TU 四数 ae 54→55、
    // diff 3→2，零劣化）。历史「纯 register 提示 45→46」方向已作废。
    expert_job::CDisjointer* job = static_cast<expert_job::CDisjointer*>(
        GlobalData::s_ExpertJobMgr->AcquireExpertJob(
            static_cast<ENUM_EXPERT_JOB_TYPE>(3)));
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
    job->OnCreateDisjointStore(
        GetUser(), "test", 500,
        static_cast<int>(static_cast<unsigned short>(GetUser()->get_posX())),
        static_cast<int>(static_cast<unsigned short>(GetUser()->get_posY())));
}