// 0x866273c Remove_Mission：转发 charac_expand 槽 8 的任务列表。
void CUser::Remove_Mission(short kind)
{
    CMissionList_Charac* list =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    list->Remove_Mission((int)kind);
}