// [b28c-34] ORIG T 0x0808d074：type<0xc 且 (1<<type & 0xac4)!=0（位 2/6/7/9/11）。
// 推断 ORIG 原码为 switch 位掩码族：4.4 对 if+短路形把 (1u<<type)&0xac4 前端折叠成
// (0xac4>>type)&1（N6 位折叠：shr/and $0x1），且无符号比较先载寄存器（P209），
// 共 21 条；改写 switch(type){case 2,6,7,9,11:return 1;} 后 4.4 对 switch 生成
// 范围检查+位测试，逐条复现 ORIG 的 `cmpl $0xb,0x8(%ebp); ja` +
// `mov $0x1,%edx; mov %edx,%ebx; mov %eax,%ecx; shl %cl,%ebx; and $0xac4`
// 全形（探针 /tmp/sdc_probe1.cpp d1_switch，20 条全等）。
int isSyncActiveStatus(int type)
{
    switch (type)
    {
    case 2:
    case 6:
    case 7:
    case 9:
    case 11:
        return 1;
    }
    return 0;
}