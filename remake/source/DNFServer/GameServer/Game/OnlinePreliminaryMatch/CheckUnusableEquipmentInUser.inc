bool COnlinePreliminary::CheckUnusableEquipmentInUser(CUser* user)
{
    for (int slot = 0; slot <= 0x16; ++slot)
    {
        const Inven_Item* item =
            user->getCurCharacInvenR()->GetInvenRef(0, slot);   // T 0x84fc1de
        /* ORIG @08560a7f xor $1,%eax / test / je 续循环；msg 免槽直用
         * (@08560ab4 mov %eax,0x4(%esp)【技巧37】) */
        if (!CheckUnusableEquipment(item))
        {
            user->SendNotiPacketMessage(g_scriptStringManager_.findString(
                4, "game_server_msg_145", (bool*)0), (ENUM_MESSAGE_TYPE)0);
            return 0;
        }
    }
    return 1;
}