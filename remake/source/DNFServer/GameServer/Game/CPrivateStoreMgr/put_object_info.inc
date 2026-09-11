// ORIG T 0x85cadf6（报告 docs/class_func_reports/private_store__CPrivateStorePos/
// put_object_info.md）：校验宿主 store 状态与卖家位置后序列化私商对象信息：
//   byte(1) | short(seller unique id) | int+str(店名) | byte(state!=1)
//   | byte(pos.a) | byte(pos.b) | short(pos.field2) | short(pos.field4)
//   | byte(isDollCreated)
// [b28d-solo-CPrivateStoreMgr] P167 归零。残 154 行：寄存器分配（ORIG
// `movzbl %al,%ebx` vs 我们 `%eax`）+ 槽号整体偏移（trace 对象 ORIG -0x58 vs
// 我们 -0x28，N82 槽复用）。
bool CPrivateStorePos::put_object_info(PacketGuard& p)
{
    if (m_pStore == 0)
    {
        // [b28d-solo] P221 全表达式临时链：ORIG 0x85cadf6 段 trace 对象槽
        // -0x58/-0x48/-0x38/-0x28（先写块占深槽），具名对象令 4.4 反向。
        cMyTrace(__PRETTY_FUNCTION__, 0xd0b, 0)("NOTEXIST_ERROR : private_store(null)");
        return false;
    }

    // ORIG 0x85cadf6 P167 差集：GetPos×9 / GetSeller×6 / GetState×4 /
    // GetSubject×4 / isDollCreated×3 / getCurCharacVill×2 / get_area×2 ——
    // 原码逐使用点重读 getter（P157/P183 禁缓存），日志变参亦直接读 getter
    // 表达式（P92）；缓存 pos/seller/state/subject 局部丢 20 个调用点。
    if (m_pStore->GetState() != 1 && m_pStore->GetState() != 2)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xd11, 0)(
            "STATE_ERROR : private_store_state(%d)", m_pStore->GetState());
        return false;
    }

    if (m_pStore->GetSeller() == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xd17, 0)(
            "NOTEXIST_ERROR : private_store(%d, %d) user(null)",
            (int)m_pStore->GetPos()->m_a, (int)m_pStore->GetPos()->m_b);
        return false;
    }

    // 未生成 doll 时才校验 store 登记位置与卖家当前位置一致。
    if (!m_pStore->isDollCreated())
    {
        if ((int)m_pStore->GetPos()->m_a !=
                (int)(signed char)m_pStore->GetSeller()->getCurCharacVill() ||
            (int)m_pStore->GetPos()->m_b !=
                m_pStore->GetSeller()->get_area(false))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xd23, 0)(
                "DIFFERENT_ERROR : private_store(%d,%d), user(%d,%d), uid(%d)",
                (int)m_pStore->GetPos()->m_a, (int)m_pStore->GetPos()->m_b,
                (int)(signed char)m_pStore->GetSeller()->getCurCharacVill(),
                m_pStore->GetSeller()->get_area(false),
                m_pStore->GetSeller()->get_unique_id() & 0xffff);
            return false;
        }
    }

    p.put_byte(1);
    p.put_short((unsigned short)m_pStore->GetSellerIndex());

    p.put_int((int)m_pStore->GetSubject()->length());
    p.put_str(m_pStore->GetSubject()->c_str(),
              (int)m_pStore->GetSubject()->length());

    // [b28d-solo] ORIG 0x85cb1xx `setne al; movzx edx,al` ⇒ 直接传 bool 值
    // （三目形 4.4 生 mov$1/jmp/mov$0 岛）。
    p.put_byte(m_pStore->GetState() != 1);

    p.put_byte((int)m_pStore->GetPos()->m_a);
    p.put_byte((int)m_pStore->GetPos()->m_b);
    p.put_short((short)m_pStore->GetPos()->m_field2);
    p.put_short((short)m_pStore->GetPos()->m_field4);

    // ORIG 0x85cb21c 尾：`call isDollCreated; test; je else; put_byte(1); jmp;
    // else: call isDollCreated; xor $1; test; je end; put_byte(0)` = 两独立守卫
    // 各重读一次 getter（P48/P184 死块照抄；三目形只出 1 次调用）。
    if (m_pStore->isDollCreated())
        p.put_byte(1);
    else if (!m_pStore->isDollCreated())
        p.put_byte(0);
    return true;
}