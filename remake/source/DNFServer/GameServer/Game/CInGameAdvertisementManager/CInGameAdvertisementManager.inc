CInGameAdvertisementManager::CInGameAdvertisementManager()
{
    time_t now = time(0);
    // ORIG 0x080fdd8a：time 结果槽 -0x1c 单槽直绑（lea -0x1c 早于 operator new，
    // %esi 跨调用缓存 &now）。const unsigned long& 形参 → time_t 槽同址绑定。
    // 4.4 对 &now 实参表达式自然产 lea -0x1c 并物化（P0：寄存器仅承载表达式值）。
    // b28d-solo-CInGameAdvertisementManager 判词（N 封顶，ORIG 0x080fdd8a）：
    // ORIG 4.1 在 `call _Znwj` **之前**发单条 `lea -0x1c(%ebp),%esi`（&now 直接
    // 提前入 callee-saved，无中间槽）；4.4.7 恒把 lea 放到 operator new 之后且用
    // %edx。本轮独立复测 9 形（/tmp/b28d_solo_probe{,2,3}.cpp：现形 /
    // reinterpret_cast 引用 cast / C cast / 裸语句表达式 ({&now;}) / volatile now /
    // 同型 unsigned long 直传 / 声明后赋值 / 具名指针局部 / 具名引用局部）：前 7 形
    // lea 恒在 new 之后（%edx）；仅具名中间局部形（指针/引用/unsigned long 副本）
    // 能让 lea 提前，但额外物化 1~2 个槽 + 2 条 store/load（ORIG 无该槽），形态更劣。
    // ⇒ 4.1「无副作用地址计算提前调度 + callee-saved 直绑」不可复现（P75 跨版本
    // 实参装载调度 + N7 寄存器分配族）。残差 3 行（lea 位置 + %esi/%edx）。
    m_rand = new CMTRand(*reinterpret_cast<unsigned long*>(&now));
