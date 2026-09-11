// ORIG 0x0816e0ac：static 成员（P245/P281：首参 0x8(%ebp)=sret、ret $0x4、全程无 this），
// 返回 std::string（sret）。拼接 Mobile(0..0x21) 与 Character(0..2) 限制串；
// enumToString 的 << 与 ")" 各为独立语句（ORIG 重新 lea oss，非链式复用 %eax）。
std::string ServiceRestrictManager::serviceRestrictStringQuery()
{
    std::ostringstream oss;
    for (int i = 0; i <= 0x21; ++i)
    {
        oss << "(" << 1 << "," << i << ",";
        oss << MobileRestrict::enumToString(static_cast<MobileRestrict::Enum>(i));
        oss << ")";
    }
    for (int i = 0; i <= 2; ++i)
    {
        oss << "(" << 2 << "," << i << ",";
        oss << CharacterRestrict::enumToString(static_cast<CharacterRestrict::Enum>(i));
        oss << ")";
    }
    return oss.str();
}