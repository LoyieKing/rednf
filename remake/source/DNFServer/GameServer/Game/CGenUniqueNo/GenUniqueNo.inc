char* CGenUniqueNo::GenUniqueNo(char* out)
{
    // b28d-solo-CGenUniqueNo（AE）：ORIG 0x084b76d8 首实参 = Server_Envir.m_serverGroup
    // (+0x378)、次实参 = m_channelNo (+0x1b0)（双证：ORIG get_server_group 0x08106ce0
    // 读 +0x378、get_channel_no 0x0814aaca 读 +0x1b0）——原码把 serverGroup 具名物化到
    // -0xc、channelNo 内联作第二实参。旧代码两字段写反（真语义 bug：唯一号两个 %02d 段
    // 互换）。调用目标同 ORIG OS_API::snprintf（_ZN6OS_API8snprintfEPcjPKcz @0x0858c81c），
    // 旧为 libc snprintf（p167 各 MISSING/EXTRA 1）。sec@-0x10、serverGroup@-0xc、
    // count→%ebx、帧 push %ebx/sub $0x34 全对齐。
    _updateTimeCount();
    int sec = GlobalData::s_systemTime_.getCurSec();
    int serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    OS_API::snprintf(out, 0x14, "%02d%02d%10d%05d",
                     serverGroup,
                     G_CEnvironment()->m_serverEnvir.m_channelNo,
                     sec, (unsigned int)m_nTimeCount);
    return out;
