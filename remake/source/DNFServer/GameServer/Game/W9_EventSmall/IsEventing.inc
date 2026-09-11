bool CTrcOnOffEvent::IsEventing(CUser* user) const  // ORIG 0x08285ba4：透传基类
{
    // ORIG 0x08285ba4：第二参数恒传 0（NULL），不转发本函数形参。返回仍等于
    // 基类结果，4.1 走「取反+反极性岛」。b28d-lane6 P0 清绑（N96）：删历史
    // `register bool t __asm__("al")` 绑定，改 P168a 裸形 `if(!boolcall())`。
    if (!CEventBase::IsEventing(static_cast<CUser*>(0)))
        return false;
    return true;
}