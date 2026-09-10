void CAccountCargo::CreateAccountCargo(CUser* user)
{
    // [b28d-solo-CAccountCargo 判词·AE] ORIG 0x0828ad72：217/217；裸偏移
    // `*(short*)((char*)getCurCharacR()+0x27)` 具名化为 `->m_level`（§4.3 清违，
    // 同时消 add-vs-disp 折叠差 N102）＋ `m_fieldC` 左操作数（P66）＋ delete_item
    // 守卫合并 `&&`。p167 CLEAR（30/30）。
    if (user->IsExistAccountCargo()) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x14);
        return;
    }
    if (G_CDataManager()->m_accountCargo.m_reqLevel >
        (int)user->getCurCharacR()->m_level) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0xe);
        return;
    }
    UpgradeInfo* createInfo =
        (UpgradeInfo*)G_CDataManager()->m_accountCargo.GetCreateInfo();
    if (createInfo == 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x13);
        return;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 0x18)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0xd1);
        return;
    }
    unsigned int err = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)4);
    if (err != 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, (unsigned char)err);
        return;
    }
    Inven_Item item;
    int itemIdx = G_CDataManager()->m_accountCargo.m_itemIdx;
    int slot = ((CInventory*)user->getCurCharacInvenR())->GetInvenData(itemIdx,
                                                                        item);
    if (createInfo->m_field8 != 0 &&
        (slot == -1 || createInfo->m_field8 > (int)item.m_addInfo2)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x16);
        return;
    }
    if (0 < createInfo->m_fieldC &&
        createInfo->m_fieldC > user->getCurCharacMoney()) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x16);
        return;
    }
    if (createInfo->m_field8 != 0 &&
        !((CInventory*)user->getCurCharacInvenW())
             ->delete_item((INVEN_TYPE)1, slot, createInfo->m_field8,
                           (eItemDelReason)0x24, 1)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x16);
        return;
    }
    if (0 < createInfo->m_fieldC) {
        if (!((CInventory*)user->getCurCharacInvenW())
                 ->use_money(createInfo->m_fieldC, (eMoneySubReason)0x29, 1)) {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, 0x16);
            return;
        }
    }
    SendNotifyRecipe(user, slot, createInfo->m_field8 != 0);
    DB_CreateAccountCargo::makeRequest(user->GetUID(), user->get_acc_id(),
                                       (unsigned int)createInfo->m_field0);
