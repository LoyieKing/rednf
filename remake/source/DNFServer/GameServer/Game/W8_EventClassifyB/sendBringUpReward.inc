bool CObjectBringUp::sendBringUpReward(CUser* user,
                                       std::vector<int>& rewardVec,
                                       unsigned char& flag)
{
    if (user == 0)
    {
        LogManager::logFormat(
            1, "EventClassify.cpp", __PRETTY_FUNCTION__, 0x7ad,
            "CObjectBringUp::sendBringUpReward(pUser(NULL))");
        return false;
    }

    // [b28d-W8EventB 实测] ORIG 0x810fed4/0x810fedb 函数头两条 store：
    // `movl $0x0,-0x14(%ebp)`（result 预清，P198①）+ `movl $0x1,-0x10(%ebp)`
    // （死局部，全函数无读取点 ⇒ P171 死写入照抄）。两分支共用同一 result 槽。
    int result = 0;
    int deadSlot = 1;
    (void)deadSlot;

    for (::std::vector< ::std::pair<ObjectBringUpScript::ObjectBringUpRule,
                                  ::Action_SendMail> >::iterator it =
             static_cast<ObjectBringUpScript*>(m_script1c)
                 ->m_sendMails28.begin();
         it != static_cast<ObjectBringUpScript*>(m_script1c)
                   ->m_sendMails28.end();
         ++it)
    {
        if (!verifyGiveBringUpReward(
                it->first, (int)user->getCurUseBringUpMaterialTotal()))
            continue;

        if (static_cast<ObjectBringUpScript*>(m_script1c)
                ->m_sendMails28[1].second.m_itemIdx == it->second.m_itemIdx)
        {
            if (user->getBringUpRewardFlag())
                continue;

            CEventActionMng actionMng;
            result = actionMng.process_action_send_mail(
                user, it->second, false);
            if (result != 0)
            {
                LogManager::logFormat(
                    1, "EventClassify.cpp", __PRETTY_FUNCTION__, 0x7c3,
                    "CObjectBringUp::sendBringUpReward(m_id:%d, "
                    "charac_no:%d, reward:%d)",
                    it->second.m_itemIdx, (int)user->getCurCharacNo(),
                    (int)user->get_acc_id());
                return false;
            }
            std::copy(it->second.m_vec38.begin(),
                      it->second.m_vec38.end(),
                      std::back_inserter(rewardVec));
            it->second.m_vec38.clear();
            ++flag;
            user->setBringUpRewardFlag(true);

            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("EventClassify.cpp",
                                                   0x7cc),
                true);
            guard.operator*()->operator<<((int)0x2ca);
            guard.operator*()->operator<<((int)-1);
            SIG_OBJECT_BRING_UP* sig =
                guard.operator->()->GetInBuffer<SIG_OBJECT_BRING_UP>();
            sig->m_accId = user->get_acc_id();
            sig->m_characNo = user->getCurCharacNo();
            // m_serverGroup 为 4B 字段（SigTypes_D.h，ORIG 0x0810ffb8 4B 写）；
            // (unsigned char) cast 保留 ORIG `movzbl` 取值宽度。
            sig->m_serverGroup =
                (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
            sig->m_usedMaterialCnt =
                user->getCurUseBringUpMaterialCount();
            sig->m_rewardFlag = user->getBringUpRewardFlag();
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            user->resetCurUseBringUpMaterialCount();
        }
        else
        {
            CEventActionMng actionMng;
            result = actionMng.process_action_send_mail(
                user, it->second, false);
            if (result != 0)
            {
                LogManager::logFormat(
                    1, "EventClassify.cpp", __PRETTY_FUNCTION__, 0x7e2,
                    "CObjectBringUp::sendBringUpReward(m_id:%d, "
                    "charac_no:%d, reward:%d)",
                    it->second.m_itemIdx, (int)user->getCurCharacNo(),
                    (int)user->get_acc_id());
                return false;
            }
            std::copy(it->second.m_vec38.begin(), it->second.m_vec38.end(),
                      std::back_inserter(rewardVec));
            it->second.m_vec38.clear();
            ++flag;
        }
    }
    return true;
