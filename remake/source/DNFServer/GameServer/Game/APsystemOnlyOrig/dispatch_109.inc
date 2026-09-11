// ============================================================================
// DB_InsertPvPBuddy —— ORIG dispatch 0x0843a9a2（报告 dispatch.md）。
// ORIG 本体即恒返 1（9 字节：push ebp/mov esp,ebp/mov $1,%eax/pop ebp/ret；
// 三参均未使用），非绕过。
// ============================================================================
bool DB_InsertPvPBuddy::dispatch(int userIdx, int slot, Stream* stream)
{
    return 1;
}