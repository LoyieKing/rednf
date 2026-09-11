// ORIG T 0x088d6622：直接以 m_info（this+0x30）调 importScript(dir, file)，
// 返回值透传。
// [b24F] DONE：返回类型 bool→int（ORIG call 后直接 leave;ret 透传，无 test;setne）。
bool BlueMarbleScriptManager::initBlueMarbleInfo(const char* dir, const char* file)  // [P330] 与 PvPTypes.h 同步
{
    return m_info.importScript(dir, file);
}