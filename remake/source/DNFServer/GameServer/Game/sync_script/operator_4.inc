void CStringMaker::operator()(LevelUpSkill v)         // 0x08626f7a job==0xb→99
{
    // [b28d-solo-sync_script] 判词：NEAR 封顶（P75 跨版本 vararg 装载时刻）。
    // ORIG 0x08626f7a：76=76，唯一残差 = sprintf 前两个 vararg 先装 %ecx/%edx、
    // job 后读 %eax，再统一压栈；4.4 恒边求边压（cast/裸形实测同形，ndiff 恒 5，
    // 全为标签+装载序）。语义等价。
    // [S3/M3FreeNext15B 2026-09-11] **报告缺失**（report=NO）⇒ §3.2 兜底：
    //   `nm -S --defined-only` 实测 ORIG @0x08626f7a size 0xec（236B）
    //   + `objdump -d --start-address=0x08626f7a --stop-address=0x08627066`
    //   全 76 条逐条对齐（本波）。p167 调用点差集 CLEAR（4=4：empty /
    //   operator+=(", '") / operator+=(num) / sprintf）。§6.5 串审计 CLEAR
    //   （2 kinds/2 refs：`%d,%d,%d`@0x8ce2dee、";"@0x8ce2df7，逐字节相等）。
    //   ⇒ 维持 P75 登记，不再试形。
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
}