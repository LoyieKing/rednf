void ScriptManager::RegistScript(ScriptInterface* s, int priority)
{
    // ORIG 0x81a136c：find 已存在（重复注册）→
    // printf("Arad_ScriptManager::RegistScript() Duplicate Class : Line(%d)\n", priority) + exit(0)；
    // 不存在 → insert，返回 true。
    // b28d-60：NEAR 残差=insert 后 `leave` 前一条对齐 nop（ORIG 无）——if/else
    // 双块形替代 guard-return 试探（消 4.4 的块对齐 nop）。
    if (m_scripts.find(s) == m_scripts.end())
    {
        m_scripts.insert(s);
    }
    else
    {
        printf("Arad_ScriptManager::RegistScript() Duplicate Class : Line(%d)", priority);
        exit(0);
    }
