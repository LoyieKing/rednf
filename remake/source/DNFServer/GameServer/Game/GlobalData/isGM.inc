// [b28d-solo-GlobalData] 修：P56+P293 单表达式 if-return（end 先/find 后 + 分支物化） | ORIG 0x08109346
bool WongWork::CGMAccounts::isGM(unsigned int mid)
{
    // ORIG 0x0810934c..0x08109364：key 先整体零初始化（两条 movl $0x0，
    // 成员声明序 mid→type），再 type=3、mid=mid（P202① 预清两句式）。
    WongWork::CGMAccounts::stGMInfo_t key = {0};   // POD 无用户 ctor（ORIG nm 无 C1）
    key.type = 3;
    key.mid = mid;
    // [b28d-solo-GlobalData] ORIG 0x081093a6：end() 先求值（-0x18）→ find（sret -0x14，
    // 内联 last=第二次 end -0x10、first=begin -0xc）→ ne(this=find 结果, arg=end)；
    // 尾 `test%al;je;mov$1;jmp;mov$0` 为 P56 分支物化（return 表达式直写会少 5 条）。
    // 单表达式形复现 ORIG 求值序（P293：具名 it 形 4.4 恒 find 先/end 后）。
    if (std::find(m_gmList.begin(), m_gmList.end(), key) != m_gmList.end())
    {
        return true;
    }
    return false;
}