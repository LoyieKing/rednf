// ORIG W _ZN27CInGameAdvertisementManagerC1Ev @0x080fdd6a：multimap 默认构造
// → time(0) → new CMTRand(0x9c8, seed) 存 +0x18。
CInGameAdvertisementManager::CInGameAdvertisementManager()
{
    time_t now = time(0);
    // ORIG 0x080fdd8a：time 结果槽 -0x1c 单槽直绑（lea -0x1c 早于 operator new，
    // %esi 跨调用缓存 &now）。const unsigned long& 形参 → time_t 槽同址绑定。
    // 4.4 对 &now 实参表达式自然产 lea -0x1c 并物化（P0：寄存器仅承载表达式值）。
    // 旧判 N96（b28d-solo-CInGameAdvertisementManager，ORIG 0x080fdd8a）：
    // ORIG 4.1 在 `call _Znwj` **之前**发单条 `lea -0x1c(%ebp),%esi`（&now 直接
    // 提前入 callee-saved，无中间槽）；4.4 把 lea 放到 operator new 之后且用 %edx。
    // 旧复测 9 形（/tmp/b28d_solo_probe{,2,3}.cpp：现形 / reinterpret_cast 引用
    // cast / C cast / 裸语句表达式 ({&now;}) / volatile now / 同型 unsigned long
    // 直传 / 声明后赋值 / 具名指针局部 / 具名引用局部）无一复现——后两形虽能让 lea
    // 提前，但额外物化 1~2 个槽 + 2 条 store/load（ORIG 无该槽），形态更劣。
    // ⇒ 旧结论 4.1「无副作用地址计算提前调度 + callee-saved 直绑」不可复现。
    // [L6S6b 2026-09-11 复核] 46/46 ndiff4：唯一差异 = ORIG 0x080fdd8d
    //   `lea -0x1c(%ebp),%esi`（`call _Znwj` @0x080fdd97 **之前**）vs 我方
    //   `lea -0x1c(%ebp),%edx`（new 之后）。上轮 9 形（现形/reinterpret_cast/
    //   C cast/裸语句表达式/volatile/同型直传/声明后赋值/具名指针局部/具名引用
    //   局部）无一能让 4.4 提前且不加槽 ⇒ 旧判 N96（P75 实参装载调度 + N7）。
    // [L6S6b-2 2026-09-11 P307 **命中**，破 N96] 裸 `register` 指针承载 &now：
    //   register 变量无地址 ⇒ 4.4 不落槽，&now 的 lea 成为独立语句被提前到
    //   operator new 之前并直绑 callee-saved %esi（跨 new + ctor 保活），
    //   逐条复现 ORIG 0x080fdd8d..0x080fddac。C1Ev DIFF→AE（151→152）。
    register const unsigned long* seedp =
        reinterpret_cast<const unsigned long*>(&now);
    m_rand = new CMTRand(*seedp);
}