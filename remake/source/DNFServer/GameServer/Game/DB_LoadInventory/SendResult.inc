// ============================================================================
// DB_LoadInventory::SendResult @0x0840da6e：直接转发 SendResult_forchina。
// ============================================================================
bool DB_LoadInventory::SendResult(int userIdx, int slot, SIG_LOAD_INVENTORY* sig)
{
    char frame[0x10];      // ORIG 0840xxxx sub $0x28（4.1 转发帧留白）；4.4 叶折 0x18
    (void)frame;
    return SendResult_forchina(userIdx, slot, sig);
}