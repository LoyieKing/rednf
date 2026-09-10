void CSharedServerMessageManager::UpdateServerMessageByBroadcast(int nIndex,
                                                                 char* pMsg, char* pMsg2,
                                                                 int nEndTime)
{
    // [b28d-solo-CSharedServerMessageManager 判词 · AE] ORIG 0x08601476（fresh 129=129，
    // TU near=0 diff=0，p167 16/16 CLEAR）。原 N168a③b 判词作废：①IsOnIndex 改正极性
    // if/else 后 ORIG 的 `call; test %al,%al; je` 直出；②IsSetServerMessage 返回类型
    // bool 化（P292/P248 档①）消 `test; sete; test; je` 四段；③字段同型化（P211/P254）。
    stSERVER_MESSAGE* pCur = getCurMessage(nIndex);
    if (pCur == 0)
    {
        // [b28d-S112] P292/P214①：ORIG 0x086014b0 为 `call IsOnIndex; test %al,%al;
        // je <trace 岛>`（无 xor $1）⇒ 正极性 if/else；负极性形恒多 xor 段。
        if (m_adapter.IsOnIndex(nIndex))
        {
            int nKind = m_adapter.GetShareKindWithIndex(nIndex);
            if (nKind == -1)
                return;

            stSERVER_MESSAGE local;
            local.m_kind = (unsigned char)nKind;
            local.m_index = (unsigned char)nIndex;
            local.m_lenMsg2 = (unsigned int)strlen(pMsg2);
            strncpy(local.m_msg2, pMsg2, local.m_lenMsg2 + 1);
            local.m_lenMsg = (unsigned int)strlen(pMsg);
            strncpy(local.m_msg, pMsg, local.m_lenMsg + 1);
            local.m_endTime = nEndTime;

            m_event.m_vecMsg.push_back(local);
            ++m_event.m_count;
            pCur = &local;
        }
        else
        {
            cMyTrace tr(__PRETTY_FUNCTION__,
                        0x1c6, 0);
            tr("SERVER_MESSAGE : NOT EXIST INDEX and NOT EXIST DATA");
            return;
        }
    }
    else
    {
        // [b28d-S112] P211/P254：ORIG 0x086015b9 `mov -0x10(%ebp),%edx; mov %eax,0x4(%edx)`
        // 为 LHS-first 直存 ⇒ 字段 unsigned int + 显式 cast（探针 /tmp/gs_probe3.cpp a2）。
        pCur->m_lenMsg = (unsigned int)strlen(pMsg);
        strncpy(pCur->m_msg, pMsg, pCur->m_lenMsg + 1);
        pCur->m_lenMsg2 = (unsigned int)strlen(pMsg2);
        strncpy(pCur->m_msg2, pMsg2, pCur->m_lenMsg2 + 1);
        pCur->m_endTime = nEndTime;
    }

    if (!IsSetServerMessage())
        setLoadingComplete();
