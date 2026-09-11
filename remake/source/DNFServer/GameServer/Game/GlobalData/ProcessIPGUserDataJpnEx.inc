// ----------------------------------------------------------------------------
// ProcessIPGUserDataJpnEx —— ORIG 0x081783ea（0xe9 字节）。
// user 判空后按 resData+0x1c 分发；未匹配状态走 LogManager 告警。
// ----------------------------------------------------------------------------
void ItemVendingMachine::ProcessIPGUserDataJpnEx(CUser* user,
                                                 WongWork::IPG::SIPGData* reqData,
                                                 WongWork::IPG::SIPGData* resData,
                                                 Stream* stream)
{
    if (user == 0)
        return;

    // ORIG 0x081783fd/0x08178431/0x08178464/0x08178498：每处条件/log 实参都
    // 两段重读 `mov 0x14(%ebp),%eax; mov 0x1c(%eax),%eax` —— 源码无 status
    // 缓存局部（缓存反产 cmpl imm,-0xc 槽比较 + 帧 +0x10）。
    if (resData->m_type == 4)
    {
        ProcessIPG_ResultQuery(user, reqData, resData, stream);
    }
    else if (resData->m_type == 0)
    {
        ProcessIPG_ResultInput(user, reqData, resData, stream);
    }
    else if (resData->m_type == 2)
    {
        ProcessIPG_ResultOutput(user, reqData, resData, stream);
    }
    else
    {
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", __PRETTY_FUNCTION__,
                              0x482, "Unhandled reply from BModule is %d",
                              resData->m_type);
    }
}