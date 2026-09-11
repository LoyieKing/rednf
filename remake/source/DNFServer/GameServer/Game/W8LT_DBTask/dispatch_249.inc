// [SweepW8LT b28d] AE：P214③ 四数组 `{0,0,0,0,0,0}`→`{0}` 复现 4.1/4.4
// 循环清零展开（11 条/数组），gap 20→0（234/234）；ORIG 084285ae
bool DB_UpdateItemDropStatistics::dispatch(int userIdx, int slot,
                                           Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int channelNo = 0;
    int dropType = 0;
    // P214③：单元素 `{0}` 触发 4.1/4.4 循环清零展开（11 条/数组：and $0xfffffffc
    // + 4 字节步进回跳）；写满 6 个 0 则退化为 6 条 movl（gap 20 的根因）。
    unsigned int stackable[6] = { 0 };
    unsigned int equip[6] = { 0 };
    unsigned int recipe[6] = { 0 };
    unsigned int artifact[6] = { 0 };

    *stream >> channelNo;
    *stream >> dropType;
    for (int i = 0; i <= 5; ++i)
        *stream >> stackable[i];
    for (int i = 0; i <= 5; ++i)
        *stream >> equip[i];
    for (int i = 0; i <= 5; ++i)
        *stream >> recipe[i];
    for (int i = 0; i <= 5; ++i)
        *stream >> artifact[i];

    db->set_query(
        "inSert into log_item_drop(occ_date,channel_no,drop_type,stackable_common,stackable_uncommon,stackable_rare,stackable_unique,stackable_epic,stackable_chronicle,equip_common,equip_uncommon,equip_rare,equip_unique,equip_epic,equip_chronicle,recipe_common,recipe_uncommon,recipe_rare,recipe_unique,recipe_epic,recipe_chronicle,artifact_common,artifact_uncommon,artifact_rare,artifact_unique,artifact_epic,artifact_chronicle) values(cast(now() as date),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
        channelNo, dropType,
        stackable[0], stackable[1], stackable[2], stackable[3], stackable[4],
        stackable[5],
        equip[0], equip[1], equip[2], equip[3], equip[4], equip[5],
        recipe[0], recipe[1], recipe[2], recipe[3], recipe[4], recipe[5],
        artifact[0], artifact[1], artifact[2], artifact[3], artifact[4],
        artifact[5]);
    return db->exec(true);
}