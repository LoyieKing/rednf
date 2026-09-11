// ORIG T 0x081c0a1a（makeGift.md）：按创建槽位（=职业索引）发放初始武器/衣服与
// 职业赠礼道具。find_item 失败时仅置标志 0；命中则 Inven_Item 预填 +0x02 索引后
// [b28d-29] P205/P30：ORIG 单条 scaled `mov <A>(,%eax,8)`，4.4 拆 add+scaled；字段直读 vs 缓存差，语义集合全等  (ORIG 0x081c0a1a insn 227->223)
// 经 CItem::make_item（vtable+0x08）填充并整体拷贝入 SIG 槽位。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 158（ORIG 225 / ours 221），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
void DisPatcher_CreateCharac::makeGift(CUser* /*user*/, SIG_CREATE_CHARAC* sig)

{
    // [DpcMisc-R2 2026-09-11 负结论] 除下方 tmpClothes 声明位置修复外，本件唯一残差 =
    // `CREATECHARACTEREQUIP[idx*2]` 的寻址折叠：ORIG 0x081c0a2x/0x081c0b7x
    // `mov 0x9370180(,%eax,8),%ebx`（单条 scale8），4.4 恒 `add %eax,%eax;
    // mov 0xc80(,%eax,4),%ebx`（+1 条）。全局表 `static const int CREATECHARACTEREQUIP[22]`
    // 定义在聚合器 DisPatcherCore.cpp:462（他写域，不可改类型），且 N125 已定案
    // 「4.4 对 i*2 下标缩放恒 add+scale4，不复现 4.1 的 scale8」⇒ 封 N，不试。
    // 另：`call *%edx`↔`call *%eax`（虚调用目标寄存器）为 N7 族。
    // [b28d] P0/P183：ORIG 无 `job` 具名局部——0x081c0a24 / 0x081c0a49 / 0x081c0aab /
    // 0x081c0b75 / 0x081c0dd8 五处各自重发
    // `mov 0x10(%ebp),%eax; movzbl 0x22(%eax),%eax; movsbl %al,%eax`。
    CItem* weapon = G_CDataManager()->find_item(
        CREATECHARACTEREQUIP[static_cast<int>(static_cast<signed char>(sig->m_slot)) * 2]);
    CItem* clothes = G_CDataManager()->find_item(
        CREATECHARACTEREQUIP[static_cast<int>(static_cast<signed char>(sig->m_slot)) * 2 + 1]);
    if (weapon == 0 || clothes == 0)
    {
        sig->m_equipFindOk = 0;                       // ORIG SIG+0x5340；落入赠礼段
    }
    else
    {
        sig->m_equipFindOk = 1;
        Inven_Item tmpWeapon;
        // [DpcMisc-P7 2026-09-11] 声明位置（构造发射序）：ORIG 0x081c0a8x 段两个
        // `Inven_Item::C1Ev` **相邻发射**（`lea -0x55; call C1Ev` 紧跟 `lea -0x92;
        // call C1Ev`）后才用 tmpWeapon ⇒ 原码两个声明相邻（P28 声明序）。旧形把
        // tmpClothes 声明在使用点前，4.4 把第二个 C1 延后到 tmpWeapon 用完之后
        // （difflib 记 -1/+3 错位）。提前声明即对齐，指令数不变。
        Inven_Item tmpClothes;
        tmpWeapon.m_addInfo =
            CREATECHARACTEREQUIP[static_cast<int>(static_cast<signed char>(sig->m_slot)) * 2];
        weapon->make_item(tmpWeapon);
        sig->m_weaponItem = tmpWeapon;                        // 0x3d 字节 → SIG+0x4e7b

        tmpClothes.m_addInfo = CREATECHARACTEREQUIP[
            static_cast<int>(static_cast<signed char>(sig->m_slot)) * 2 + 1];
        clothes->make_item(tmpClothes);
        sig->m_clothesItem = tmpClothes;                      // 0x3d 字节 → SIG+0x4ef5
    }

    // ORIG +0x233 起的赠礼段：无论装备是否命中都会执行。
    int giftId = 0;   // ORIG 0x081c0dcd `movl $0x0,-0x10(%ebp)` 预清（P198①）
    switch (static_cast<int>(static_cast<signed char>(sig->m_slot)))
    {
    case 0: giftId = 0x6bd0; break;
    case 1: case 7: giftId = 0x74cc; break;
    case 2: case 5: giftId = 0x7a44; break;
    case 3: giftId = 0x8340; break;
    case 4: giftId = 0x88b9; break;
    case 6: giftId = 0x908a; break;
    case 8: giftId = 0x8340; break;
    case 9: giftId = 0x6bd0; break;
    case 10: giftId = 0x6520aea; break;
    default:
        return;                                       // ORIG ja +0x3d5：越界不处理
    }
    CItem* gift = G_CDataManager()->find_item(giftId);
    if (gift == 0)
    {
        sig->m_giftFindOk = 0;                        // ORIG SIG+0x533f
        return;
    }
    sig->m_giftFindOk = 1;
    {
        Inven_Item tmp;
        tmp.m_addInfo = giftId;
        gift->make_item(tmp);
        sig->m_giftItem = tmp;                        // 0x3d 字节 → SIG+0x4e01
    }
}
