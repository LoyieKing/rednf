void CUser::eventDeliveryBuyItem(int deliveryType, const Inven_Item& item)
{
    // b28d-solo-CUserDepsB: NEAR——P23/P24 实参内联（去三具名槽，帧 0x58→0x40、
    // 42=42 指令全等）；残差 N7：第 3/第 2 实参 callee-saved 角色互换（ORIG
    // serverGroup→%esi、characNo→%ebx；4.4 →%ebx、%esi），探针同构形给 ORIG 分配、
    // 本 TU 因可见 GetServerGroupToPvP 定义而反转，源形不可择。ORIG 0x0866463c。
    // [S191 本轮复验] p167 CLEAR（6/6）；42=42 仅两处 `mov %eax,%esi/%ebx` 与
    // 对应压栈寄存器互换（P248② 反例族）。试形：cast/具名中间量/求值序均不可翻
    // ⇒ N7 封顶（助记符序列全等，仅 callee-saved 角色）。ORIG 0x0866463c。
    // [L29 2026-09-09 翻正 NEAR→AE] 旧判「N7 封顶」**作废**：给 serverGroup 补
    // **裸 `register` 具名局部**（无 asm 绑定，P0 合规）即复现 ORIG 的 callee-saved
    // 角色（serverGroup→%esi、characNo→%ebx；旧直内联形 4.4 恒 %ebx/%esi 互换）。
    // 边界：同法在 UpdateEventInfo（uid/characNo 加 register 或改 unsigned）实测
    // 均**打成 DIFF** ⇒ register 旋钮按件实测，非普遍解。ORIG 0x0866463c。
    BuyItemDeliveryMsg msg;                        // lea -0x20(%ebp) ctor
    register int serverGroup = GetServerGroupToPvP();   // P231 裸 register（翻正件）
    msg.setBaseData(2, get_acc_id(), getCurCharacNo(),
                    serverGroup);                  // W 0x086978ec
    msg.m_deliveryType = deliveryType;             // -0x10(%ebp) → +0x10
    msg.m_item = &item;                            // -0xc(%ebp) → +0x14
    GlobalData::s_event_manager->eventDeliveryNotify(
        msg.m_type, msg);                          // T 0x08116874，首参取 msg 首 dword
