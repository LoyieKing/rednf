bool get_compressed_blob_data(MySQL* db, int col, char* out, int outCap,
                              int recordSize);

// ORIG T 0x0844159a（LoadMonsterInfo.md 全文）：GetDBHandle(DB_HANDLE_2,0)；
// charac_kill_monster_info 按 charac_no 查 3 个 blob 列（boss/named/apc_boss）：
// select exec 失败/insert exec 失败/fetch 失败分别 cMyTrace 返 0；无行则
// inSert 新行返 1；各列 get_binary_length 判空，非空时 get_compressed_blob_data
// 解压至 sig 载荷 +0x000/0x1d4c/0x927c（0x1d4c/0x7530/0x1d4c），失败 trace 返 0。
// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 0844159a
// [L4-r1] 6 处 cMyTrace 改 P227 全表达式临时链 ⇒ ndiff 275→116（余 idx/col 槽序
//   + ok 的 P292 归一化，属 N18a/N82 族封顶）。
