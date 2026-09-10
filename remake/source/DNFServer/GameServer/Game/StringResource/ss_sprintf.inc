int ss_sprintf(char (&buffer)[N], const char* format, ...);

// [b28d-solo-StringResource AE(源码 66=66 逐条全等): 唯一残差 = compare_tu 的 TLS_LE_LEA_RE
// 只归一「源/目标同寄存器」形，ORIG `lea -0x3c048(%eax),%ebx`（eax→ebx）未归一而 OURS
// `lea <A>(%eax),%ebx` 已归一 ⇒ 假 NEAR；已上报 Main 放宽该正则 | ORIG 0x08ac35f8]
