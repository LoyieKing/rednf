// ---------------------------------------------------------------------------
// getNpcMoodFromString — ORIG @ 0x89faf46。
// strncmp(name, X, 10)：good→2、normal→1、bad→0、其余→3。
// ---------------------------------------------------------------------------
int getNpcMoodFromString(const char* name)
{
    if (std::strncmp("good", name, 10) == 0)   return 2;
    if (std::strncmp("normal", name, 10) == 0) return 1;
    if (std::strncmp("bad", name, 10) == 0)    return 0;
    return 3;
}