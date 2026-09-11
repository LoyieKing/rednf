// ORIG T 0x081c6ee6：直接尾调 RequestDBInventory_forchina。
void DisPatcher_SelectCharac::RequestDBInventory(CUser* user, MSG_CHARAC_SELECT& msg)

{
    // P31 帧差：ORIG sub $0x28 → dead 局部 0x10。
    volatile char dead_10[0x10]; (void)dead_10;
    RequestDBInventory_forchina(user, msg);
}
