// ORIG 0x08333ea6：importCompoundAvatarTable(dir,path,this) == 0。
bool CCompoundAvatar::LoadScript(const char* dir, const char* path)
{
    return importCompoundAvatarTable(dir, path,
                                     reinterpret_cast<tagCompoundAvatarTable*>(this)) == 0;
}