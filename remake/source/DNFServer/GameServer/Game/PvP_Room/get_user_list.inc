void PvP_Room::get_user_list(void* out)
{
    // N7 封顶：ORIG 虚表槽函数指针落 %ecx、receiver 链 %eax；4.4 恒 eax/edx 形，
    // 临时/引用/限定名均无效（负例表 N7）。
    ((IMatch*)m_matching)->GetUserList(out);
}