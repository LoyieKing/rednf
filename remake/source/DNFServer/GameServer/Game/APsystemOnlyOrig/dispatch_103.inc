// ============================================================================
// DB_LoadPvPBuddy::dispatch —— ORIG 0x0843a998
// ============================================================================
// ORIG 0x843a998：push/mov esp/mov $1,%eax/pop/ret —— 恒返 1（无参使用）。
bool DB_LoadPvPBuddy::dispatch(int userIdx, int slot, Stream* stream)
{
    return 1;
}