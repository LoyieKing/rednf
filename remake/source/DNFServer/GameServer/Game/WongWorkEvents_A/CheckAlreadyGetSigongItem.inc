int EventSigong::CheckAlreadyGetSigongItem(unsigned int id, int itemNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    // [b28d-solo-WongWorkEvents_A] P251/P279：ORIG 0x083fa6d5 无槽 cmp 链
    // （`call; cmp $1,%eax; je; cmp $2,%eax; je; jmp`）⇒ 原码是
    // `switch (GetItemType(...))` 直传，不是具名 int + if/else 链
    // （具名局部令 4.4 产 `mov %eax,-N; cmpl $K,-N` 且整帧槽号漂移）。
    switch (CWebEvent::GetItemType(3, itemNo))
    {
        case 1:
            db->set_query("seLect item1_check from event_sigong_info where id=%s",
                          NumberToString(id, 0));
            break;
        case 2:
            db->set_query("seLect item2_check from event_sigong_info where id=%s",
                          NumberToString(id, 0));
            break;
    }

    if (!db->exec(true))
        return 0;

    int check = 0;
    // [b28d-solo-WongWorkEvents_A] 真语义 bug 修正 + P226 条件向：
    // ORIG 0x083fa779 `xor $1;test;jne L1`（get_int 失败→L1）+
    // `test check;jne L2`（check!=0→L2）+ `L1:mov $1` / `L2:mov $0` +
    // `test %al;je L4` + `mov $0` / `L4:mov $1` ⇒ 语义 =
    // (get_int() && check!=0) ? 1 : 0；旧写 `if(!get_int) return 1;` 极性反
    // （get_int 失败误返 1）。裸语句表达式复现 4.1 短路物化岛（4.4 对裸
    // `!call()||…` 折直 test/je 且省尾岛）。
    if (({ !db->get_int(0, check) || check == 0; }))
        return 0;
    return 1;
