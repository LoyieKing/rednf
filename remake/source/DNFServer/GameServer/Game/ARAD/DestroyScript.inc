bool ScriptManager::DestroyScript()
{
    // ORIG 0x81a12f8：遍历 set 逐个 Unload()，返回 1。
    // ORIG end() 一次求值（P13 缓存）；it 槽浅 -0xc / e 槽深 -0x10 ⇒
    // it 先声明（P212 头部直构试探）。
    std::set<ScriptInterface*>::const_iterator it = m_scripts.begin();
    std::set<ScriptInterface*>::const_iterator e = m_scripts.end();
    for (; it != e; ++it)
        (*it)->Destroy();
    return true;
