// [b28d-L1] N82 封顶：ORIG 0x0819eaf8；P167 CLEAR（3/3）⇒ 语义完成。
// [AradMisc-E1/E2 2026-09-11 · DIFF→**AE** 70/70 gap0] 残余为 cMyTrace 实参装载/帧槽形态差，
//   两旋钮落地：① 去 `raw` 具名指针（ORIG 0x0819eb01 起一律 `mov 0xc(%ebp),%eax` 基址访问，
//   无 raw 槽）⇒ 70→66；② 临时对象直呼 `cMyTrace(...)(...)`（ORIG 0x0819eb73 ctor 调用位于
//   **全部**复杂实参求值之后、装参之前；具名 `trace` 形实测多 4 条）⇒ 66→70 = ORIG。
void write_incorrect_item_info(unsigned int characNo, const Inven_Item& item,
                               const char* invenType, int errorLine, int slotNo)
{
    // ORIG：cMyTrace(funcName,0x106,5)(fmt,...)；字段映射按 Inven_Item.h 布局：
    //   Seal=+0x00 / ItemType=+0x01 / ItemID=+0x02(m_addInfo) / ItemAt=GetItemAttr&0xff /
    //   add_info=+0x07(m_addInfo2) / endurance=+0x0b / extend_info=+0x0d(m_fieldd) /
    //   amp_opt=+0x11(`*(const unsigned*)((char*)&item + 0x11)`：ORIG 0x0819eb04 用
    //   `add $0x11,%eax; mov (%eax),%edi` 两段形 = cast 口径，非成员直访 disp 形)。
    cMyTrace(
        __PRETTY_FUNCTION__,
        0x106, 5)("charac_no[%u],Seal[%d],ItemType[%d],ItemID[%u],ItemAt[%d],add_info[%d],endurance[%d],extend_info[%u],amp_opt[%d] - inven_type[%s] slot_no(%d), error_line(%d) \n",
                  characNo,
                  static_cast<unsigned int>(static_cast<unsigned char>(item.m_field0)),
                  static_cast<unsigned int>(item.m_field1),
                  item.m_addInfo,
                  static_cast<unsigned int>(item.GetItemAttr()) & 0xff,
                  item.m_addInfo2,
                  static_cast<unsigned int>(item.m_fieldb),
                  item.m_fieldd,
                  *reinterpret_cast<const unsigned int*>(&item.m_amp),
                  invenType, slotNo, errorLine);
}