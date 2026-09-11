// [b28d-lane2] verdict=IDENTICAL ndiff=0（ORIG 0x084e7fe2）：混合残差；gap0。
int Inter_LoadPuUserCheck::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    char* packet = data;
    // [b28d-lane2] 源侧 uchar 读旋钮（探针 /tmp/l2probe/p9.cpp v3/v4/v5 实证）：
    // ORIG 0x084e7ff1 单步 `movzbl (%eax),%edx` 只在「源表达式为 unsigned char 左值」
    // 时复现；`*packet`（char 源）4.4 恒产两段 `movzbl (%eax),%eax; mov %eax,%edx`。
    // 字段仍为 uchar（CUser.h 冻结），零 ABI/布局影响。
    user->m_field796d8 = *reinterpret_cast<unsigned char*>(packet);
    return 0;
}