// ---------------------------------------------------------------------------
// 34) CUser::checkCodeChecksum(CodeHackChecksum const&, char*)
//     报告：docs/class_func_reports/CUser/checkCodeChecksum.md。ORIG 0x0867ebba。
// ---------------------------------------------------------------------------
bool CUser::checkCodeChecksum(const CodeHackChecksum& ck, char* dumpOut)
{
    // ORIG 0x0867ebba：`call IsRightChecksum; test %al,%al; je ELSE`（无 bool 局部槽，
    // 帧 sub $0x18）；then 尾 `call PopData(); mov $1,%eax; jmp END`（技巧7 return 岛）；
    // else `call PopData(dump); mov $0,%eax` fall leave。显式 true/false 直返。
    if (m_codeHackCheck.IsRightChecksum(&ck))
    {
        m_codeHackCheck.PopData();
        return true;
    }
    else
    {
        m_codeHackCheck.PopData(dumpOut);
        return false;
    }
}