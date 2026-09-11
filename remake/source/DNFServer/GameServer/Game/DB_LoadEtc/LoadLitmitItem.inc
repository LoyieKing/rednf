// ============================================================================
// DB_LoadEtc::LoadLitmitItem @0x08405c60：sub $0x30 后直接 ret 1（如实空体）。
// ============================================================================
bool DB_LoadEtc::LoadLitmitItem(SIG_LOAD_ETC* sig)
{
    (void)sig;
    char frame[0x30];          // ORIG 0x0840xxxx sub $0x30,%esp + leave 同形
    (void)frame;
    return true;
}