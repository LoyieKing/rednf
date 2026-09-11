// [b28d-47 / b28c-R26 复验] verdict=DIFF(routed) [N93附 + P168a③] ORIG 0x084231b6
//   orig/ours=74/75；p167 调用点差集 CLEAR（ORIG calls=12 / OUR calls=12，无 MISSING/EXTRA）。
//   残差两处均为跨版本 codegen、无源形解：
//   ① ORIG 尾 0x084232db `test %al,%al`（紧随 `xor $0x1,%eax`）是 4.1 对
//      `return !exec()` 的 bool 归一落空指令（dead test，无分支消费者）；
//      P168a③ 总闸实测 4.4 恒以 `movzbl %al,%eax` 加宽顶掉 dead test。
//   ② ORIG 的 `mov $0,%eax`(0x0842328b) 同时服务条件汇聚 r=false 与 `return false`
//      （4.1 尾常量共享，函数只有一处 mov $0）；4.4 恒在函数尾另发 `mov $0,%eax`
//      并使 insert 块尾多发 `jmp` 中继（N93附 尾块发射序）。
//   已试形（本轮复验）：① if/else 具名 else{return false;}（四数不变，仍 DIFF）；
//   ② 早返回正向形 / `if(exec) return false;`（判词记录 79 insn 反劣，mov$0/mov$1 双岛）。
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
    // [P1T4 #284-303 试验②：反极性等价重写 `if(A&&B) return false; …; return !E;`
    // 实测反劣 —— 判断段由「call exec; test; jne/jne」变「call; test %al,%al; je」，
    // ORIG 0x084232db `xor $0x1,%eax`（`!exec` 归一）被 4.4 挪到调用点后，与
    // 0x0842328b 的汇聚 `mov $0,%eax` 不再同形，ndiff 由 3 升到 ≥4；已回滚。]
    if (!(grade->exec(true) && grade->getAffectedRowCount() != 0))
    {
        grade->set_query(
            "inSert into member_security_grade(m_id, goblin_fail_cnt) values(%s, %d)",
            NumberToString(mid, 0), failCnt);
        return !grade->exec(true);
    }
    return false;
}