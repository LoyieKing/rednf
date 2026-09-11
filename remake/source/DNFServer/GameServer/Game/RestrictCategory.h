// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + RestrictCategory 报告
// 报告：docs/class_func_reports/RestrictCategory/enumToString.md
// ORIG enumToString @0x0816d454（5 insn）：
//   mov 0x8(%ebp),%eax; mov &_ZN16RestrictCategoryL18enumStringRestrictE(,%eax,4),%eax
// ⇒ `RestrictCategory` 是 **namespace**：enum Enum + namespace 内 static 指针表 +
//   自由函数 enumToString（无 this；namespace 函数与 class 静态成员函数 mangling 相同，
//   故 `_ZN16RestrictCategory12enumToStringENS_4EnumE` 两侧一致）。
// [PoolB b28f 探针实证] 同一源码（namespace 内 `static const char* const t[4]`）在
//   c6root 4.4.7 生成 `_ZN16RestrictCategoryL18enumStringRestrictE`（L = internal linkage），
//   c5root 4.1.2 生成**无 L** 形 `_ZN16RestrictCategory18enumStringRestrictE`
//   ⇒ ORIG 该 TU 系 4.4.x 编译（与 P327「ORIG 混编 4.4」一致）；函数内 static（旧形）
//   在 4.4 下恒发射 `_ZZN16RestrictCategory12enumToStringENS_4EnumEE18enumStringRestrict`
//   （名字不匹配 ORIG）⇒ 必须建 namespace 才能发射 ORIG 符号名。
// 本头 = RestrictCategory 唯一声明点（原 ServiceRestrictManager.h 的本地 namespace
//   定义已并入此处 ⇒ 消除同名实体 ODR 双定义；两套枚举名值相同、全部保留）。
#ifndef DNF_GAME_RESTRICTCATEGORY_H_
#define DNF_GAME_RESTRICTCATEGORY_H_

namespace RestrictCategory
{
enum Enum
{
    // 表 enumStringRestrict（0x093700a0）4 项，值 0..3：
    // "NullPolicy"/"AuthMobilePolicy"/"CreateCharacterPolicy"/"SecurityPolicy"。
    ENUM_0 = 0,
    Enum_1 = 1,   // ORIG isRestricted(user,1,0xf/0x10)：随机选项重置/再铸限制类别
    Enum_2 = 2,   // ORIG CreateCharacterRestrictPolicy code（initialize 注册 4 策略 0-3）
    Enum_3 = 3,
    // 语义别名（与上列同值；enumToString 表项直接对应）。
    RESTRICT_CATEGORY_NULL = 0,               // "NullPolicy"
    RESTRICT_CATEGORY_AUTH_MOBILE = 1,        // "AuthMobilePolicy"
    RESTRICT_CATEGORY_CREATE_CHARACTER = 2,   // "CreateCharacterPolicy"
    RESTRICT_CATEGORY_SECURITY = 3,           // "SecurityPolicy"
    RESTRICT_CATEGORY_COUNT = 4
};

char const* enumToString(Enum);   // T 0x0816d454【推断返回 char const*（表内字符串指针）】
}

#endif  // DNF_GAME_RESTRICTCATEGORY_H_
