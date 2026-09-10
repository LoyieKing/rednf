bool ExpertJobMaterial::DelMaterialOfRecipe(
    CUser* user, const std::vector<std::pair<int, int> >& materials,
    int slot, PacketGuard* packet, bool& changed)
{
    int materialCount = 1;   // 配方本体固定计 1 份

    std::vector<int> deletedSlots;
    std::vector<int> unusedSlots;                 // ORIG 0x849c221：未使用局部（保形）
    std::vector<int>::iterator it;                // ORIG 0x849c22c：迭代器前置构造
    int delCount = user->getCurCharacInvenW()->delete_event_items(
        materials, deletedSlots, changed,
        static_cast<eMoneySubReason>(0xc),
        static_cast<eItemDelReason>(8), false);
    if (delCount == 0)
    {
        cMyTrace(
            "bool expert_job::ExpertJobMaterial::DelMaterialOfRecipe("
            "CUser*, const IntPairVector&, int, PacketGuard*, bool&)",
            0x288, 5)(
            "ExpertJobMaterial::DelMaterialOfRecipe, "
            "getCurCharacInvenR()->delete_event_items , size does not "
            "match! %d %d!=%d",
            user->getCurCharacNo(), delCount, materials.size());
        return false;
    }

    packet->put_byte(delCount + materialCount);

    // b28d-solo-ExpertJobOnlyMaterial：P248 档②——局部改 bool 复现 ORIG
    // `movzbl -0x9; xor $1,%eax; test %al,%al; je`（char 局部 4.4 恒折 cmpb）。
    bool removedBody = user->getCurCharacInvenW()->delete_item(
        INVEN_TYPE_EQUIP, slot, materialCount,
        static_cast<eItemDelReason>(8), true);
    if (!removedBody)
    {
        cMyTrace(
            "bool expert_job::ExpertJobMaterial::DelMaterialOfRecipe("
            "CUser*, const IntPairVector&, int, PacketGuard*, bool&)",
            0x292, 5)(
            "CUser::compound_item, getCurCharacInvenW()->delete_item "
            "failed , %d %d",
            user->getCurCharacNo(), slot);
        return false;
    }

    packet->put_byte(0);
    packet->put_short(slot);
    packet->put_int(materialCount);

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
