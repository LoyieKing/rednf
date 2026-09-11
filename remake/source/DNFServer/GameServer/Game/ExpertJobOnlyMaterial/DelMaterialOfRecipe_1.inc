// ORIG _ZN10expert_job19DelMaterialOfRecipeEP5CUserRSt6vectorISt4pairIiiES
// aIS4_EEiP11PacketGuardRbi @0x0849b957：与成员版差别——清单先经
// CalculateExpertJobMaterials 归一化；尾参 bodyCount 取 1 时附带删除并包
// 上报配方本体条目，其它值只上报材料。
// b28d-solo-ExpertJobOnlyMaterial: AE — P248 档②（同成员版 bool 局部）。
bool DelMaterialOfRecipe(CUser* user,
                         IntPairVector& materials,   // [Q3PrettSweepA] 同上
                         int slot, PacketGuard* packet, bool& changed,
                         int bodyCount)
{
    std::vector<std::pair<int, int> > normalized;
    normalized.clear();
    normalized = materials;
    user->CalculateExpertJobMaterials(normalized);

    std::vector<int> deletedSlots;
    std::vector<int> unusedSlots;                 // ORIG 0x849b9xx：未使用局部（保形）
    std::vector<int>::iterator it;                // ORIG 0x849b9xx：迭代器前置构造
    int delCount = user->getCurCharacInvenW()->delete_event_items(
        normalized, deletedSlots, changed,
        static_cast<eMoneySubReason>(0xc),
        static_cast<eItemDelReason>(8), false);
    if (delCount == 0)
    {
        cMyTrace(
            __PRETTY_FUNCTION__,
            0x1c0, 5)(
            "ExpertJobMaterial::DelMaterialOfRecipe, "
            "getCurCharacInvenR()->delete_event_items , size does not "
            "match! %d %d!=%d",
            user->getCurCharacNo(), delCount, normalized.size());
        return false;
    }

    if (bodyCount == 1)
    {
        packet->put_byte(delCount + bodyCount);
        // b28d-solo-ExpertJobOnlyMaterial：P248 档②（同成员版）。
        bool removedBody = user->getCurCharacInvenW()->delete_item(
            INVEN_TYPE_EQUIP, slot, bodyCount,
            static_cast<eItemDelReason>(8), true);
        if (!removedBody)
        {
            cMyTrace(
                __PRETTY_FUNCTION__,
                0x1cf, 5)(
                "CUser::compound_item, getCurCharacInvenW()"
                "->delete_item failed , %d %d",
                user->getCurCharacNo(), slot);
            return false;
        }
        packet->put_byte(0);
        packet->put_short(slot);
        packet->put_int(bodyCount);
    }
    else
    {
        packet->put_byte(delCount);
    }

    if (delCount > 0)
    {
        for (it = deletedSlots.begin(); it != deletedSlots.end(); it++)
        {
            packet->put_byte(*it);
            it++;
            packet->put_short(*it);
            it++;
            packet->put_int(*it);
        }
    }
    return true;
}