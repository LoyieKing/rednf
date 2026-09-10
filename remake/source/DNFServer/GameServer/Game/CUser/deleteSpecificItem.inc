void CUser::deleteSpecificItem(const std::vector<std::pair<int, int> >& list,
                               std::vector<std::pair<int, int> >& out)
{
    if (getCurCharacR() == 0)
        return;

    // ORIG 0x0867dbc4 `movl $0x0,-0x28` 预清 + 0x0867dbcb 无 mgr 具名局部
    //   （GetInstancePrivateStoreMgr 结果经 %eax 直传）⇒ 声明与赋值分两句（P198③）。
    private_store::CPrivateStore* store = 0;
    store = (private_store::CPrivateStore*)
        private_store::GetInstancePrivateStoreMgr()->FindStoreSeller(this);
    // 2026-09-10（P225 物化复现）：ORIG 0x0867dbc8 段为
    //   `cmpl $0x0,-0x28(%ebp); setne %al; test %al,%al; je <跳过>`（4.1 bool 物化），
    //   4.4 对裸 `if (store != 0)` 恒折直跳 je（无 setcc）⇒ 改 while+break 形
    //   （P225 实证：唯一复现 setne;test;je 的源形）。
    while (store != 0)
    {
        // 真成员调用（b22 消解旧 §4.5 例外）：ISeller 槽 +0x04
        // RemovePrivateStore(bool)，CPrivateStoreMgr.h:54/217 权威声明
        // （ORIG T 0x85c5fd0，vtable @0x8cbeb60 实证），编译器自动出
        // vtbl[1] 间接调用，与原手写槽取法同形。
        store->RemovePrivateStore(false);
        break;
    }

    _deleteSpecificItemInven((INVEN_TYPE)0, list, out, 10, 0x15);
    _deleteSpecificItemInven((INVEN_TYPE)1, list, out, 3, 0x137);
    _deleteSpecificItemCargo(list, out);
    _deleteSpecificItemAccountCargo(list, out);
    _deleteSpecificItemTitieBook(list);

    // ORIG this+0x71b98 实证：无 mailBox 具名局部，每处重读 `mov 0x71b98(%eax),%eax`
    //   （0x0867dcad/0x0867dcd3/0x0867de88/0x0867dea0/0x0867dee0/0x0867dff4）。
    if (m_premium.m_mailBox != 0)
    {
        std::vector<std::pair<int, int> > mailDel;
        WongWork::CMailBox::CMail* mail = 0;   // ORIG 0x0867dccc `movl $0x0,-0x24` 预清
        ((WongWork::CMailBox*)m_premium.m_mailBox)->setMailIterator();
        // ORIG 0x0867dce4 `rep stos` 0x15 dwords ⇒ rmList[21]，rmList[0] 即计数。
        unsigned int rmList[21] = {0};
        // b28d-solo-CUser p167：ORIG 单 getNextMail 调用点（while 条件内赋值）；
        //   for(init;cond;incr) 形产 2 个调用点（EXTRA ×1）⇒ 改 while 赋值形。
        while ((mail = ((WongWork::CMailBox*)m_premium.m_mailBox)
                           ->getNextMail()) != 0)
        {
            // b28d-solo-CUser p167 修复（ORIG 0x0867dba0 实形）：超时判定用
            //   `_checkTimeoutItem(mail->GetPackageItem())` 值拷贝（ORIG -4 GetPackageItem、
            //   -1 _checkTimeoutItem；我方原用 mail->IsTimeOver(nowSec)+getCurSec）；
            //   移除索引用 `mail->getMailIdx()`（ORIG -2 getMailIdx；我方原 GetLetterId）；
            //   无效物品分支每字段重调 GetPackageItem（P157）。
            bool remove = false;
            Inven_Item pkg = mail->GetPackageItem();
            if (_checkTimeoutItem(pkg))
            {
                remove = true;
            }
            else
            {
                // ORIG 0x0867dd6a `cmp $-1,%eax; setne %al; test; je` ⇒ 先判 != -1
                if (_isMatchedItem(mail->GetPackageItem().m_addInfo, list) != -1)
                {
                    remove = true;
                }
                else
                {
                    if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                            mail->GetPackageItem().m_addInfo))
                    {
                        // b28d-solo-CUser p167：ORIG 实例化
                        // `make_pair<unsigned long, int>(unsigned long&&, int&&)`（右值形）
                        // ⇒ 显式 cast 成右值（P61/P315）。
                        mailDel.push_back(std::make_pair(
                            (unsigned long)mail->GetPackageItem().m_addInfo,
                            (int)mail->GetPackageItem().m_addInfo2));
                        remove = true;
                    }
                }
            }
            // ORIG 0x0867de56：rmList[0] 自增后写入，无上界检查
            if (remove)
            {
                rmList[++rmList[0]] = mail->getMailIdx();
                ((WongWork::CMailBox*)m_premium.m_mailBox)
                    ->RemoveMail(mail->getMailIdx());
            }
        }
        if (rmList[0] != 0)
        {
            WongWork::CMailBoxHelper::ReqDBRemoveMail(this, rmList, false);
            PacketGuard packet;
            InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
            pkt->put_header(0, 0x62);
            pkt->put_int((int)rmList[0]);
            for (int i = 1; i <= rmList[0]; ++i)
                pkt->put_int((int)rmList[i]);
            pkt->finalize(true);
            Send(packet);
        }
        RewardItem2DeleteInvalidItem("Mail", mailDel);
    }
    _deleteSpecificRedeemItem(list);
