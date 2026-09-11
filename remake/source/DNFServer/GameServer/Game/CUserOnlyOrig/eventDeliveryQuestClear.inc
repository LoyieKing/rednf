// [b28d-22 N82] ORIG 0x086645a4：p167=CLEAR，gap=-1；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x086645a4：末位实参套 P226 裸语句表达式
// `({ GetServerGroupToPvP(); })` ⇒ 4.4 把该值升格 %esi（原内联实参落 %ebx），
// 复现 ORIG「sg→%esi、cno→%ebx、accId→%eax」三实参 callee-saved 分配 ⇒ AE。
// 新范式（P226 扩展）：裸语句表达式可翻转多实参 callee-saved 的 ebx↔esi 选择。
void CUser::eventDeliveryQuestClear(int a, int b,
                                    std::vector<std::pair<int,int> >& items,
                                    int d, int e, int f)
{
    QuestClearDeliveryMsg msg;                                  // C2 0x8697914
    // [b28d-solo-CUserOnlyOrig P0/P52] ORIG 0x086645bd/0x086645ca/0x086645d7 三个
    // 实参按右到左求值并存 callee-saved（sg→%esi、cno→%ebx、accId→%eax），
    // 无任何栈槽；旧写法三个具名局部生 -0x14/-0x10/-0xc 槽并把 msg 挤到 -0x38
    // （ORIG -0x2c）⇒ 内联实参。
    // [b28d-solo-CUserOnlyOrig P226 扩展] 纯内联实参时 4.4 给首个求值者 %ebx
    // （ebx↔esi 与 ORIG 反）；末位实参改裸语句表达式 `({ ...; })` 即翻正。
    msg.setBaseData(1, get_acc_id(), getCurCharacNo(), ({ GetServerGroupToPvP(); }));  // ORIG 0x86978ec
    msg.m_questParamA = a;                                      // +0x10  ← ebp+0xc
    msg.m_itemsPtr = &items;                                    // +0x20  ← ebp+0x14
    msg.m_paramD = d;                                           // +0x14  ← ebp+0x18
    msg.m_paramE = e;                                           // +0x18  ← ebp+0x1c
    msg.m_paramF = f;                                           // +0x1C  ← ebp+0x20
    GlobalData::s_event_manager->eventDeliveryNotify(msg.m_type, msg);
}