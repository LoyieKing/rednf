STMapScript::STMapScript()                  // ORIG C1 0x0836cc94
{
    // ORIG C1：仅隐式成员构造（非平凡类 C1 按声明序），无标量写；
    // 标量清零全部由 body 的 Clear() 完成（P0 槽律同源）。
    Clear();
}