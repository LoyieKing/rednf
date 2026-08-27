#ifndef DNF_GAME_SHASUPPORT_H_
#define DNF_GAME_SHASUPPORT_H_

// ============================================================================
// df_game_r 还原 —— CodeHack 校验域加密支撑（加密域 producer，全局命名空间）
//
// 符号证据（nm / objdump df_game_r）：
//   _Z13sha256_startsP14sha256_context     T @0x080841fc（size 0x68）
//   _Z14sha256_processP14sha256_contextPh  T @0x08084264（size 0x3688）
//   _Z13sha256_updateP14sha256_contextPhm  T @0x080878ec（size 0xfe）
//   _Z13sha256_finishP14sha256_contextPh   T @0x080879ea（size 0x27c）
//   _Z13StrCpyMaxPathPcPKc                 T @0x0808a5dd（size 0x3d）
//
// sha256_context 布局由 starts/update/finish 反汇编实证，sizeof == 0x68：
//   +0x00 total[2]（低/高已处理字节数）/+0x08 state[8]/+0x28 buffer[0x40]。
// 引用方 CodeHackChecksum.cpp（ZeroMemoryChecksum/CalculateMemoryChecksum/
// CalculateFileChecksum/SetFileName 等）统一经本头取得权威定义。
// ============================================================================

struct sha256_context
{
    unsigned long m_total[2];          // +0x00 已处理总字节数（低32位/高32位）
    unsigned long m_state[8];          // +0x08 SHA-256 链值寄存器（big-endian 语义）
    unsigned char m_buffer[0x40];      // +0x28 未满分组缓冲
};

void sha256_starts(sha256_context* ctx);                               // @0x080841fc
void sha256_process(sha256_context* ctx, unsigned char* data);         // @0x08084264（内部压缩函数）
void sha256_update(sha256_context* ctx, unsigned char* input,
                   unsigned long ilen);                                // @0x080878ec
void sha256_finish(sha256_context* ctx, unsigned char output[0x20]);   // @0x080879ea

void StrCpyMaxPath(char* dest, const char* src);                       // @0x0808a5dd

#endif  // DNF_GAME_SHASUPPORT_H_
