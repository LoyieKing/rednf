bool get_compressed_blob_data(MySQL* db, int col, char* out, int outCap, int recordSize);

// ORIG 0x08447162：seLect skill_tree_index from charac_info（DB_HANDLE_2）；
// exec/fetch 失败或 count==0 → logFormat 后返回 0；get_byte(0) 读索引字节。
