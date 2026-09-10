void write_incorrect_item_info(unsigned int characNo, const Inven_Item& item,
                               const char* invenType, int errorLine, int slotNo)
{
    // ORIG：cMyTrace(funcName,0x106,5)(fmt,...)；字段映射按 Inven_Item.h 布局：
    //   Seal=+0x00 / ItemType=+0x01 / ItemID=+0x02(m_addInfo) / ItemAt=GetItemAttr&0xff /
    //   add_info=+0x07(m_addInfo2) / endurance=+0x0b / extend_info=+0x0d(m_fieldd) /
    //   amp_opt=+0x11(stAmplifyOption_t 首 dword)。
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&item);
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x106, 5);
    trace("charac_no[%u],Seal[%d],ItemType[%d],ItemID[%u],ItemAt[%d],add_info[%d],endurance[%d],extend_info[%u],amp_opt[%d] - inven_type[%s] slot_no(%d), error_line(%d) \n",
          characNo,
          static_cast<unsigned int>(raw[0x00]),
          static_cast<unsigned int>(raw[0x01]),
          static_cast<unsigned int>(item.m_addInfo),
          static_cast<unsigned int>(item.GetItemAttr()) & 0xff,
          item.m_addInfo2,
          static_cast<unsigned int>(item.m_fieldb),
          *reinterpret_cast<const unsigned int*>(raw + 0x0d),
          *reinterpret_cast<const unsigned int*>(raw + 0x11),
          invenType, slotNo, errorLine);
