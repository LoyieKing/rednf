CCreatureScript* CCreatureScriptMgr::FindCreatureScript(int creatureId)
{
    // round1 R19：ORIG 0x0833d4a2 段为 **非 const** `find` sret 直构进 it 槽
    // （-0x20）+ 内联 `end()`（-0x1c 临时）+ `_Rb_tree_iterator::eq` 单条比较；
    // const_iterator 会多出 iterator→const_iterator 转换构造（+2 lea/call，
    // P96/P270/P323）⇒ 改非 const iterator，`end()` 不落具名局部（L29 ①）。
    std::map<int, CCreatureScript*>::iterator it = m_scripts.find(creatureId);
    if (it == m_scripts.end())
    {
        // P221：ORIG 槽序 it(-0x20) < end(-0x1c) < trace(-0x18)（迭代器深、
        // 具名 trace 浅）；4.4 具名对象恒反向（trace 深 -0x28）⇒ 改「构造即调用」
        // 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0xfe8, 5)("[%s][%d]",
                                                __PRETTY_FUNCTION__,
                                                0xfe8);
        return 0;
    }
    return it->second;
