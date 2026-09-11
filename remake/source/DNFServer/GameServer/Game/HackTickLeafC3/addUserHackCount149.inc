// ============================================================================
// addUserHackCount149(CUser*) —— ORIG T 0x830dc9d [_Z19addUserHackCount149P5CUser]
// 报告反编译：
//   pCVar1 = CUser::getHackAnalyzer(param_1);
//   addServerHackCnt(pCVar1, param_1, 0x198, 1, 0, 0);
//   return 1;
// 实参序（汇编压栈）：type=0x198(ENUM_HACKTYPE), count=1, a=0, b=0。
// ============================================================================
bool addUserHackCount149(CUser* user)
{
    user->getHackAnalyzer()->addServerHackCnt(
        user, (WongWork::ENUM_HACKTYPE)0x198, 1u, 0u, 0u);
    return true;
}