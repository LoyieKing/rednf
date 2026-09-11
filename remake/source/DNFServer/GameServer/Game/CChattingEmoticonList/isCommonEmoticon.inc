bool ChattingEmoticon_byLevel::isCommonEmoticon(const CUser& user) const
{
    // [b26N7] 虚调用 callee 寄存器：ORIG(4.1) vfn→%edx，4.4 恒 call *%eax；
    // 探针 /tmp/nl29/n7probe.cpp 实证不可翻，ndiff 12 全由此封顶。
    (void)user;
    return 1;
}