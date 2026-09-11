// ---------------------------------------------------------------------------
// 31. CUser::get_ingame_event_history(int, pair<int,int>&) T 0x0868d50c【报告缺失·无参版报告≠本符号】
void CUser::get_ingame_event_history(int key, std::pair<int,int>& out)
{
    // [b28d-22 修法 P0/P48] ORIG 0x0868d515/0x0868d537 两次 `lea 0x8eb68(%eax)` 直取成员
    // （无 hist 引用槽）；0x0868d56b 单次 ptEv、0x0868d579/0x0868d57b 8 字节整体赋值
    // ⇒ 删具名引用、out 整体赋值。
    std::map<int, std::pair<int,int> >::iterator it = m_map8eb68.find(key);
    if (it != m_map8eb68.end()) {
        out = it->second;
    }
}