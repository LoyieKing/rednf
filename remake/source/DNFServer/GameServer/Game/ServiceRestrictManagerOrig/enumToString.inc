// ORIG T 0x0816d454 _ZN16RestrictCategory12enumToStringENS_4EnumE。
namespace RestrictCategory
{
// ORIG 0x093700a0 `_ZN16RestrictCategoryL18enumStringRestrictE`（d 段 0x10）：
// namespace 作用域 static 指针表。4.4.7 只有此形发射 L 名（c6root 实测）；
// 旧形（class 静态成员函数内 static）在 4.4 下恒发射
// `_ZZN16RestrictCategory12enumToStringENS_4EnumEE18enumStringRestrict`（名字不匹配）。
// 数组元素类型与 ORIG 段（d = 可重定位/可写）一致：元素为 `const char*`，数组本体非 const。
static const char* enumStringRestrict[4] = {
    "NullPolicy",
    "AuthMobilePolicy",
    "CreateCharacterPolicy",
    "SecurityPolicy"
};

char const* enumToString(Enum e)
{
    return enumStringRestrict[e];
}

}
