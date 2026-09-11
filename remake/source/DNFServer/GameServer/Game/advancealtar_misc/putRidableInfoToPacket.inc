// ============================================================================
// putRidableInfoToPacket（ORIG T 0x08141781，报告全文）：前 0x21 字节直拷 +
// 七个 map 各 put_int(size) 后逐元素 put_binary(&value, 元素尺寸)。
// 去发明模板：ORIG nm 无 putMapEntries 实例（§4.1），七段逐循环内联，
// 循环体形态按 ORIG 汇编（begin 前置 0x0851417d2 区、end 每轮尾构、
// postfix it++ 带 dest 临时 0x08135f38、operator-> 后 +4/+1 直传 put_binary）。
// 残余 DIFF 归因（终态 insns 350 vs 349，槽图 -0x10/-0xc/-0x14..-0x64 与
// begin/end/++temp 三件套逐段对齐）：唯一真实指令差 = 首条 put_binary 的
// data 装载 ORIG 两段 `mov 0xc(%ebp),%eax; mov %eax,%edx`（0x08141787/8a），
// 4.4.7 恒折单段（N18a 负例，probe 不可翻）；其余 diff 行为该 1 行错位引发
// 的对齐伪差与 ORIG nearest-symbol call 显示行（put_binary 等公共 W 符号两侧
// 同名，relink 归一后消）。
// [b28d-solo-advancealtar_misc 判词] N 封顶（N18a 单段 regmove）：
//   fresh .o 350:349（gap 1），p167 调用点差集 CLEAR（57=57，0 missing/extra）；
//   唯一残差 = 首条 put_binary 的 `&data` 实参装载：ORIG 两段 `mov 0xc(%ebp),%eax;
//   mov %eax,%edx`，4.4.6 恒折单段 `mov 0xc(%ebp),%edx`。P248 三档旋钮已试：
//   `const_cast<char*>(reinterpret_cast<const char*>(&data))` 与
//   `reinterpret_cast<char*>(const_cast<AdvanceAltarData*>(&data))` 两 cast 形
//   产物相同（均单段）；形参类型受 mangled 约束（RKNS_16AdvanceAltarDataE）不可异型化；
//   ORIG 的 `mov mem,%eax; mov %eax,%reg` 属 4.1 冗余 regmove（N18a）不可造。
// [S4AltarStageMisc 2026-09-11 复核] 本 TU §6.5 串核查：`objcopy -j .rodata` 得 15 个
//   ≥4B 串（getFieldData 两处 logFormat 的文件串/__FUNCTION__/格式串、checkAndSet
//   ActionDefine 的 printf 串 "advancealtar : not found ActionDefine Id = %d\n"、
//   importPhase 的 "[...]" 段串、libstdc++ "vector::_M_insert_aux"），逐个在 ORIG
//   全二进制字节匹配：**0 missing**（含 ORIG 0x8e05c24/0x8e05c4c/0x8e0617c/0x8e05c84/
//   0x8e052e4 逐字核对）；变参顺序 ORIG `logFormat(0,file,func,line,fmt,level)` 与源码
//   逐位相同。§6.5.1：enc_kr_scan.py 全库 0 命中（无非 ASCII 字面量编码错配）。
//   GI 假缺口（按名跨 TU 错配，ORIG 493 实例取首个 0x0807ee76 属 PacketTypes）：本 TU
//   自身实例 = ORIG 0x088a34a2（键 `_GLOBAL__I_...AdvanceAltarShopParameterC2Ev` 0x088a34e2
//   call 之）为**纯 ioinit 形**（`ios_base::Init::C1` + `__cxa_atexit`）⇒ 无真缺口，零改动。
// ============================================================================
void putRidableInfoToPacket(PacketGuard& packet, const AdvanceAltarData& data)
{
    // b28d-35: DIFF→N82（350/349 差 1 条）。ORIG @0x08141781 首条载 0xc(%ebp)，
    //   我方装载序差 1 条（b28d 已 288→1，P183 去缓存后残余为实参装载序）。
    // ORIG 每处直接 `mov 0x8(%ebp)` 传参（无 buf 引用槽）⇒ 不用具名别名。
    // 段 2..6 容器指针 = ORIG 二段初始化槽（`movl $0,-0x10/-0xc` + 赋值，
    // 0x0814184e/0x0814185b 等）；段 1/7 直取（0x081417a3/0x08141bb7）。
    // [Q8 2026-09-11 旧N判词重扫·维持] N82/N18a 单段 regmove：ORIG 0x08141787/0x0814178a
    //   `mov 0xc(%ebp),%eax; mov %eax,%edx`（4.1 先求 data 地址入 %eax，再为 this 覆盖
    //   %eax 前把 data 搬到 %edx）vs 4.4 恒 `mov 0xc(%ebp),%edx` 直落目标。本波追加
    //   试形 C `reinterpret_cast<char*>(&const_cast<AdvanceAltarData&>(data))`（在 P248
    //   两 cast 形之外的第 3 形）**零变化**（350/350、delta 仍 1 行、四数不变）⇒ 维持。
    // [W13C 2026-09-11 试形 D·负结论] 再试 **C 风格 cast** `(char*)&data`（第 4 形）：
    //   影子树 routed 实测 `verdict=DIFF orig=350 our=350 gap=0 ndiff=1` —— 与基线逐项
    //   相同（仍缺 ORIG 那条 `mov %eax,%edx`）⇒ 维持；本件 4 形已穷尽（P248 两 cast +
    //   Q8 形 C + 本形 D），残余确为 4.1 冗余 regmove（N18a），源码侧无旋钮。
    packet.put_binary(reinterpret_cast<char*>(const_cast<AdvanceAltarData*>(&data)), 0x21);   // [objdump 0x8141798]

    // +0x24 stages（put_binary 8 字节，0x081417fe）
    packet.put_int((int)data.m_stages.size());                           // [0x081417ac/0x081417bd]
    for (std::map<int, _Stage>::const_iterator it = data.m_stages.begin();
         it != data.m_stages.end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_Stage*>(&it->second)), sizeof(_Stage));

    // +0x3c / +0x54 / +0x6c 三张 buyItem 表（各 0xc，0x081418b3/0x0814195e/
    // 0x08141a09），局部引用形：ORIG 每轮 `lea 0xXX(%eax)` 重取容器地址。
    std::map<int, _BuyItem> const* pBuy = 0;
    pBuy = &data.m_buyItems1;
    packet.put_int((int)pBuy->size());                                   // [0x08141864]
    for (std::map<int, _BuyItem>::const_iterator it = pBuy->begin();
         it != pBuy->end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_BuyItem*>(&it->second)), sizeof(_BuyItem));

    pBuy = &data.m_buyItems2;
    packet.put_int((int)pBuy->size());                                   // [0x0814190f]
    for (std::map<int, _BuyItem>::const_iterator it = pBuy->begin();
         it != pBuy->end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_BuyItem*>(&it->second)), sizeof(_BuyItem));

    pBuy = &data.m_buyItems3;
    packet.put_int((int)pBuy->size());                                   // [0x081419ba]
    for (std::map<int, _BuyItem>::const_iterator it = pBuy->begin();
         it != pBuy->end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_BuyItem*>(&it->second)), sizeof(_BuyItem));

    // +0x84 / +0x9c slot 表（key uchar，value 偏移 +1，len 10，
    // 0x08141abd/0x08141b6a）
    std::map<unsigned char, _Slot> const* pSlot = 0;
    pSlot = &data.m_slots1;
    packet.put_int((int)pSlot->size());                                  // [0x08141875]
    for (std::map<unsigned char, _Slot>::const_iterator it = pSlot->begin();
         it != pSlot->end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_Slot*>(&it->second)), sizeof(_Slot));

    pSlot = &data.m_slots2;
    packet.put_int((int)pSlot->size());                                  // [0x08141920]
    for (std::map<unsigned char, _Slot>::const_iterator it = pSlot->begin();
         it != pSlot->end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_Slot*>(&it->second)), sizeof(_Slot));

    // +0xb4 rewards（5 字节，0x08141c17）
    packet.put_int((int)data.m_rewards.size());                          // [0x08141bc2]
    for (std::map<int, _Reward>::const_iterator it = data.m_rewards.begin();
         it != data.m_rewards.end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_Reward*>(&it->second)), sizeof(_Reward));
}