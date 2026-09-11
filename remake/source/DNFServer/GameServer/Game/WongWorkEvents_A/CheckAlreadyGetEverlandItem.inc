// ----------------------------------------------------------------------------
// EventEverland::CheckAlreadyGetEverlandItem —— ORIG T 0x083fac0d (size 0xd5)
// [b28d-solo-WongWorkEvents_A] AE：P286 单 case switch（GetItemType 直比
// `cmp $1,%eax; jne`）+ P226 语句表达式（尾段短路物化岛）。
// ----------------------------------------------------------------------------
int EventEverland::CheckAlreadyGetEverlandItem(unsigned int id, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    // [b28d-solo-WongWorkEvents_A] P286：ORIG 0x083fac46 `cmp $1,%eax; jne`
    // 直比无 bool 物化；4.4 对 `if (f()==1)` 恒 `cmp $1; sete %al; test; je`
    // （+2 条）。单 case switch 直比即复现 ORIG（同 UpdateEventEverlandInfo）。
    switch (CWebEvent::GetItemType(4, itemNo))
    {
    case 1:
        db->set_query("seLect coin_check from event_everland_info where id=%s", NumberToString(id, 0));
        break;
    default:
        break;
    }

    if (!db->exec(true))
        return 0;

    int check = 0;
    // [b28d-solo-WongWorkEvents_A] P226 条件向：ORIG 0x083facb7 `xor $1;
    // test;jne L1` + `test check;jne L2` + `L1:mov $1` / `L2:mov $0` +
    // `test %al;je L4` + `mov $0` / `L4:mov $1` 为 4.1 短路 bool 物化岛；
    // 4.4 对裸 `!call()||…` 折直 test/je 并省尾岛。裸语句表达式（无具名
    // 变量）令 4.4 保留物化岛且零槽（P226 条件向，FCCombo/Sigong 同形）。
    if (({ !db->get_int(0, check) || check == 0; }))
        return 0;
    return 1;
}