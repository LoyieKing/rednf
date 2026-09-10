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
    cMyTrace("void CUser::prepareDisconnect()", 0x8ed, 0)
        ("[Taiwan, Antibot] Delete client. (sequence:%d, mid:%u)",
         G_CEnvironment()->getProcessSequence(), (unsigned int)get_acc_id());

    void* checker = GlobalData::s_antibotChecker_;
    void** vtbl = *(void***)checker;
    // §4.5 例外：antibot 类未建模（见 AntibotSend 处 b22 单），
    // Fn3=+0x08（DeleteClient 上报）/ Fn1=+0x24（DeleteClient 尾标）。
    typedef void (*Fn3)(void*, int, int, unsigned int);   // vptr+0x08
    typedef void (*Fn1)(void*, unsigned int);             // vptr+0x24
    ((Fn3)vtbl[2])(checker, G_CEnvironment()->getProcessSequence(), 0,
                   (unsigned int)get_acc_id());
    ((Fn1)vtbl[9])(checker, (unsigned int)get_acc_id());

    // ORIG 0x086487ea：直读 m_garenaAuthData(+0x6ef92) 调 getTimeStamp，不经 getGarenaAuthData
    unsigned int ts = m_garenaAuthData.getTimeStamp();
    DBUpdateExitTime::makeRequest(
        (unsigned int)get_acc_id(),
        (unsigned int)(unsigned short)get_unique_id(), ts);
