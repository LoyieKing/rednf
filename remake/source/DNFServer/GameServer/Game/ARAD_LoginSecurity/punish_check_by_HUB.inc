// punish_check_by_HUB @ 0x08196dbf（size 0xf）
void PUNISH_CHECK::punish_check_by_HUB(SIG_LOGIN_DATA* data, int /*type*/)
{
    data->m_hubPunishFlag = 1;
}