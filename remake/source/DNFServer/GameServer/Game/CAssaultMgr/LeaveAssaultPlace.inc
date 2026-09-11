void CAssaultPlace::LeaveAssaultPlace(CUser* user, bool flag)
{
    // ORIG T 0x82eb450：入口 `mov %al,-0xc; movzbl -0xc,%ebx` 把 bool 形参
    // 截断+零扩展入 callee-saved %ebx，idx 直落 %eax，传参 `mov %ebx,0x8(%esp)`
    // 无二次物化。4.4 对 bool 值传 bool 形参恒发 `mov %ebx,%edx; movzbl %dl`
    // （P33/P73 物化，多 2 条）；P78 禁局部 register asm。
    LeaveAssaultPlace(_GetUserIndex(user, true), flag, false);
}