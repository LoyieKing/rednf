// ---- 杂项对象 ----
StackableAction::Action::Action()
{
    // ORIG 0x0827f6d8：两个 map 构造（成员自动）+ init_function_type()。
    init_function_type();
}