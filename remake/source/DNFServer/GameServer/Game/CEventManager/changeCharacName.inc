// ---------------------------------------------------------------------------
// LinkClosureB-2 追加：changeCharacName（ORIG T 0x08116276 sz 0x98，
// 报告 docs/class_func_reports/CEventManager/changeCharacName.md 全文已读）：
// 遍历事件数组 0..0xa5：槽非空且 IsEventing(0)（ORIG vptr+0x34 槽）为真 →
// 调该事件 vptr+0x20 槽（ORIG changeCharacName 槽，头内声明序与 ORIG vtable
// 对位：_ZTV10CEventBase @0x08b4bae0 实测 +0x20=changeCharacName、
// +0x34=IsEventing），实参 (user, key, name, flag)。
// b28d-solo-CEventManager: [N7 封顶] 同 dailyresetData——50=50 助记符序列全等，
// 残余=虚调用目标 %ecx vs 4.4.7 %eax + 索引寄存器轮转；P282 复核 +0x34=IsEventing、
// +0x20=changeCharacName 槽与头声明序一致。ORIG 0x08116276。
// b28d-solo 双后端实测：4.4.7 NEAR / 4.4.6 后端 IDENTICAL（N7 敏感族）。
void CEventManager::changeCharacName(CUser* user, int key, char* name, int flag)
{
    for (int i = 0; i <= 0xa5; ++i)
    {
        if (m_events[i] != 0)
        {
            if (m_events[i]->IsEventing(0))
                m_events[i]->changeCharacName(user, key, name, flag);
        }
    }
}