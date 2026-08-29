// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + RestrictCategory 报告
// 报告：docs/class_func_reports/RestrictCategory/enumToString.md
// enumToString @0x0816d454：return enumStringRestrict[param_1]（char 指针表，步长 4）。
// enumStringRestrict 为 ORIG 局部数据符号（d 0x093700a0，static storage），不 extern 声明。
#ifndef DNF_GAME_RESTRICTCATEGORY_H_
#define DNF_GAME_RESTRICTCATEGORY_H_

class RestrictCategory
{
public:
    enum Enum
    {
        // 枚举项按 enumStringRestrict 表（0x093700a0）4 项回填：
        // "NullPolicy"/"AuthMobilePolicy"/"CreateCharacterPolicy"/"SecurityPolicy"
        // （表 0x093700a0 objdump 实测 4 项，步长 4）。
        RESTRICT_CATEGORY_NULL = 0,          // "NullPolicy"
        RESTRICT_CATEGORY_AUTH_MOBILE = 1,   // "AuthMobilePolicy"
        RESTRICT_CATEGORY_CREATE_CHARACTER = 2,  // "CreateCharacterPolicy"
        RESTRICT_CATEGORY_SECURITY = 3,      // "SecurityPolicy"
        RESTRICT_CATEGORY_COUNT = 4
    };
    static char const* enumToString(Enum);   // T 0x0816d454【推断返回 char const*（表内字符串指针）】
};

#endif  // DNF_GAME_RESTRICTCATEGORY_H_
