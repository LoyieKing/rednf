// 0x086487ea
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086487ea | _ZN5CUser17prepareDisconnectEv
void CUser::prepareDisconnect()
{
    // 0x086487ea 主分支（prepareDisconnect.md 反编译 C 全文转写）：state<2 早退；
    // 非人证用户记 hack 0x3b9、SSO 强存计数 + DB_ForceSaveSimpleSSO；安全卡/密保
    // 请求态为 1 时 Dispatcher_SecurityCardAuthCancel::process(this)；antibotChecker
    // vtable +0x08 四参注销 / +0x24 两参注销；DBUpdateExitTime(accId, uid, ts)。
    if (get_state() < 2)
        return;
    if (!isHumanCertified())
    {
        setHumanCertified(true);
        getHackAnalyzer()->addServerHackCnt(this, (WongWork::ENUM_HACKTYPE)0x3b9, 1, 0, 0);
        m_humanCertifyErrorCnt += 1;   // ORIG this+0x8e0fc 实证：SSO 强制保存计数
        DB_ForceSaveSimpleSSO::makeRequest((unsigned int)get_acc_id(), m_humanCertifyErrorCnt);
    }
    // b28d-lane5 复测（P226 语句表达式）：ORIG 0x08648a?? 无 cancelAuth 槽——两 getter
    //   短路后 `mov $1,%eax / mov $0,%eax / test %al,%al / je`（零槽物化岛）；
    //   旧具名 bool 形多 `movb $0x0,-0x15 / movb $0x1,-0x15` 与帧 +0x10。
    if (({ getSecurityCard()->getRequestState() == 1 ||
           getPad()->getRequestState() == 1; }))
        Dispatcher_SecurityCardAuthCancel::process(this);

    // b28d-lane5 复测（P227 全表达式临时链）：ORIG 0x08648a5x 在 cMyTrace ctor **之前**
    //   右起求 varargs 并以 %esi 跨 ctor 保活（`mov %eax,%esi` … `mov %esi,0xc(%esp)`）。
    cMyTrace(__PRETTY_FUNCTION__, 0x8ed, 0)
        ("[Taiwan, Antibot] Delete client. (sequence:%d, mid:%u)",
         G_CEnvironment()->getProcessSequence(), (unsigned int)get_acc_id());

    // §4.5 例外：antibot 类未建模（见 AntibotSend 处 b22 单），
    // Fn3=+0x08（DeleteClient 上报）/ Fn1=+0x24（DeleteClient 尾标）。
    typedef void (*Fn3)(void*, int, int, unsigned int);   // vptr+0x08
    typedef void (*Fn1)(void*, unsigned int);             // vptr+0x24
    // b28d-G3C 2026-09-10（P183 去具名缓存）：ORIG 0x086488f?/0x0864891? 对
    //   `GlobalData::s_antibotChecker_` **每次重算**——`mov 0x941f7e4,%eax;
    //   mov (%eax),%eax; add $0x8,%eax; mov (%eax),%esi`（取 vtbl[2]）以及调用前
    //   `mov 0x941f7e4,%edx`（作 this 实参）；+0x24 段同形（vtbl[9]）。
    //   我方原有 `void* checker`(-0x14) 与 `void** vtbl`(-0x10) 两个独占具名槽
    //   （每处多 store+load，帧 0x30→0x40）。改为全表达式重算。
    // [g03 · P58/P-g03-d 类型化函数指针数组修法（翻正落地）] ORIG 该处为 `mov <A>,%eax; mov (%eax),%eax; add $0x8,%eax;
    //   mov (%eax),%esi`（解引用结果直落 callee-saved）；void*** 三层 cast 使 4.4 多一条
    //   `mov %eax,%esi` 中转 ⇒ 改为真实类型化指针数组 In（Fn3* / Fn1*），定址形不变。
    ((Fn3)(*(Fn3**)GlobalData::s_antibotChecker_)[2])(
        GlobalData::s_antibotChecker_, G_CEnvironment()->getProcessSequence(), 0,
        (unsigned int)get_acc_id());
    ((Fn1)(*(Fn1**)GlobalData::s_antibotChecker_)[9])(
        GlobalData::s_antibotChecker_, (unsigned int)get_acc_id());

    // ORIG 0x086487ea：直读 m_garenaAuthData(+0x6ef92) 调 getTimeStamp，不经 getGarenaAuthData；
    //   2026-09-11（H3CUser，P183）：ORIG 全函数无 `mov %eax,-0xc(%ebp)` ts 具名槽
    //   （我方旧形多 store+load 一对）⇒ getTimeStamp 内联进实参位。
    DBUpdateExitTime::makeRequest(
        (unsigned int)get_acc_id(),
        (unsigned int)(unsigned short)get_unique_id(),
        m_garenaAuthData.getTimeStamp());
}