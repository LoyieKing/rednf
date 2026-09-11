// b28c-L20: 正极性 bool 分支（P168a①：ORIG 0x081e8862 无 xor 取反，`test %al; je <mov $0 岛>`
//   fall-through return 1）；GetRepeatEvent 结果右值内联（P0，无槽往返）。
int Dispatcher_SeriaRoom_Ani_Deco_Event::check_error(CUser* user, MSG_BASE& msg)
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x69)->IsEventing(0))
        return 1;
    return 0;
}