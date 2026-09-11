// ORIG _ZN5CUser24set_server_fatigue_countEs T 0x0868d32a：按当日
// get_today_time_day 在 m_map8eb50 累加，无表项则插入。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868d32a | _ZN5CUser24set_server_fatigue_countEs
// 2026-09-10（FL2CUser 类型轴）：ORIG 0x0868d32a 段 p167 实形 = value_type 构造
//   `_ZNSt4pairIKhsEC2IRhRKsEE`（第二实参 **const short&**）；我方 `C1IRhRsEE`
//   （非 const）⇒ 原码形参为顶层 const（顶层 cv 不进 mangled，声明侧不变）。
void CUser::set_server_fatigue_count(const short v)
{
    unsigned char todayDay =
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())
            ->get_today_time_day();
    std::map<unsigned char, short>::iterator it = m_map8eb50.find(todayDay);
    if (it != m_map8eb50.end())
    {
        // b28d-G3C 2026-09-10（P167 调用点差集）：ORIG 0x0868d3a? 段只调
        //   `_Rb_tree_iterator::ptEv` **一次**（`call ptEv; movzwl 0x2(%eax),%edx;
        //   mov %edx,%ecx; movzwl -0x2c(%ebp),%edx; lea (%ecx,%edx,1),%edx;
        //   mov %dx,0x2(%eax)` → 同一 %eax 读改写）。我方原 `it->second =
        //   static_cast<short>(it->second + v);` 两次写 `it->second` ⇒ 两次 ptEv
        //   调用（EXTRA ×1）。改复合赋值（单次 ->）。
        it->second += v;
    }
    else
    {
        // b28d-solo-CUser p167：ORIG 0x0868d32a 直接构造 value_type
        //   `pair<unsigned char const, short>::pair<unsigned char&, short const&>`
        //   （无 make_pair 调用）⇒ 改 value_type 直构（P123/P61）。
        m_map8eb50.insert(std::map<unsigned char, short>::value_type(todayDay, v));
    }
}