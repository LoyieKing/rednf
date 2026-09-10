bool CCharacList::init()
{
    // [b28d-solo-CtorProducers_1 判词·P50+P96+P0] ORIG 0x08349514：旧「N89 迭代器
    // 实例差封顶」判词作废。ORIG 逐条实证：①`movl $0x0,-0xc(%ebp)` 先清 char* 局部
    // （P244 具名局部，声明在最前）；②iterator **默认构造** `_Rb_tree_iterator::C1Ev`
    // 占 -0x18，再 `it = begin()` 经 sret -0xc3c 拷入（P50 复活：先声明后赋值）；
    // ③STCharacterScript 在 -0xc34（声明序在 iterator 之后）；④`operator->`/`ne`
    // 均为 **_Rb_tree_iterator（非 const）** 的 const 成员 `_ZNKSt17_Rb_tree_iterator…`
    // （P96/P270：勿用 const_iterator，否则多转换构造）；⑤返回值 `mov $0/%ebx` 直返，
    // 无 bool 局部槽（P0 槽-局部双向对应律）。
    // 增量段必须写 `it++`（P28 增量侧推论：ORIG 增量段调 `_Rb_tree_iterator::ppEi`
    // 后置版并带独立 sret 临时槽 -0x10 ⇒ 迭代器/end/临时三槽阶梯；`++it` 走 ppEv
    // 免临时槽，后续槽整体上移一格）。
    const char* name = 0;
    std::map<int, std::string>::iterator it;
    STCharacterScript script;
    for (it = g_stCharacterScriptFileList.m_fileMap.begin();
         it != g_stCharacterScriptFileList.m_fileMap.end(); it++)
    {
        name = it->second.c_str();
        if (name == 0)
            break;
        if (!ImportCharacterScript(&script, name))
        {
            printf("Charac Script Error: %s\n", name);
            return false;
        }
        m_characs[script.m_job].set_data(&script);   // 真实成员调用 _ZN10CCharacter8set_dataEP17STCharacterScript
    }
    return true;
