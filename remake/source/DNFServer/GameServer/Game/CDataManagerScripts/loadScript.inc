// [b28d-solo-CDataManagerScripts] AE：函数头声明序 `token, ok, idx, data`（P212/P309：4.4 头部声明按先声明浅 ⇒ data 落最深 -0x50、idx -0x44，与 ORIG 一致）；fresh clean 149=149；ORIG 0x08ac1e60。
bool DeleteInvalidItemScript::loadScript()
{
    // [X1 形] ORIG 7 检查点（load 0x08ac1e7c / ScanType 0x08ac1ec3 / ScanInt-ok×3
    // 0x08ac1efb.. / ScanStr→ok 回读×2 0x08ac1fdd/0x08ac201a）均 xor$1+test 带分支
    // 消费者形。[b28c-18-unbind] P0 硬约束：register asm("eax") 绑定已全域清除，
    // 落 N96+N18a+P168a④ 判词——无绑定即不可复现 movzbl;xor$1;test 寄存器物化形，
    // 封顶自然形（bool callee 裸形直产 xor 段者除外，P168a①）。
    if (!(loadRDARScriptFile("Etc/", "tw_DeleteInvalidItem.etc")))
        return false;

    std::string token;
    bool ok = false;
    int idx;
    DeleteInvalidItemData data;
    for (;;)
    {
        if (!(ScanType(token, true)))
            break;
        if (token == "[delete item]")
        {
            for (;;)
            {
                idx = ScanInt(&ok);
                if (!(ok))
                    break;
                int rewardIdx = ScanInt(&ok);
                if (!(ok))
                    break;
                int count = ScanInt(&ok);
                if (!(ok))
                    break;
                data.init();
                data.m_field0 = idx;        // ORIG：data.f0=首个 int（与 key 同值）
                data.m_field4 = rewardIdx;
                data.m_field8 = count;
                m_deleteMap.insert(std::make_pair(idx, data));
            }
        }
        else if (token == "[send mail title]")
        {
            ok = (bool)ScanStr(&m_field0);  // ORIG：先物化入 ok 槽再回读
            if (!(ok))
                break;
        }
        else if (token == "[send mail content]")
        {
            ok = (bool)ScanStr(&m_field4);
            if (!(ok))
                break;
        }
    }
    unloadRDARScriptFile();
    return true;
}