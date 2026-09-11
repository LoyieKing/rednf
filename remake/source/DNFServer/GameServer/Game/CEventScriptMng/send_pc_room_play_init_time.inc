// ORIG 0x810c3a4：网吧事件初始化通知。
void CEventScriptMng::send_pc_room_play_init_time(CUser* user)
{
    // ORIG 形：三段独立守卫（P133）+ verify int 返回 `== 0` 直比
    // （probe pr6.cpp h1 实证 4.4 出 test %eax; sete %al; test %al 同形）。
    if (!is_eventing(0x5c))
        return;
    CPcRoomPlayTime* entity =
        static_cast<CPcRoomPlayTime*>(get_event_entity(0x5c));
    if (entity == 0)
        return;
    if (entity->verifyUserPcRoomPlayEvent(user) == 0)
        user->sendNotiPcRoomPlayTime();
}