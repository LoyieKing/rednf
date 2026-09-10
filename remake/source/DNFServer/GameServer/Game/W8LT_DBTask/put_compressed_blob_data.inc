bool put_compressed_blob_data(MySQL* db, int fieldIdx, char* src, int srcLen,
                              char** outStr);

// +0x000 实为 charac_no（CUser.cpp:5738 出站包填 getCurCharacNo；本类 SaveSkill
// 用作 SQL where 键）。SIG_LOAD_SKILL 该段声明为 m_pad0[4]，此处视图具名。
