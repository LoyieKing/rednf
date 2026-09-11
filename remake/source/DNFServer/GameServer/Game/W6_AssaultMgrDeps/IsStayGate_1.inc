// ===========================================================================
// IsStayGate —— ORIG T 0x082edc05 (0x81)，全局自由函数（被本 TU 使用）。
// ===========================================================================
// [b28d-solo-W6_AssaultMgrDeps] AE（40=40）：P225 `while (x==0) { return true; break; }` 复现 4.1
// 槽比零 sete/test/je 物化；P212 全头部声明对齐双预清；area 内联保 %ebx 跨 call。
// 旧 P197④「不可翻」判词作废。ORIG 0x082edc05。
bool IsStayGate(CUser* user)                            // _Z10IsStayGateP5CUser
{
    // ORIG 0x082edc05：入口 `movl $0x0,-0x10/-0xc(%ebp)` 双预清（P198① 两句式）
    // ⇒ 原码为「声明=0 + 独立赋值」两段。
    // [b28d-solo] P212 全头部声明：ORIG 两条预清 `movl $0,-0x10/-0xc` 连续
    // 落函数头（0x082edc0c/0x082edc13）⇒ 两个指针局部都声明在函数头。
    GameWorld* world = 0;
    Village* village = 0;
    world = G_GameWorld();
    // [b28d-solo] P225：ORIG 槽比零物化 `cmpl $0,-0x10; sete %al; test %al,%al;
    // je`（0x082edc22..0x082edc2b）⇒ while(cond){return X; break;} 形可翻。
    while (world == 0)
    {
        return true;
        break;
    }
    village = world->getUserVillage(user);
    while (village == 0)
    {
        return true;
        break;
    }
    // [b28d-solo] P0：ORIG get_area 结果留 %ebx 跨 get_gate_area call（无槽往返）
    // ⇒ 原码无具名 area 局部，使用点内联表达式。
    return user->get_area(false) == village->get_gate_area();
}