void CAccountCargo::UpgradeAccountCargo(CUser* user)
{
    // [b28d-solo-CAccountCargo 判词·AE] ORIG 0x0828b0b2：210/210；`upgradeInfo->m_fieldC`
    // 作左操作数（P66 左先物化 %ebx）＋ delete_item 守卫合并 `&&`（4.1 对 bool 左项
    // 的物化岛形）后全等。p167 CLEAR（29/29）。
    if (!user->IsExistAccountCargo()) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x15);
        return;
    }
    CAccountCargo* cargo = user->GetAccountCargo();
    UpgradeInfo* upgradeInfo =
        (UpgradeInfo*)G_CDataManager()->m_accountCargo.GetNextUpgradeInfo(
            cargo->GetCapacity());
    if (upgradeInfo == 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0x13);
        return;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 0x19)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0xd1);
        return;
    }
    unsigned int err = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)5);
    if (err != 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, (unsigned char)err);
        return;
    }
    Inven_Item item;
    int itemIdx = G_CDataManager()->m_accountCargo.m_itemIdx;
    int slot = ((CInventory*)user->getCurCharacInvenR())->GetInvenData(itemIdx,
                                                                       item);
    if (upgradeInfo->m_field8 != 0 &&
        (slot == -1 || upgradeInfo->m_field8 > (int)item.m_addInfo2)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0x16);
        return;
    }
    if (0 < upgradeInfo->m_fieldC &&
        upgradeInfo->m_fieldC > user->getCurCharacMoney()) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0x16);
        return;
    }
    if (upgradeInfo->m_field8 != 0 &&
        !((CInventory*)user->getCurCharacInvenW())
             ->delete_item((INVEN_TYPE)1, slot, upgradeInfo->m_field8,
                           (eItemDelReason)0x24, 1)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0x16);
        return;
    }
    if (0 < upgradeInfo->m_fieldC) {
        if (!((CInventory*)user->getCurCharacInvenW())
                 ->use_money(upgradeInfo->m_fieldC, (eMoneySubReason)0x29, 1)) {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133, 0x16);
            return;
        }
    }
    SendNotifyRecipe(user, slot, upgradeInfo->m_field8 != 0);
    DB_UpgradeAccountCargo::makeRequest(user->GetUID(), user->get_acc_id(),
                                        (unsigned int)upgradeInfo->m_field0);
}