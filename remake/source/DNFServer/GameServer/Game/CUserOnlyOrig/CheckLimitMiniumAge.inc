// ---------------------------------------------------------------------------
// 42) CUser::CheckLimitMiniumAge(int ageLimit, int& uniqueLow16)
//     报告：docs/class_func_reports/CUser/CheckLimitMiniumAge.md。ORIG 0x0864fc3e。
// ---------------------------------------------------------------------------
bool CUser::CheckLimitMiniumAge(int ageLimit, int& outUnique)
{
    // ORIG 0x0864fc4a/0x0864fc7e：两处独立 `call GetAge` 直落 `lea -0x1(%eax),%edx`
    // 免槽（P48 双读 + 技巧37）；具名 age 占 -0xc 且省掉第二次 call。
    // probe p15.a4：`(unsigned)call - 1u`（cast 先于减）才产 `lea -0x1(%eax),%edx`
    // 单段；`(unsigned)(call - 1)` 恒 sub+mov 两段（a1/a2）。
    if (((unsigned int)GetAge()) - 1u < (unsigned int)ageLimit)
    {
        outUnique = (int)(get_unique_id() & 0xffff);
        return false;
    }
    if (((unsigned int)GetAge()) - 1u == (unsigned int)ageLimit)
    {
        if (!IsOverBirthDay())
        {
            outUnique = (int)(get_unique_id() & 0xffff);
            return false;
        }
    }
    return true;
}