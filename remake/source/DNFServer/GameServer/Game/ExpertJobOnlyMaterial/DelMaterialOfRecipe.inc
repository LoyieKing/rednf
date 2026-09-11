// ORIG _ZN10expert_job17ExpertJobMaterial19DelMaterialOfRecipeEP5CUserRKSt
// 6vectorISt4pairIiiESaIS5_EEiP11PacketGuardRb @0x0849c204：
// 先 delete_event_items 清单材料（moneyReason 0xc / delReason 8 / 不改
// changed），失败记 trace 返 false；成功 put_byte(delCount+1)、删除配方
// 物品本体槽（count 固定 1）并按 byte/short/int 三连写本体条目，随后对
// 每个事件删除槽位同样三连写入。
// b28d-solo-ExpertJobOnlyMaterial: AE — P248 档② removedBody 局部 bool 化
// 复现 ORIG `movzbl -0x9; xor $1; test %al,%al; je`（char 局部 4.4 恒折 cmpb）。
bool ExpertJobMaterial::DelMaterialOfRecipe(
    CUser* user, const IntPairVector& materials,
    int slot, PacketGuard* packet, bool& changed)   // [Q3PrettSweepA] typedef 拼写对齐 ORIG prett
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
            __PRETTY_FUNCTION__,
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
            __PRETTY_FUNCTION__,
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
}