// b28c-1 AE: P223 三岛 + P194 反形裸短路后 0 差。
void BlueMarble::giveBonusItem(CUser* user)
{
    // b28c-1：P223 多岛形（ORIG 三岛 0x080d9905/908/90b：user==0、state 条件假、
    //  m_addInfo==0 各落相邻尾标签）。
    if (user == 0) goto DONE1;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x4ac,
                                  "BlueMarble::giveBonusItem slot(%d)", seat);
        }
        else
        {
            // ORIG 0x080d9876..9900：bad=(state!=2||userstate!=1) 物化 mov$1/$0 后
            // test al; jne 岛2（P194 X1 反形，裸短路形）。
            if (getState() != 2 || getUserState(seat) != 1)
                goto DONE2;
            Inven_Item item;
            item.m_addInfo = getItemIndex(seat);
            item.m_addInfo2 = 1;
            if (item.m_addInfo == 0)
                goto DONE3;
            {
                insertItem(seat, item, (eItemAddReason)0x41);
                resetIndexInfo(seat);
            }
            goto DONE3;
        }
    }
DONE1:
    ;
DONE2:
    ;
DONE3:
    ;
}