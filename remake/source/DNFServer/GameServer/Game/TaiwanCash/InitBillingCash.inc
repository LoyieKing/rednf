// ---- InitBillingCash —— ORIG T 0x0814657c：
//   定义位置：置于 TaiwanCash::TaiwanCash 定义之前（P210 三态③：4.4 只见声明
//   ⇒ new-expression 保守布 EH landing pad；ORIG 0x81465b7..0x81465cc 有
//   `jmp` + operator delete landing 段实证）。
//   type = *(uchar*)(G_CEnvironment()+0x378)（= Server_Envir.m_serverGroup 低字节，
//   CEnvironment.h 布局 +0x198+0x1e0 实证）——ORIG 中保持 %esi 寄存器、无栈槽 ⇒
//   P0 判为右值内联，禁具名局部；G_CEnvironment() 亦临时直用（各次仅寄存器）。
//   new TaiwanCash(type, 1) 存入出参；以 m_dbConnInfo[DB_HANDLE_E] 的连接信息
//   init(host, port, db, user, pass)。
//   ORIG 反汇编尾部：出参判空走 0xd0 → 恒返回 1；init 结果按 0xd5/0xe0/0xe5
//   三态全部走 eax 寄存器（无栈槽 ⇒ P0：ghidra 的 bVar2 非具名局部），
//   即 `!(cash==0 || init==1)` 单表达式返回，勿拆 if 注入 bool 局部。----
// [b28d-47] verdict=DIFF(routed) [N100 + 真语义修正] ORIG 0x0814657c orig/ours=76/61
//   修：真语义 bug——ORIG 返回 true ⇔ (cash==0 || init 返回 1)（双层取反
//   bVar2=!(A||B); return !bVar2）；旧单层 !(A||B) 在 cash==0/init 成功时返回 false，
//   与 ORIG 相反。已改嵌套 !(!(...))（4.4 折叠单层但语义正确、无 bVar2 槽、槽位
//   -0x20/-0x1c 与 ORIG 对齐）。残余：new-expression landing pad 抑制（ORIG 有
//   `jmp`+_ZdlPv+_Unwind_Resume 10 条，4.4 判 ctor nothrow 不布 cleanup ⇒ N100）。
bool InitBillingCash(Taiwan::TaiwanCash*& cash)
{
    // ORIG 0x0814657c：G_CEnvironment() 调两次（type 一次、map 访问一次），
    // 两次结果均为临时（寄存器直用，无栈槽）。
    cash = new Taiwan::TaiwanCash(
        static_cast<unsigned char>(
            G_CEnvironment()->m_serverEnvir.m_serverGroup),
        1);

    // ORIG 第二次 G_CEnvironment() 后直访 m_dbConnInfo[DB_HANDLE_E]（+0x68；
    // 禁 get_db_conn_info() accessor——ORIG 无该 call，P167 判据）。
    STDBConnInfo& info =
        G_CEnvironment()->m_dbConnInfo[DB_HANDLE_E];

    // [b28d-47] 真语义 bug 修正：ORIG 0x0814663e..+0xe5 双层取反 ⇒ 返回 true
    // 当且仅当 cash==0 或 init 返回 1。
    // [P2S6 2026-09-11 N82] ORIG 键形直证：+0x82 `test %eax,%eax; je +0xd0`
    //   （cash==0 短路到 b1=0）、+0xc2 `xor $0x1,%eax`（init 为 **bool** ⇒ 源码是
    //   `!init(...)`，非 `!= 1`；TaiwanCash.h:105 `bool init(...)`）、+0xd5 第二层
    //   `test %al,%al; je +0xe0; mov $0; jmp; +0xe0 mov $1`（取反的分支式物化）。
    //   ⇒ ORIG 表达式结构 = `!( cash != 0 && !init(...) )`（本行即照此写）。
    // [P2S6 2026-09-11 负结论 · 本件不可达 AE/IDENTICAL] 试过 5 种形态均无法复现：
    //   ① `!(!(cash==0 || init==1))`、② 同式加具名 bool、③ `cond ? false : true`、
    //   ④ `(cash!=0 && init!=1) ? false : true`、⑤ 本式 `!(cash!=0 && !init())` ——
    //   4.4.7 前端在 tree 层把 `!(A&&B)` de Morgan 折叠成短路 `!A || !B`（无 +0xc2
    //   xor、无 +0xd5 第二层 5 条）；具名 bool 变体虽补出 5 条却引入 ORIG **没有**的
    //   -0x19 槽（+0x? 实测 `mov %al,-0x19(%ebp); movzbl -0x19(%ebp),%eax; xor $0x1`）。
    //   另一块 +0x3b..+0x50（`jmp +0x55; mov %edx,%esi; mov %eax,%edi; mov %ebx,(%esp);
    //   call operator delete; mov %edi,%eax; mov %esi,%edx; mov %eax,(%esp);
    //   call _Unwind_Resume`）是 new-expression 的 EH cleanup：4.4.7 在 TU 级看到
    //   本 TU 内 ctor 定义（TaiwanCash.cpp:130 `: m_mysql(), m_mutex()`）判定 nothrow
    //   ⇒ 不布 landing pad（ORIG 编译器更保守）。两者皆**编译选项/前端版本差异**，
    //   源码层不可造；保持语义正确的最小表达式。gap 76/61 = +15 不变。
    return !(cash != 0 &&
             !cash->init(info.m_host, static_cast<unsigned int>(info.m_port),
                         info.m_db, info.m_user, info.m_pass));
}