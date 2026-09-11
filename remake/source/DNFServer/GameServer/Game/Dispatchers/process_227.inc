// ORIG 0x081ce6d4（报告 Dispatcher_Client_Spec_Statistic/process.md）：
// check_error（仅判 user 空）；param+4=msg+0xd byte；memcpy(getClientHWSpec(),msg+0xe,0xbf8)。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081ce6d4 orig/ours=42/42 ndiff-blocks=10
//   帧差 sub $28 vs $24 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x28,%esp | mov    %eax,-0x18(%ebp) | cmpl   $0x0,-0x18(%ebp)
int Dispatcher_Client_Spec_Statistic::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const int err = check_error(user, msg);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    MSG_CLIENT_SPEC_STATISTIC& req = reinterpret_cast<MSG_CLIENT_SPEC_STATISTIC&>(msg);
    // ORIG 0x081ce714: `mov 0x14(%ebp),%eax; mov %eax,-0x10(%ebp)` —— 具名 param 引用占槽；
    //   0x081ce71d `movzbl 0xd(%eax),%edx` / 0x081ce724 `mov %dl,0x4(%eax)` 均常量偏移，
    //   原 `*(uchar*)((char*)&param+4)` 会产 `lea 0x4(%eax),%edx; add $0xd,%eax` 形。
    stClientSpecStatisticParam& res = static_cast<stClientSpecStatisticParam&>(param);
    // [G3 试] disp 折叠（P311/P290）：ORIG 0x081ce71d `movzbl 0xd(%eax),%edx` 单条常量
    //   disp 直读；旧形 `*(uchar*)((char*)&req + 0xd)` 产 `add $0xd,%eax; movzbl (%eax)`（+1 条）。
    struct SpecStatView
    {
        char m_pad0d[0xd];        // +0x00..+0x0c
        unsigned char m_spec;     // +0x0d
    };
    res.m_spec = reinterpret_cast<SpecStatView*>(&req)->m_spec;
    // ORIG +0x53：getClientHWSpec 结果具名槽 -0xc（P0），memcpy 首参经槽直传
    void* dest = user->getClientHWSpec();
    memcpy(dest, reinterpret_cast<char*>(&req) + 0xe, 0xbf8);
    return 0;
}