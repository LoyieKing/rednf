// df_game_r 还原 —— class-C IP "网络前缀 / 末段" 拆分全局函数
// （objdump ORIG T @0x086b1dd5、@0x086b1e85 转写）。
#pragma once

// ORIG _Z22Divide_C_ClassIP_EndIPPcS_S_ @0x086b1dd5：
//   src=="a.b.c.d"，以最后一个 '.' 为界拆分；
//   前缀（C-Class 段，≤12 字节）→ out；末段（≤4 字节）→ end。
//   任一前置条件不满足则整体静默返回（无输出写入）。
//   末段长度为 strlen(dot+1)，不含 NUL，且必须 ≤4。
void Divide_C_ClassIP_EndIP(char* out, char* end, char* src);

// ORIG _Z22Divide_C_ClassIP_EndIPPcS_S_S_ @0x086b1e85：
//   同上 + 状态码出参 err（逐次写单字节状态）：
//   1=src 为空；2=找不到 '.'；3=前缀>12；4=末段>4；
//   5=out 为空；6=end 为空；全部条件通过且两输出均非空时不写 err。
void Divide_C_ClassIP_EndIP(char* out, char* end, char* src, char* err);
