// ---- ORIG 0x84a643a JobKey::JobKey(ENUM_CHARACTERJOB, unsigned char) -------
JobKey::JobKey(ENUM_CHARACTERJOB job, unsigned char sub)
{
    // [b28 P211 AE] 字段 enum/schar 化后本体与 ORIG 0x84a643a 13 条逐字同形
    // （探针 /tmp/l2_p2.cpp JK 实证）：store1 this→value→movl；store2 RHS 先
    // movzbl→this 重载→movb。旧 B24D N 判词作废。
    m_job = job;
    m_sub = sub;
}