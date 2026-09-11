char* WongWork::CSecurityCardCenter::generateServerKey(char* out)
{
    // [b28d-solo-W6_SecMiscDeps 判词 · IDENTICAL_AE] ORIG 0x085fe0ec：random5/timeCount/
    // channelNo/serverGroup 全程 callee-saved（%ebx/%edi/%esi/%eax）无栈槽往返 ⇒
    // 原码无这些具名局部，必须内联进 snprintf 实参位（P0）；仅 curSec 占 -0x1c
    // 具名槽。旧具名局部版每值各多 store+load 且无 push edi/esi/ebx。
    // ORIG T 0x085fe0ec (size 0x91)：
    // "%02d%02d%010d%05d%05d"(env+0x378, env+0x1b0, curSec, timeCount, rand5)
    _updateTimeCount();
    const int curSec = s_systemTime_.getCurSec();
    // CEnvironment 服务器标识字段尚未具名化（域外批次负责），按既有口径偏移读。
    OS_API::snprintf(out, 0x21, "%02d%02d%010d%05d%05d",
                     G_CEnvironment()->m_serverEnvir.m_serverGroup,  // +0x378
                     G_CEnvironment()->m_serverEnvir.m_channelNo,    // +0x1b0
                     curSec, m_timeCount, get_rand_int(99999));
    return out;
}