// ORIG 0x810bf00：活动 0x193 的任务计数命中时累加一次物品计数。
// [B24D判词 N] 语义正确：user!=0 → get_event_entity(0x193)→is_event_quest→
//   IncCurCharacEventAddItemCount(1)。diff=ORIG 句尾 `jmp;nop`（4.1 合并跳转+对齐）
//   我方直接 leave;ret，27/25 纯 lowering/对齐表示差异，语义等价。
void CEventScriptMng::process_add_quest_count_db(CUser* user, int questId)
{
    if (user == 0)
        return;
    CAddItemCountDB* entity =
        static_cast<CAddItemCountDB*>(get_event_entity(0x193));
    if (entity != 0)
    {
        if (entity->is_event_quest(questId))
            user->IncCurCharacEventAddItemCount(1);
    }
}