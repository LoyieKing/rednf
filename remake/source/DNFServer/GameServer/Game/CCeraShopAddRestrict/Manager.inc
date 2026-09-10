Manager::Manager()
    // [b28c-4] 试 value-init 形复现 ORIG 4.1 对 m_loader 的前置 zero-init
    //（P90/N77：4.4 按 CWG434 可能仍不 emit）。
    : m_loader()
{
