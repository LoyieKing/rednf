bool CNPCDynamicInfoManager::add(const CNPCScript* script)
{
    // [b28d-solo-CtorProducers_1 判词·P0+P236+P292] ORIG 0x08580668：旧「N/P75+N89
    // 封顶」判词作废其可翻部分。ORIG 逐条：①第一次 `getIndex()` **内联实参**
    // （`call getIndex; mov %eax,0x4(%esp)`，无具名 idx 槽往返，P0/P236）；②守卫
    // `test %eax,%eax; setne %al; test %al,%al; je <new 块>` ⇒ 正极性 `!= 0` 早返
    // （我们旧 `== 0` 包裹产 sete + 反向布局，P292 调用点判别）；③第二次
    // `getIndex()` 结果落 -0xc 为编译器物化的 make_pair 实参临时（非具名局部）。
    // 残余：4.4 对 new-expression 布 landing pad（push esi/edi + operator delete +
    // _Unwind_Resume），ORIG 无（N100）；消解需 callee `CNPCDynamicInfo::C1` 加
    // `throw()`（声明 NPCDynamicInfo.h + 定义 CharacterAIA2.cpp）——跨 TU，已上报 Main。
    // ORIG 0x08580668：operator new(0x10) 后直接 C1 构造，无空指针检查
    // （GCC 4.4 -O0 平铺 `new X()` 恰为此形）。真实 ctor
    // _ZN15CNPCDynamicInfoC1EPK10CNPCScript（NPCDynamicInfo.h 权威声明，
    // 定义 CharacterAIA2.cpp W 0x0858191c）。§4.5 禁 extern "C" 桥。
    if (getNPCInfo(script->getIndex()) != 0)
        return false;
    CNPCDynamicInfo* info = new CNPCDynamicInfo(script);
    m_map.insert(std::make_pair(script->getIndex(), info));
    return true;
}