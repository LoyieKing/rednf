// ============================================================================
// DB_LoadEtc::GetUserPvpBase @0x08407db4：纯 ret 1。
// ============================================================================
int DB_LoadEtc::GetUserPvpBase(SIG_LOAD_ETC* sig)
{
    (void)sig;
    char frame[0x10];          // ORIG 0x08407db4 sub $0x10 + leave 同形（P31）
    (void)frame;
    return 1;
}