bool CCompoundAvatar::LoadScript(const char* dir, const char* path)
{
    return importCompoundAvatarTable(dir, path,
                                     reinterpret_cast<tagCompoundAvatarTable*>(this)) == 0;
