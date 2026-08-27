// W7_BuildHarmonize: dnf_game_cxx.sh 部分路由为指令级对齐 ORIG 而强制
// -std=gnu++98（如 Arad_Script.cpp / DNFLexWrapper*.cpp / ScriptStringManager.cpp），
// 但项目共享头（SigTypes.h/CDataManager.h 等）大量使用 C++0x 关键字 static_assert。
// 单 TU 检查链（check_tu_game_orig.sh）统一 gnu++0x 故无此问题。
// 本头经 dnf_game_cxx.sh 以 -include 自动注入到这些 gnu++98 TU：
// 在非 C++0x 编译下把 static_assert 展开为 C++03 合法的编译期断言 typedef，
// 断言语义（编译期失败即报错）不变；在 gnu++0x 下不做任何定义。
#ifndef DNF_STATIC_ASSERT_COMPAT_H
#define DNF_STATIC_ASSERT_COMPAT_H

#ifndef __GXX_EXPERIMENTAL_CXX0X__
#define DNF_SA_CAT_(a, b) a##b
#define DNF_SA_CAT2_(a, b) DNF_SA_CAT_(a, b)
#define static_assert(cond, msg) \
    typedef char DNF_SA_CAT2_(dnf_sa_, __LINE__)[(cond) ? 1 : -1]
#endif

#endif // DNF_STATIC_ASSERT_COMPAT_H
