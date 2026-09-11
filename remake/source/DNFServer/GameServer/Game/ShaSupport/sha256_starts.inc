// ---------------------------------------------------------------------------
// sha256_starts(sha256_context*) T @0x080841fc
// ORIG：ctx+0x00/+0x04 清零；state[8] 按 IV 0x6a09e667/0xbb67ae85/0x3c6ef372/
//       0xa54ff53a/0x510e527f/0x9b05688c/0x1f83d9ab/0x5be0cd19 写 +0x08..+0x24。
// [b28d-L1] 修法：ORIG 0x080841fc..0x08084263 为**逐条展开**的 10 条独立赋值
// （每条 `mov 0x8(%ebp),%eax` 重载 + store，无循环/无 iv 表）；原循环形使 4.4
// 发 sub/setle/回边（27 条 vs ORIG 24 条 DIFF）。展开为逐字段赋值后逐条对齐
// （P168 表达式直读：每处重载 this，不缓存 ctx 指针）。
// ---------------------------------------------------------------------------
void sha256_starts(sha256_context* ctx)
{
    ctx->m_total[0] = 0;
    ctx->m_total[1] = 0;
    ctx->m_state[0] = 0x6a09e667UL;
    ctx->m_state[1] = 0xbb67ae85UL;
    ctx->m_state[2] = 0x3c6ef372UL;
    ctx->m_state[3] = 0xa54ff53aUL;
    ctx->m_state[4] = 0x510e527fUL;
    ctx->m_state[5] = 0x9b05688cUL;
    ctx->m_state[6] = 0x1f83d9abUL;
    ctx->m_state[7] = 0x5be0cd19UL;
}