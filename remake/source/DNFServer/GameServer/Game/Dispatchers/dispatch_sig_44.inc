// ==== Dispatcher_MouseRegister::dispatch_sig (ORIG 0x082637fe) ====
// §6.2 巨件核销：P167 89/89 [0缺0多]；gap 636→662（+26 全帧 N82 槽号错位，本帧
// 4096×2 解密缓冲使 4.4 vs 4.1 槽分配天然漂移）；残差归因 N82（帧/槽号+寄存器），
// 语义完整（详见 docs/asm_translation_patterns.md 负例表）。真修：
// ① pkgType==3 分支补 operator* ×2→operator<< ×2→operator-> 写入链（P103/P5，
//   原简写致 P167 -2operator*/-1operator->）；主成功路径同形补齐；
// ② 删我方多缺 memset(&bufView)（ORIG 无——m_encPwd 由 get_binary、m_encPwdCopy
//   由 memcpy 覆盖，pkgType 槽不被读）；
// ③ 始段删 user==0 守卫（ORIG 无，get_state 两次独立调用双 je 同目标，P133）；
// ④ cryptoType 三守卫 !(a&&b&&c) 还原为 ORIG 独立 ==0/>0xd/==5 判（P133）；
// ⑤ Decrypt 双调用返回处理还原 ORIG「首次 jne 汇聚1、次次 je 汇聚0」单出口发错
//   （P48 独立无 setne 物化）；
// ⑥ checkValiedPassword ×2 还原 bool 汇聚物化（xor$1/test 首项 jne 尾项 je，P22）。
// PacketDispatcher_Secu.cpp 密保卡注册链：包类型分发 → 解密双密码 → shake 减扰
// → checkValiedPassword ×2 → 写 SIG_MOUSE_REGISTER 进 DB_Q。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082637fe orig/ours=636/660 ndiff-blocks=82
//   帧差 sub $20bc vs $20cc + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: movl   $&_ZZN24Dispatcher_MouseRegister12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | lea    -0x89(%ebp),%eax | movl   $&_ZZN24Di
int Dispatcher_MouseRegister::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x26380a..：无 user==0 守卫；get_state==3 / ==2 两次独立调用，
    // 双 je 同一目标（P133 独立守卫）。
    if (!(user->get_state() == 3 || user->get_state() == 2))
    {
        return LineFunc(0x27c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    // ORIG 段直证：读包前无 `movb $0x0,-0x89(%ebp)`（直接 `lea -0x89(%ebp),%eax`）⇒
    // P0/P328⑤ 删预清零（原初始化令帧 0x20bc→0x20cc 并整体槽位下移 0xc）。
    unsigned char pkgType;
    if (!packet.get_byte(pkgType))
    {
        return LineFunc(0x282,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    if ((int)user->getPad()->getFailCnt() > 2)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x74, 0x88);
        return 0;
    }
    if ((int)user->getPad()->getCancelCnt() > 10)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x74, 0x8b);
        return 0;
    }

    unsigned char decPwd1[4096] = { '\0' };   // ORIG local_108d（gcc 内联清零 P4）
    unsigned char decPwd2[4096] = { '\0' };   // ORIG local_208d
    int decLen1 = 0x1000;                     // ORIG local_30[1]（第一次 Decrypt 出参）
    int decLen2 = 0x1000;                     // ORIG local_30[0]
    unsigned char cryptoType = 0xe;           // ORIG -0x2d
    unsigned char cryptoTypeCopy = 0xe;       // ORIG -0x2e

    // 未认证的 pad 必须先通过激活 + 认证检查（pkgType != 1 时）。
    if (pkgType != 1 && !user->getPad()->isActivate())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x74, 1);
        return 0;
    }
    if (pkgType != 1 && !user->getPad()->isCertified())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x74, 0x86);
        return 0;
    }

    // pkgType==3：不携带密文，直接上报 DB（重置请求）后返回。
    if (pkgType == 3)
    {
        Stream* stream =
            GlobalData::s_stream_pool->Acquire("PacketDispatcher_Secu.cpp", 0x2ba);
        CStreamGuard guard(stream, true);
        // ORIG 0x263ac8..：写入经 operator*（0x080c8c46）×2 → operator<< ×2 →
        // GetInBuffer 经 operator->（P103），与主成功路径同形。
        **guard << (int)0xa3;
        **guard << (int)user->GetUID();
        SIG_MOUSE_REGISTER* sig = guard.operator->()->GetInBuffer<SIG_MOUSE_REGISTER>();
        sig->m_accId = user->get_acc_id();
        sig->m_pkgType = (unsigned int)pkgType;
        strncpy(sig->m_password1, (char*)decPwd1, 7);
        strncpy(sig->m_password2, (char*)decPwd2, 7);
        strncpy(sig->m_webAddress, user->getWebAddress(), 0x10);
        user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)0,
                                       (ENUM_SECU_SERVICE_ACTION)4);
        static_cast<XNuclear::CHades*>(user->getHades())->ChangeSecurity(
            (XNuclear::ENUM_HADES_SECURITY)1, 0x26u);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        return 0;
    }

    if (!packet.get_byte(cryptoType))
    {
        return LineFunc(0x2dc,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG 0x263c75..：三条独立守卫（==0 或 >0xd 或 ==5）双 jcc 同一目标（P133）。
    if (cryptoType == 0 || cryptoType > 0xd || cryptoType == 5)
    {
        return LineFunc(0x2e3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    // 包体：type(-0x89)+enc32(-0x88..)+encCopy(+0x21 起)；cp=ORIG 双槽操作。
    struct MouseRegDecryptBuf   // pack(1)：帧布局即按字节连续偏移
    {
        unsigned char m_pkgType;             // +0x00 (-0x89)
        unsigned char m_encPwd[32];          // +0x01 (-0x88) get_binary 目标 / Decrypt 源 1
        unsigned char m_encPwdCopy[32];      // +0x21 (-0x68) memcpy 副本 / Decrypt 源 2
    };
    MouseRegDecryptBuf bufView;              // 局部视图（取址用）
    // ORIG 无 memset(&bufView)：m_encPwd 由 get_binary 覆盖、m_encPwdCopy 由 memcpy
    // 覆盖（+1 memset 为我方多缺调用，P167 EXTRA；pkgType 槽不被读）。

    int encLen = 0;      // ORIG -0x38
    int encLenCopy = 0;  // ORIG -0x34
    if (!packet.get_int(encLen))
    {
        return LineFunc(0x2e8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_binary((char*)bufView.m_encPwd, encLen))
    {
        return LineFunc(0x2ea,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    encLenCopy = encLen;
    cryptoTypeCopy = cryptoType;
    memcpy(bufView.m_encPwdCopy, bufView.m_encPwd, encLen);

    // ORIG 0x263d8e..：解密入口即 Cipher::Decrypt(int,uchar const*,int,uchar*,int*)
    // （T 0x0809751e，ORIG 报告调用实证），索引 shl $0x4 直读全局表（ORIG 4×
    // sizeof(Cipher*)，当前头即 Cipher* 表——勿再乘 sizeof）；两次调用返回 eax
    // 各 xor$1/test（P48 独立、无局部缓存物化）。
    Sanicova::CPad* padCipher = user->getPad();   // P48 双臂各自调用
    Cipher& cipher = *GlobalData::s_secu_cipher_keystring_manager_
        [padCipher->GetSelectedCipherIdx() & 0xffff].cipher;
    const bool firstOk = cipher.Decrypt(cryptoType, bufView.m_encPwd, encLen,
                                        decPwd1, &decLen1);
    Sanicova::CPad* padCipher2 = user->getPad();
    Cipher& cipher2 = *GlobalData::s_secu_cipher_keystring_manager_
        [padCipher2->GetSelectedCipherIdx() & 0xffff].cipher;
    const bool secondOk = cipher2.Decrypt(cryptoTypeCopy, bufView.m_encPwdCopy,
                                          encLenCopy, decPwd2, &decLen2);
    // ORIG 0x263df1..：两次 Decrypt 返回各 xor$1/test；首次 jne 汇聚「1」、
    // 次次 je 汇聚「0」——单 SendCmdErrorPacket 在汇聚点（test al 后）发一次。
    if (!firstOk || !secondOk)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x74, 1);
        return 0;
    }

    for (int i = 0; i <= 3; ++i)
    {
        decPwd1[i] -= user->getPad()->GetShakeData(i);
        decPwd2[i] -= user->getPad()->GetShakeData(i);
    }

    // ORIG 0x263f26..：P48 两次独立调用；P22 布尔取反——每 call 后 xor$1/test，
    // 首项 jne 汇聚「1」、尾项 je 汇聚「0」，再统一 test al 分流。
    const bool bad = !checkValiedPassword((char*)decPwd1) ||
                     !checkValiedPassword((char*)decPwd2);
    if (bad)
    {
        return LineFunc(0x343,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    Stream* stream =
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Secu.cpp", 0x346);
    CStreamGuard guard(stream, true);
    // ORIG 0x263fbe..：operator* ×2 → operator<< ×2 → operator->（P103）。
    **guard << (int)0xa3;
    **guard << (int)user->GetUID();
    SIG_MOUSE_REGISTER* sig = guard.operator->()->GetInBuffer<SIG_MOUSE_REGISTER>();
    sig->m_accId = user->get_acc_id();
    sig->m_pkgType = (unsigned int)pkgType;
    strncpy(sig->m_password1, (char*)decPwd1, 7);
    strncpy(sig->m_password2, (char*)decPwd2, 7);
    strncpy(sig->m_webAddress, user->getWebAddress(), 0x10);
    if (pkgType == 1)
    {
        user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)0,
                                       (ENUM_SECU_SERVICE_ACTION)2);
    }
    else if (pkgType == 2)
    {
        user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)0,
                                       (ENUM_SECU_SERVICE_ACTION)3);
    }
    if (pkgType == 1)
    {
        static_cast<XNuclear::CHades*>(user->getHades())->ChangeSecurity(
            (XNuclear::ENUM_HADES_SECURITY)1, 0x25u);
    }
    else if (pkgType == 2)
    {
        static_cast<XNuclear::CHades*>(user->getHades())->ChangeSecurity(
            (XNuclear::ENUM_HADES_SECURITY)1, 0x24u);
    }
    Sanicova::CPad* padPass = user->getPad();
    memcpy(sig->m_padPassword, padPass->getPassword(), 0x24);
    Sanicova::CPad* padAdv = user->getPad();
    sig->m_advideChangePwd = (char)(unsigned int)padAdv->getAdvideChangePwd();
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return 0;
}