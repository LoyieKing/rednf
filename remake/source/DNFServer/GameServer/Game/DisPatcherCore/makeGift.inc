void DisPatcher_CreateCharac::makeGift(CUser* /*user*/, SIG_CREATE_CHARAC* sig)

{
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
        tmpWeapon.m_addInfo =
            CREATECHARACTEREQUIP[static_cast<int>(static_cast<signed char>(sig->m_slot)) * 2];
        weapon->make_item(tmpWeapon);
        sig->m_weaponItem = tmpWeapon;                        // 0x3d 字节 → SIG+0x4e7b

        Inven_Item tmpClothes;
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
