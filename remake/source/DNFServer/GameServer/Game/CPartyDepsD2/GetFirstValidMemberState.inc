// ---------------------------------------------------------------------------
// GetFirstValidMemberState —— ORIG T 0x085ba574
// 报告：docs/class_func_reports/CParty/GetFirstValidMemberState.md
// 第一个有效成员的 CUser::get_state()；无有效成员返回 0。
// ---------------------------------------------------------------------------
char CParty::GetFirstValidMemberState()
{
    for (int i = 0; i < 4; ++i)
    {
        // [b28d-51] P0/N96 清绑（原 `register uchar v asm("eax")`）；ORIG
        // 0x085ba574 O@9-12 `call; xor$1; test; je` 由裸 `!call` 形承载。
        if (!_checkValidUser(i))
        {
            continue;
        }
        return (char)memberUser(this, i)->get_state();   // thunk 0x080da38c
    }
    return 0;
}