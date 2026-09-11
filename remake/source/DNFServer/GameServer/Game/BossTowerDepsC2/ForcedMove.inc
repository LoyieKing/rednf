// ============================================================================
// pvp_assault::CForcedMoveCharacter —— 强制移动请求值对象（仅本 TU 使用，
// 类定义置于 .cpp 内避免跨 TU ODR 风险；后续 CExtraMatrix/CDynamicDisposition/
// CStaticDisposition::MoveUser(CForcedMoveCharacter*) 批次可迁至权威头）。
//
// 布局依据 ForcedMove.md 汇编读取偏移（this+0x08..+0x1c）；ORIG 唯一构造点
// （CAssaultPlace::ForcedMoveAtPowerWarPlace 栈上组装）不调用任何构造函数、
// 无 vtable 写入证据，故 +0x00..+0x07 未建模。
// ============================================================================
// pvp_assault::CForcedMoveCharacter 权威完整定义见 CAssaultMgr.h（本 TU 本地镜像已删，
// 避免与全域权威头 ODR 冲突）。以下 ForcedMove ORIG T 0x082e567c 报告全文转写。
// [b28d-L8] N82 | 块序 + 槽平移（ORIG 0x08xxxxxx）
void CForcedMoveCharacter::ForcedMove()
{
    // [M3S5 2026-09-11 · P222 落地] ORIG 0x082e567c 头部 `je +0x210` 直跳
    //   epilogue；早退形（`if (m_user == 0) return;`）我方多 2 条（`212: nop` +
    //   send_to_area 后的 `jmp 205`，our 160 > orig 158）；改「if (m_user != 0)
    //   正向包裹」后 our **158 = orig 158（gap 0）**、TU 四数零劣化 ⇒ 按 §6.4 保留。
    if (m_user != 0)
    {
        // ORIG：%al 标志直传（无内存 bool 中转）。先比村庄（movsbl 后有符号
        // 比较），不等即走 move_area；相等再比 get_area(false)，均等才原地
        // set_position。短路结果无槽物化 ⇒ P226 语句表达式（旧具名 int
        // needMove 落槽致帧/指令膨胀）。ORIG 0x082e567c。
        if (({ static_cast<int>(static_cast<signed char>(m_user->getCurCharacVill())) !=
                m_destVill ||
              m_user->get_area(false) != m_destArea; }))
        {
            G_GameWorld()->move_area(m_user, m_destVill, m_destArea, m_x, m_y,
                                     m_dir, false, 0, 0, 0);
        }
        else
        {
            m_user->set_position(static_cast<unsigned short>(m_x),
                                 static_cast<unsigned short>(m_y),
                                 static_cast<char>(m_dir));
        }

        PacketGuard packet;
        packet.put_header(0, 0x8a);
        packet.put_short(
            static_cast<int>(static_cast<unsigned short>(m_user->get_unique_id())));
        packet.put_short(m_x);
        packet.put_short(m_y);
        packet.put_byte(m_dir);
        packet.finalize(true);

        // [VerifyN2 2026-09-11 · 三轴免疫件实测翻正] 旧判 N82「块序封顶」作废：
        //   写成「否定前置空 then + else」即复现 ORIG 0x082e567c 的块序
        //   （send_to_area → ~PacketGuard → jmp epilogue；正向 if 形 4.4 把
        //   ~PacketGuard 放到 jmp 之后）⇒ 件级 DIFF→AE（158=158，ndiff 6→0）。
        if (m_destVill == 0 && m_destArea == 0)
        {
        }
        else
        {
            G_GameWorld()->send_to_area(m_destVill, m_destArea, packet);
        }
    }
}