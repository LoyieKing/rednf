int SDCActiveStatusSyncCommon::getDuration(ENUM_ACTIVESTATUS status,
                                           int level, void* param)
// 0x0808dc7a：param（DataSend 视图）尾部三 int 作 objectType/a4/hpPenalty；
// ORIG 透传 getActiveStatusDurationServer 返回值。
{
    // [b28c-L7] P58/P82：ORIG 0x0808dc7a 三实参为 `mov 0x31(%eax),%ebx /
    // 0x2d(%eax),%ecx / 0x29(%eax),%edx`——基址 eax 跨三次读保持、单条 disp
    // 折叠 ⇒ 原码是**类型化指针的成员访问**；`p + 0xNN` 指针算术形使 4.4
    // 每次 `add $NN,%eax; mov (%eax)`（+1 条/处）。改用具名 int 字段直访。
    const STSDCActiveStatusSyncDataSend* d =
        static_cast<const STSDCActiveStatusSyncDataSend*>(param);
    return getActiveStatusDurationServer(
        status, level, d->m_field29, d->m_field2d, d->m_field31);
}