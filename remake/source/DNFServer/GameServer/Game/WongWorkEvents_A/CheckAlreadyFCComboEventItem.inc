int EventFCCombo::CheckAlreadyFCComboEventItem(unsigned int id, int itemNo)
{
    MySQL* db = GetDBHandle();
    db->set_query("seLect item_check from event_combo_entry where id=%s", NumberToString(id, 0));
    if (!db->exec(true))
        return 0;

    int check = 0;
    // [b28d-solo-WongWorkEvents_A] P226 条件向：ORIG 0x083fb8fd 起
    // `xor $1;test %al;jne L1` + `test check;jne L2` + `L1:mov $1;jmp L3` +
    // `L2:mov $0` + `L3:test %al;je L4` + `mov $0;jmp` + `L4:mov $1` 为
    // 4.1 短路 bool 物化岛；4.4 对裸 `!call()||…` 折直 test/je。裸语句
    // 表达式（无具名变量）令 4.4 保留该物化岛且零槽。
    if (({ !db->get_int(0, check) || check == 0; }))
        return 0;
    return 1;
