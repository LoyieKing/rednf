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

    // [b28d-47] 真语义 bug 修正：ORIG 0x0814663e..+0xe5 双层取反
    // bVar2 = !(cash==0 || init(...)==1); return !bVar2 ⇒ 返回 true 当且仅当
    // cash==0 或 init 返回 1（+0xc7 je +0xd0 落 bVar2=0 ⇒ 尾 +0xe0 mov $1）。
    // 旧实现单层 !(...) 在 cash==0/init 成功时返回 false，与 ORIG 相反。
    // [b28d-47] 嵌套双层 `!(!(...))`：具名 bool 会落 -0x19 槽（4.4 实测），
    // ORIG 第一层结果直接留在 eax 供第二层 test（无槽往返）。
    return !(!(cash == 0 ||
               cash->init(info.m_host, static_cast<unsigned int>(info.m_port),
                          info.m_db, info.m_user, info.m_pass) == 1));
