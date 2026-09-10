bool DB_MouseRegister::_updateFailCnt(unsigned int mid, int failCnt, char* ip)
{
    (void)ip;   // ORIG 形参未被函数体消费（Ghidra 同）。
    MySQL* db = _getDBHandleForDevideAccountDB();
    db->set_query("upDate member_mousepass set fail_cnt=%d where m_id=%s",
                  failCnt, NumberToString(mid, 0));
    db->exec(true);
    MySQL* grade = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_1, static_cast<ENUM_SERVER_GROUP>(0));
    grade->set_query(
        "upDate member_security_grade set goblin_fail_cnt=%d where m_id=%s",
        failCnt, NumberToString(mid, 0));
    // 实测三形后取双 return（bool 返回）：判定段 xor/test/jne+or/test/jne+
    // mov$1/mov$0 与 ORIG 全等；汇聚 bool 局部形 4.4 必落 -0x9 槽（4 条差）
    // 反劣。残差仅尾部 jmp+mov$0（4.4 双 return 分离块 vs ORIG 汇聚共同
    // leave，跨版本块排布 N 族）。[b28 BM8 复验] P222 两形互试：
    // `if(exec) return false; return true;` 反劣 79 insn（mov$0/mov$1 双岛），
    // 维持 `return !exec()`（75，ndiff 3 封顶，N93附 族）。
    if (!(grade->exec(true) && grade->getAffectedRowCount() != 0))
    {
        grade->set_query(
            "inSert into member_security_grade(m_id, goblin_fail_cnt) values(%s, %d)",
            NumberToString(mid, 0), failCnt);
        return !grade->exec(true);
    }
    return false;
