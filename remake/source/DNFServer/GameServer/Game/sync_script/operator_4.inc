void CStringMaker::operator()(LevelUpSkill v)         // 0x08626f7a job==0xb→99
{
    // [b28d-solo-sync_script] 判词：NEAR 封顶（P75 跨版本 vararg 装载时刻）。
    // ORIG 0x08626f7a：76=76，唯一残差 = sprintf 前两个 vararg 先装 %ecx/%edx、
    // job 后读 %eax，再统一压栈；4.4 恒边求边压（cast/裸形实测同形，ndiff 恒 5，
    // 全为标签+装载序）。语义等价。
    // b28d-35：同 clEf——`={0}` 复现 4.1 内联三段清零（P214③/P253）。
    char num[0xff] = {0};
    // b28d-35：ORIG 0x08626f7a 为 then-first if/else（`cmp $0xb; jne <else>;
    //   movl $0x63,-0xc; jmp` 后 else `mov 0xc(%ebp),%eax; mov %eax,-0xc`）；
    //   三目形产镜像块序（`je` + 常量块在后）⇒ 改 if/else。
    int job;
    if (v.m_job == 0xb)
        job = 0x63;
    else
        job = v.m_job;
    sprintf(num, "%d,%d,%d", job, v.m_field4, v.m_field8);
    if (!m_str.empty())
    {
        m_str += ";";
    }
    m_str += num;
