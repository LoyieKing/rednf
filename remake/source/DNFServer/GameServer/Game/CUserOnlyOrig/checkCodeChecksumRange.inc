bool CUser::checkCodeChecksumRange(const CodeHackChecksumEx& ckEx, const CodeHackCheckRangeEx& rangeEx)
{
    // [b28d-22 修法 P59/P0] ORIG 0x0867ea87 `jge 0x0867ebae`（直达尾部 `mov $1,%eax`）
    // = `if (resumeAt < now) { body } return true;` 正向包裹；0x0867eaef strlen 结果直压
    // （无 len 槽）、0x0867eb11 `mov %eax,%ebx` 跨 GetFileName 保活。
    long resumeAt = GetResumeChecksumTime();
    long now = GlobalData::s_systemTime_.getCurSec();
    if (resumeAt < now)
    {
        if (m_codeHackCheck.PushData(&ckEx) != 1)
            return false;
        PacketGuard pg;
        pg.put_header( 0, 0x6f);
        pg.put_int( strlen(rangeEx.GetFileName()));
        // [b28d-solo-CUserOnlyOrig P167 重载选错真 bug] ORIG 0x0822b770 命中
        // `_ZN18InterfacePacketBuf7put_strEPKci`（const char* 版），我方命中
        // `...7put_strEPci`（char* 版）⇒ ORIG 实参静态类型为 const char*；
        // 本仓 GetFileName() 定义在 CodeHackChecksum.cpp 返回 char*（非本泳道文件），
        // 故在调用点 const_cast 收窄到 const char*（零指令成本）。
        pg.put_str( const_cast<const char*>(rangeEx.GetFileName()), strlen(rangeEx.GetFileName()));
        pg.put_int( rangeEx.m_start104);    // Range+0x104
        pg.put_int( rangeEx.m_size108);     // Range+0x108
        pg.finalize( true);
        Send(pg);
    }
    return true;
