// ============================================================================
// _CheckConditionPutonAvatar —— ORIG 0x08324966 (T)
// 遍历角色装备槽（INVEN 0，slot 0..9）与头像槽（INVEN 2，slot 0..0x68），
// 任一槽位 Inven_Item 的 m_addInfo(+0x02) 非 0 即返回 1（已有装备/头像），否则 0。
// ============================================================================
// b28d-solo-CCeraShop_Special verdict=DIFF [P223 边界封顶] ORIG 0x08324966 56/54。
// ORIG 两守卫 je 分落 +0x4b(单独 nop 岛)/+0x4c(直接落 addl join)；4.4 岛数恒 =
// 条件数或 0（探针 fA/fB/hA/hB/hD/kA 六形全败：goto 形恒多发 `nop;jmp;nop`，
// 共享/空 else/continue 形恒折同目标）⇒ 保持 `&&` 原形（P48 双静态调用点）。
// [R8a03 2026-09-12 · P292 档①] 返回类型 int→bool：ORIG 调用点 0x0832421x 直返
//   无归一化（见 CCeraShop.h 同处注释）。体内 `return 1/0` 在两类型下同为
//   `mov $0x1/$0x0,%eax`，定义体形态不变（A/B 零劣化实测）。
bool CCeraShop::_CheckConditionPutonAvatar(CUser* user)
{
    // P48：两循环各自入口一次 getCurCharacInvenR 静态调用点（禁缓存）。
    for (int i = 0; i <= 9; ++i)
    {
        const Inven_Item* it = user->getCurCharacInvenR()->GetInvenRef(0, i);
        if (it != 0 && it->m_addInfo != 0)
            return 1;
    }
    for (int i = 0; i <= 0x68; ++i)
    {
        const Inven_Item* it = user->getCurCharacInvenR()->GetInvenRef(2, i);
        if (it != 0 && it->m_addInfo != 0)
            return 1;
    }
    return 0;
}