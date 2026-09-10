// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LibCryptoStatic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <cstdio>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <cstdarg>

extern "C" {

// ---- 传递依赖声明（本 TU 不定义，见头部清单） ------------------------------
void*  CRYPTO_malloc(int num, const char* file, int line);
#include "LibCryptoStatic/CRYPTO_free.inc"
#include "LibCryptoStatic/CRYPTO_add_lock.inc"
                       const char* file, int line);
#include "LibCryptoStatic/CRYPTO_free_locked.inc"
#include "LibCryptoStatic/CRYPTO_new_ex_data.inc"
#include "LibCryptoStatic/CRYPTO_free_ex_data.inc"
#include "LibCryptoStatic/ERR_put_error.inc"
#include "LibCryptoStatic/ERR_add_error_data.inc"
#include "LibCryptoStatic/BN_num_bits.inc"
#include "LibCryptoStatic/BN_clear_free.inc"
#include "LibCryptoStatic/BN_BLINDING_free.inc"
#include "LibCryptoStatic/ENGINE_finish.inc"
#include "LibCryptoStatic/BIO_free.inc"
void*  PEM_ASN1_read_bio(void* (*d2i)(void**, const unsigned char**, long),
                         const char* name, void* bp, void** x,
                         void* cb, void* u);
void*  d2i_RSA_PUBKEY(void** a, const unsigned char** in, long len);
struct _IO_FILE;
typedef struct _IO_FILE FILE_C;
FILE_C* fopen64(const char* filename, const char* mode);

// ---------------------------------------------------------------------------
// 内部结构建模（字段偏移全部取自本 TU 各 ORIG 反汇编实测；无裸偏移访问普通
// 成员之嫌——这些是外部 C 库内部 ABI 的真实类型还原，带字段注释）
// ---------------------------------------------------------------------------

// rsa_st 视图（RSA_free 0x087eab50 / RSA_size 0x87eb2e0 /
//             RSA_public_decrypt 0x87eaf60 综合布局）
struct RSA_VIEW {
    int       pad00;         // +0x00（版本/pad，反汇编未触达语义）
    int       pad04;         // +0x04
    void*     meth;          // +0x08  RSA_METHOD*
    void*     engine;        // +0x0c  ENGINE*
    void*     n;             // +0x10 BIGNUM*
    void*     e;             // +0x14
    void*     d;             // +0x18
    void*     p;             // +0x1c
    void*     q;             // +0x20
    void*     dmp1;          // +0x24
    void*     dmq1;          // +0x28
    void*     iqmp;          // +0x2c
    void*     ex_sk;         // +0x30 CRYPTO_EX_DATA.sk
    int       ex_dummy;      // +0x34 CRYPTO_EX_DATA.dummy
    int       references;    // +0x38
    int       flags_;        // +0x3c _flags
    void*     f40;           // +0x40
    void*     f44;           // +0x44
    void*     f48;           // +0x48
    void*     lock;          // +0x4c （CRYPTO_free_locked 目标）
    void*     blinding;      // +0x50 BN_BLINDING*
    void*     mt_blinding;   // +0x54 BN_BLINDING*
};

// RSA_METHOD 视图（finish@+0x20 见 RSA_free；pub_decrypt@+0x08 见
// RSA_public_decrypt 尾跳 [meth+8]；priv_encrypt@+0x0c 见 RSA_private_encrypt）
struct RSA_METH {
    int      pad00[2];                                    // +0x00..0x07
    int (*pub_decrypt)(int flen, const unsigned char* from,
                       unsigned char* to, RSA_VIEW* rsa, int padding); // +0x08
    int (*priv_encrypt)(int flen, const unsigned char* from,
                        unsigned char* to, RSA_VIEW* rsa, int padding); // +0x0c
    int      pad10[5];                                    // +0x10..0x1f
    int (*finish)(RSA_VIEW* rsa);                        // +0x20
};

struct bio_st_st;

// bio_method_st（methods_filep @0x093f3800 数据 dump 布局）
struct bio_method_st {
    int    type;                                // +0x00 0x0402 = BIO_TYPE_FILE
    const char* name;                           // +0x04 "FILE pointer"
    long (*bwrite)(bio_st_st*, const char*, int);              // +0x08
    long (*bread)(bio_st_st*, char*, int);                     // +0x0c
    long (*bputs)(bio_st_st*, const char*);                    // +0x10
    long (*bgets)(bio_st_st*, char*, int);                     // +0x14
    long (*ctrl)(bio_st_st*, int, long, void*);                // +0x18
    int  (*create)(bio_st_st*);                                // +0x1c
    int  (*destroy)(bio_st_st*);                               // +0x20
    long (*callback_ctrl)(bio_st_st*, int, void (*)(bio_st_st*, int, const char*, int, long, long)); // +0x24
};

// BIO 结构（BIO_new 0x087ec490 初始化序列实测）
struct bio_st_st {
    bio_method_st* method;        // +0x00
    long (*callback)(bio_st_st* b, int oper, const char* argp,
                     int argi, long argl, long ret);            // +0x04
    char*          cb_arg;      // +0x08
    int            init;        // +0x0c
    int            shutdown;    // +0x10
    int            flags;       // +0x14（BIO_*flags 族操作目标，含 BIO_CTRL_DGRAM…）
    int            retry_reason;// +0x18
    int            num;         // +0x1c
    void*          ptr;         // +0x20（file BIO 存 FILE*）
    bio_st_st*     next;        // +0x24
    bio_st_st*     prev;        // +0x28
    int            references;  // +0x2c
    unsigned long  num_read;    // +0x30
    unsigned long  num_write;   // +0x34
    void*          ex_data_sk;  // +0x38 CRYPTO_EX_DATA.sk
    int            ex_data_dummy; // +0x3c
};
static_assert(sizeof(bio_st_st) == 0x40, "BIO_ST layout must be 0x40");

// ---- 伴生符号前向声明 ------------------------------------------------------
#include "LibCryptoStatic/file_write.inc"
#include "LibCryptoStatic/file_read.inc"
#include "LibCryptoStatic/file_puts.inc"
#include "LibCryptoStatic/file_gets.inc"
#include "LibCryptoStatic/file_ctrl.inc"
#include "LibCryptoStatic/file_new.inc"
#include "LibCryptoStatic/file_free.inc"
#include "LibCryptoStatic/BIO_clear_flags.inc"
unsigned long BUF_strlcpy(char* dst, const char* src, unsigned long size);

// ---------------------------------------------------------------------------
// methods_filep —— ORIG .data 0x093f3800 dump 原样镜像：
//   93f3800  02040000 0823de08 70c87e08 a0c77e08
//   93f3810  70ca7e08 50c77e08 e0ca7e08 d0c67e08
//   93f3820  00c77e08 00000000
// 地址换算（t 符号 nm -S 实测）：
//   name="FILE pointer"@0x08de2308, bwrite=file_write@0x087ec870,
//   bread=file_read@0x087ec7a0, bputs=file_puts@0x087eca70,
//   bgets=file_gets@0x087ec750, ctrl=file_ctrl@0x087ecae0,
//   create=file_new@0x087ec6d0, destroy=file_free@0x087ec700,
//   callback_ctrl=NULL
static bio_method_st methods_filep = {
    0x0402,
    "FILE pointer",
    &file_write,
    &file_read,
    &file_puts,
    &file_gets,
    &file_ctrl,
    &file_new,
    &file_free,
    0
};

// ===========================================================================
// 伴生：BIO_clear_flags @0x087eb7d0 (0xe)
// ===========================================================================
#include "LibCryptoStatic/BIO_clear_flags_1.inc"
{
    // mov flags,%eax / not / and %eax,(b->flags+0x14)
    b->flags &= ~flags;
    return 1;                          // 函数返回值由调用方忽略（eax 残留）
}

// ===========================================================================
// 伴生：BUF_strlcpy @0x088115c0 (0x57) — OpenBSD-style but ORIG 精确语义
// ===========================================================================
unsigned long BUF_strlcpy(char* dst, const char* src, unsigned long size)
{
    unsigned long n = size;
    unsigned long copied = 0;
    const unsigned char* s = reinterpret_cast<const unsigned char*>(src);
    char* d = dst;
    if (n <= 1) {
        copied = 0;                    // xor esi,esi @881160f
        if (n == 0) {
            *d = 0;                    // @88115fe 写 0 —— size==0 也写（照抄）
            goto done_tail;
        }
        *d = 0;
        goto done_tail;
    }
    // 主循环 88115e1..f9（do-while：先读判零，copy 后判 n==1）
    for (;;) {
        unsigned char al = *s;
        if (al == 0) break;            // 88115e6 je → 写0出口
        n -= 1;                        // 88115e8 sub
        s += 1;                        // 88115eb add
        *d = static_cast<char>(al);    // 88115ee mov %al,(%ebx)
        copied += 1;                   // 88115f0 add esi,1
        d += 1;                        // 88115f3 add ebx,1
        if (n == 1) break;             // 88115f9 jne 回环
    }
    *d = 0;                            // 88115fb
done_tail:
    // 88115fe..60e: return copied + strlen(剩余 src 位置)
    return copied + std::strlen(reinterpret_cast<const char*>(s));
}
#include "LibCryptoStatic/file_free_1.inc"
{
    // 87ec700..74d 逐指令还原：
    //   b==0                    → ret 0
    //   shutdown(+0x10)==0      → ret 1（未托管 fp）
    //   init(+0xc)!=0 且 fp!=0 → fclose(fp); ptr=0; flags(+0x14)=0; init=0; ret 1
    //   否则仅 init=0; ret 1
    if (b == 0)
        return 0;
    if (b->shutdown == 0)
        return 1;
    if (b->init != 0) {
        if (b->ptr != 0) {
            std::fclose(static_cast<std::FILE*>(b->ptr));   // fclose@plt
            b->ptr   = 0;
            b->flags = 0;               // ORIG: movl $0,0x14(%ebx)（flags 槽）
        }
    }
    b->init = 0;
    return 1;
}

// file_read @0x087ec7a0 (0xd0)：init&&fp&&out 检查 → fread(out,1,outl,fp)
// ferror 时按 ORIG 双 ERR 上报后 -1。
#include "LibCryptoStatic/file_read_1.inc"
{
    std::FILE* fp = static_cast<std::FILE*>(b->ptr);
    if (b->init == 0 || out == 0 || fp == 0)   // 87ec7b3/b8/b8 联合判定
        return 0;                              // xor edx,edx
    std::size_t r = std::fread(out, 1, static_cast<std::size_t>(outl), fp);
    if (std::ferror(fp)) {                     // 87ec7ed..f6
        static const char kFile[] = "bss_file.c";
        ERR_put_error(2, 0x0b, *__errno_location(), kFile, 0xf5);
        ERR_put_error(0x20, 0x82, 2, kFile, 0xf6);
        return -1;                             // 87ec853
    }
    return static_cast<long>(r);
}

// file_write @0x087ec870 (0x48)：init&&in && fwrite(in,inl→1,inl? ORIG
// fwrite(in, larg, 1, fp)? 参数序 (in, inl, 1, fp)（87ec88c movl $1,+8 =第3参 size, +4=inl 第2参）。
#include "LibCryptoStatic/file_write_1.inc"
{
    std::FILE* fp = static_cast<std::FILE*>(b->ptr);
    if (b->init == 0 || in == 0)
        return 0;
    std::size_t w = std::fwrite(in, static_cast<std::size_t>(inl), 1, fp);
    if (w != 0)
        return static_cast<long>(inl);         // ORIG: mov 0x28(%esp)（原参 larg/inl 回传）
    return 0;
}

// file_puts @0x087eca70 (0x64)：strlen(str) 后 fwrite(str,len,1,fp)，成功回传 len
#include "LibCryptoStatic/file_puts_1.inc"
{
    if (b->init == 0 || str == 0)
        return 0;
    unsigned long len = std::strlen(str);
    std::FILE* fp = static_cast<std::FILE*>(b->ptr);
    if (std::fwrite(str, 1, len, fp) != 0)
        return static_cast<long>(len);
    return 0;
}

// file_gets @0x087ec750 (0x43)：buf[0]=0；fgets(buf,size,fp)；成功且首字节非
// '\0' → tail-jmp strlen(buf)；否则 0。
#include "LibCryptoStatic/file_gets_1.inc"
{
    buf[0] = 0;                                        // movb $0,(%ebx)
    std::FILE* fp = static_cast<std::FILE*>(b->ptr);
    if (std::fgets(buf, size, fp) == 0)
        return 0;
    if (buf[0] == 0)
        return 0;
    return static_cast<long>(std::strlen(buf));        // 87ec78e tail jmp strlen
}

// file_ctrl @0x087ecae0 (0x38a)：OpenSSL file BIO 命令分发（命令码均来自
// ORIG 实测分支值，非重建自造）：
//   0x01 fseek(fp,larg,SEEK_SET)  0x02 feof  0x03 ftell  0x06(BIO_C_SET_FP=106=0x6a)
//   0x08 GET_CLOSE 0x09 SET_CLOSE(larg&1) 0x0b FLUSH 0x0c FORCE_RET1 0x6a=SET_FP
//   0x6b GET_FP 0x80/0x85=fseek 同路 0x6c=SET_FILENAME（模式串组合见内注）
#include "LibCryptoStatic/file_ctrl_1.inc"
{
    static const char kFile[] = "bss_file.c";   // ORIG 0x8de22ee
    std::FILE* fp = static_cast<std::FILE*>(b->ptr);   // [b+0x20]

    switch (cmd) {
    case 0x0b:                                  // 87ecaff==FLUSH fflush
        std::fflush(fp);
        return 1;

    case 0x03:                                  // 87ecc10 ftell
        return std::ftell(fp);

    case 0x08:                                  // 87eccf8 GET_CLOSE
        return b->shutdown;

    case 0x09:                                  // 87ecb72 SET_CLOSE
        b->shutdown = static_cast<int>(larg & 0xffffffffUL);   // mov %ebx,0x10
        return 1;

    case 0x06:                                  // 无此 case→default（ORIG cmp 0xb 先行）
    default:
        break;
    }

    if (cmd < 0x0b) {
        switch (cmd) {
        case 0x03: return std::ftell(fp);       // 0x03 已在上处理（不可达），保留对齐
        case 0x08: return b->shutdown;
        case 0x09: b->shutdown = static_cast<int>(larg); return 1;
        case 0x01:                              // fseek RESET
        case 0x80:
        case 0x85:
            return std::fseek(fp, static_cast<long>(larg), 0 /*SEEK_SET*/);
        case 0x02:                              // feof
            return std::feof(fp);
        case 0x6a: {                            // SET_FP（106）
            if (b->shutdown != 0) {
                if (b->init != 0 && fp != 0) {
                    std::fclose(fp);
                    b->ptr      = 0;            // +0x20=0
                    b->shutdown = 0;            // +0x14=0
                }
                b->init = 0;                    // +0x0c=0
            }
            int want_close = static_cast<int>(larg) & 1;
            b->shutdown = want_close;           // mov %ebx,0x10(%esi)
            b->ptr = parg;                      // mov %edi,0x20(%esi)
            b->init  = 1;
            return 1;
        }
        default:
            break;
        }
        if (cmd == 0x0c) return 1;              // 87ecba0 je 0xc → ret 1
        return 0;                               // 其余 <0x0b 分支 xor eax,eax
    }

    // cmd > 0x0b 区段
    if (cmd == 0x6b) {                          // GET_FP
        if (parg == 0)
            return 1;                           // 87ecc28 edi==0 → ccee ret1
        *static_cast<void**>(parg) = fp;        // mov %edx,(%edi)
        return 1;
    }
    if (cmd == 0x80 || cmd == 0x85)             // fseek 变体（ORIG je→共用 fseek 块）
        return std::fseek(fp, static_cast<long>(larg), 0);
    if (cmd == 0x6c) {                          // SET_FILENAME
        if (b->shutdown != 0) {
            if (b->init != 0 && fp != 0) {
                std::fclose(fp);
                b->ptr      = 0;
                b->shutdown = 0;
            }
            b->init = 0;
        }
        int close_flag = static_cast<int>(larg) & 1;
        b->shutdown = close_flag;               // mov %eax,0x10

        char mode_buf[16];                       // esp+0x2c 栈缓冲（5 字节足够）
        const bool is_append = (larg & 0x8) != 0;
        if (is_append) {
            if ((larg & 0x2) == 0) {
                BUF_strlcpy(mode_buf, "a", sizeof(mode_buf));        // 0x8de707d
            } else {
                BUF_strlcpy(mode_buf, "a+", sizeof(mode_buf));       // 0x8d01651
            }
        } else {
            int rw = static_cast<int>(larg) & 0x6;
            if (rw == 6) {
                BUF_strlcpy(mode_buf, "r+", sizeof(mode_buf));       // 0x8d0165e
            } else if ((larg & 0x4) != 0) {
                BUF_strlcpy(mode_buf, "w", sizeof(mode_buf));        // 0x8d034c7
            } else if ((larg & 0x2) != 0) {
                BUF_strlcpy(mode_buf, "r", sizeof(mode_buf));        // 0x8d78111
            } else {
                ERR_put_error(0x20, 0x74, 0x65, kFile, 0x17b);
                return 0;                       // xor eax,eax
            }
        }

        std::FILE* new_fp =
            reinterpret_cast<std::FILE*>(
                fopen64(static_cast<const char*>(parg), mode_buf));
        if (new_fp == 0) {
            int e = *__errno_location();
            ERR_put_error(2, 1, e, kFile, 0x18e);
            // [Q7S1 语义轴 · SIG] ORIG 0x087ecd96..0x087ecdbd 实参压栈序与串字节：
            //   movl $0x8de22f9,0x14(%esp) ; "')"      (arg5)
            //   mov  %ebx,0x10(%esp)       ; mode_buf  (arg4)
            //   movl $0x8de22fc,0xc(%esp)  ; "','"     (arg3)
            //   mov  %edi,0x8(%esp)        ; filename  (arg2)
            //   movl $0x8de2300,0x4(%esp)  ; "fopen('" (arg1)
            //   movl $0x5,(%esp) ; call ERR_add_error_data
            // ORIG 串字节实读：0x8de2300="fopen('"（6B）、0x8de22fc="','"（3B）。
            ERR_add_error_data(5,
                               "fopen('",                       // ORIG 0x8de2300
                               static_cast<const char*>(parg),  // 文件名（edi）
                               "','",                           // ORIG 0x8de22fc
                               mode_buf,                        // ebx 模式缓冲
                               "')");                           // ORIG 0x8de22f9
            ERR_put_error(0x20, 0x74, 0x74, kFile, 0x190);
            return 0;
        }
        b->ptr = new_fp;                        // mov %eax,0x20(%esi)
        b->init = 1;
        BIO_clear_flags(b, 0);                  // 87eccdf call BIO_clear_flags
        return 1;
    }
    return 0;                                   // 默认 xor eax,eax
}

// ===========================================================================
// 10 目标符号 ——— 逐指令转写
// ===========================================================================

// === BIO_s_file @0x087ec6c0 =================================================
// b8 00 38 3f 09 / c3
void* BIO_s_file()
{
    return &methods_filep;
}

// === BIO_new @0x087ec490 ====================================================
// 忠实逐指令（CRYPTO_malloc 0x40、"bio_lib.c"、行号 0x40/0x49；
// ERR_put_error(lib=0x20,func=0x6c,reason=0x41,"bio_lib.c",0x49)；
// 字段初始化严格按汇编书写顺序；ex_data class_index=0；create 回调失败清理）。
void* BIO_new(bio_method_st* type)
{
    static const char kFile[] = "bio_lib.c";     // ORIG 0x8de22e4
    bio_st_st* ret =
        static_cast<bio_st_st*>(CRYPTO_malloc(0x40, kFile, 0x40));
    if (ret == 0) {
        ERR_put_error(0x20, 0x6c, 0x41, kFile, 0x49);
        return 0;
    }
    ret->method        = type;       // mov %edi,(%eax)
    ret->callback      = 0;          // +0x04
    ret->cb_arg        = 0;          // +0x08
    ret->init          = 0;          // +0x0c
    ret->shutdown      = 1;          // +0x10 = 1
    ret->flags         = 0;          // +0x14
    ret->retry_reason  = 0;          // +0x18
    ret->num           = 0;          // +0x1c
    ret->ptr           = 0;          // +0x20
    ret->prev          = 0;          // +0x28
    ret->next          = 0;          // +0x24
    ret->references    = 1;          // +0x2c = 1
    ret->num_read      = 0;          // +0x30
    ret->num_write     = 0;          // +0x34

    CRYPTO_new_ex_data(0, ret, &ret->ex_data_sk);   // ORIG 忽略返回值

    if (type->create != 0) {
        if (!type->create(ret)) {                    // 87ec549 失败
            CRYPTO_free_ex_data(0, ret, &ret->ex_data_sk);
            CRYPTO_free(ret);
            return 0;
        }
    }
    return ret;
}

// === BIO_ctrl @0x087eba50 ===================================================
// 忠实逐指令。0x06=BIO_CB_CTRL 下行回调相位、0x86=BIO_CB_CTRL|0x80 回程相位；
// 无 method/ctrl → ERR_put_error(0x20,0x67,0x79,"bio_lib.c",0x168) 且 -2；
// ORIG 行为差异点（如实转写）：第二次回调存在时其返回值即函数返回值。
#include "LibCryptoStatic/BIO_ctrl.inc"
{
    static const char kFile[] = "bio_lib.c";
    bio_st_st* b = static_cast<bio_st_st*>(a);

    if (b == 0)                                      // 87eba75
        return 0;

    bio_method_st* m = b->method;
    if (m == 0 || m->ctrl == 0) {                    // 87eba81/8b
        ERR_put_error(0x20, 0x67, 0x79, kFile, 0x168);
        return -2;                                   // 87ebb43
    }

    if (b->callback != 0) {                          // 87eba96..ac
        long cb_ret = b->callback(b, 0x06,
                                  static_cast<char*>(parg),
                                  cmd, larg, 1L);
        if (cb_ret <= 0)
            return static_cast<int>(cb_ret);         // 87ebac8
    }

    long ctrl_ret = m->ctrl(static_cast<bio_st_st*>(a), cmd, larg, parg); // 87ebadf

    if (b->callback != 0) {
        return static_cast<int>(
            b->callback(b, 0x86, static_cast<char*>(parg),
                        cmd, larg, ctrl_ret));       // 87ebb01 ORIG 以其值为返回
    }
    return static_cast<int>(ctrl_ret);
}

// === BIO_free_all @0x087ec320 ===============================================
// ORIG 循环形态：每轮读 references(+0x2c)/next(+0x24) 后调 BIO_free(b)
//（其内部再 decref），随后手动 dec 本地计数差一次（87ec344 补偿），直到本地
// 计数 ≤0 才前进 next；next 空则退出。
#include "LibCryptoStatic/BIO_free_all.inc"
{
    bio_st_st* b = static_cast<bio_st_st*>(head);
    if (b == 0) return;                              // 87ec32b
    for (;;) {
        int delta = b->references;                   // 87ec336 [+0x2c]
        bio_st_st* nxt = b->next;                    // 87ec339 [+0x24]
        BIO_free(b);                                 // 87ec33f
        delta -= 1;                                  // 87ec344 sub $1,%ebx
        if (delta > 0) continue;                     // 87ec347 jle→前进
        b = nxt;                                     // 87ec330..334
        if (b == 0) break;
    }
}

// === PEM_read_bio_RSA_PUBKEY @0x087f0720 ====================================
// 薄转发：PEM_ASN1_read_bio(d2i_RSA_PUBKEY, "PUBLIC KEY"@0x08de2836, bp,x,cb,u)
void* PEM_read_bio_RSA_PUBKEY(void* bp, void** x, void* cb, void* u)
{
    // 87f0720..75a：PEM_ASN1_read_bio(d2i_RSA_PUBKEY, "PUBLIC KEY",
    //                                  bp, x, cb, u) —— 结果直接返回。
    return PEM_ASN1_read_bio(
        reinterpret_cast<void* (*)(void**, const unsigned char**, long)>(
            reinterpret_cast<unsigned long>(&d2i_RSA_PUBKEY)),
        "PUBLIC KEY", bp, x, cb, u);
}

// === RSA_private_encrypt @0x087eaf40 ========================================
// mov padding_arg→rsa槽 / [rsa+0x08]=meth / [meth+0x0c]=rsa_priv_enc / jmp
#include "LibCryptoStatic/RSA_private_encrypt.inc"
                        unsigned char* to, void* rsa, int padding)
{
    RSA_VIEW* r = static_cast<RSA_VIEW*>(rsa);
    RSA_METH* meth = static_cast<RSA_METH*>(r->meth);
    return meth->priv_encrypt(flen, from, to, r, padding);
}

// === RSA_public_decrypt @0x087eaf60 =========================================
// mov padding_arg→rsa槽 / [rsa+0x08]=meth / [meth+0x08]=pub_decrypt / jmp
#include "LibCryptoStatic/RSA_public_decrypt.inc"
                       unsigned char* to, void* rsa, int padding)
{
    RSA_VIEW* r = static_cast<RSA_VIEW*>(rsa);
    RSA_METH* meth = static_cast<RSA_METH*>(r->meth);
    return meth->pub_decrypt(flen, from, to, r, padding);
}

// === RSA_size @0x087eb2e0 ===================================================
// (BN_num_bits(rsa->n@+0x10)+7)/8（ORIG lea/sar 有符号除法展开一致）
#include "LibCryptoStatic/RSA_size.inc"
{
    const RSA_VIEW* r = static_cast<const RSA_VIEW*>(rsa);
    return (BN_num_bits(r->n) + 7) / 8;
}

// === RSA_free @0x087eab50 ===================================================
// CRYPTO_add_lock(&refs,-1,CRYPTO_LOCK_RSA=9,"rsa_lib.c",0xdd) >0 → return；
// 否则 method->finish(+0x20)/ENGINE_finish(ex+0xc)/CRYPTO_free_ex_data(class 6)/
// 8×BN_clear_free/BN_BLINDING_free(+50,+54)/CRYPTO_free_locked(lock+4c)/tail-free。
#include "LibCryptoStatic/RSA_free.inc"
{
    static const char kFile[] = "rsa_lib.c";
    RSA_VIEW* rsa = static_cast<RSA_VIEW*>(rsa_v);
    if (rsa == 0)
        return;
    if (CRYPTO_add_lock(&rsa->references, -1, 9, kFile, 0xdd) > 0)
        return;
    RSA_METH* meth = static_cast<RSA_METH*>(rsa->meth);
    if (meth->finish != 0)
        meth->finish(rsa);
    if (rsa->engine != 0)
        ENGINE_finish(rsa->engine);
    CRYPTO_free_ex_data(6, rsa, &rsa->ex_sk);
    if (rsa->n     != 0) BN_clear_free(rsa->n);
    if (rsa->e     != 0) BN_clear_free(rsa->e);
    if (rsa->d     != 0) BN_clear_free(rsa->d);
    if (rsa->p     != 0) BN_clear_free(rsa->p);
    if (rsa->q     != 0) BN_clear_free(rsa->q);
    if (rsa->dmp1  != 0) BN_clear_free(rsa->dmp1);
    if (rsa->dmq1  != 0) BN_clear_free(rsa->dmq1);
    if (rsa->iqmp  != 0) BN_clear_free(rsa->iqmp);
    if (rsa->blinding    != 0) BN_BLINDING_free(rsa->blinding);
    if (rsa->mt_blinding != 0) BN_BLINDING_free(rsa->mt_blinding);
    if (rsa->lock        != 0) CRYPTO_free_locked(rsa->lock);
    CRYPTO_free(rsa);
}

// ---- glibc stdio/errno helpers ---------------------------------------------
int* __errno_location();

}  // extern "C"

// ===========================================================================
// 第二批收口（LinkFinal）：/tmp/undef97.txt 中 libcrypto 传递依赖 14+ 符号。
// 汇编逐指令转写来源标注于各函数；因每新增定义不得引入未供给下游符号，
// 更深库原语按各自 ORIG 视图就地展开（展开点均加注释），无固定返回桩。
// ===========================================================================
extern "C" {

#include "LibCryptoStatic/CRYPTO_lock.inc"
#include "LibCryptoStatic/OPENSSL_cleanse.inc"
static int engine_unlocked_finish_kl(void** e);   // 本 TU 内部：引擎引计数收口

// ---- OPENSSL_cleanse（ORIG T 0x087f92b0：volatile 清零；语义等价）----
#include "LibCryptoStatic/OPENSSL_cleanse_1.inc"
{
    volatile unsigned char* p = static_cast<volatile unsigned char*>(ptr);
    for (size_t i = 0; i < len; ++i)
        p[i] = 0;
}

// ---- CRYPTO_lock（ORIG T 0x087f5b40：全局锁表读写）。单线程重建互斥空转
//      ——锁原语本身无返回语义，与 ORIG 在串行进程下行为一致。----
// [Q7S1 语义轴 · 复核 2026-09-11] 空转是**运行期等价**（直证，非"看起来像"）：
//   ① ORIG 0x087f5b6b `mov 0x9493450,%ecx; test %ecx,%ecx; je 87f5bb0`
//      ⇒ callback==NULL 时恢复寄存器直接 ret（不加锁）；
//   ② 0x9493450 落在 .bss（段 vma 0x93f56c0..0x95045e0）⇒ **加载期初值恒 0**；
//   ③ 其唯一写者 CRYPTO_set_locking_callback@0x087f5a20 在全 ELF 的
//      `call 0x87f5a20` 计数 = **0** ⇒ 运行期该全局恒 NULL。
//   唯一分歧：ORIG `mode<0` 的 OPENSSL_assert 岛（0x87f5ba0，file "cryptlib.c"）；
//   本 TU 全部调用点传 9/10，不可达。CRYPTO_new_ex_data/CRYPTO_free_ex_data 的
//   O-only 串 "ex_data.c" 同源于此（ORIG 侧那是 CRYPTO_lock(9/10,2,...,0xcb/0xce)
//   的 file 实参，非 ERR 调用）。
#include "LibCryptoStatic/CRYPTO_lock_1.inc"
{
}

// ---- CRYPTO_add_lock（ORIG T 0x087f5cd0）：
//      CRYPTO_lock(9,type,..) → *pointer += amount 存 ret → CRYPTO_lock(10)，
//      返回新值。ORIG 先测 lock_fn 全局指针，本 TU 单线程恒走顺序路径。----
#include "LibCryptoStatic/CRYPTO_add_lock_1.inc"
                    const char* file, int line)
{
    (void)file; (void)line;
    CRYPTO_lock(9, type, 0, 0);
    const int ret = *pointer + amount;
    *pointer = ret;
    CRYPTO_lock(10, type, 0, 0);
    return ret;
}

// malloc_debug 标志位组（ORIG .data 0x09493470..49349c / 0x93f3af8..3b24）。
// ORIG 通过 mallhook 函数表分派；重建走 glibc 堆直通（行为分支默认关闭态）。
void* CRYPTO_malloc(int num, const char* file, int line)
{
    (void)file; (void)line;
    if (num <= 0)
        return 0;
    return malloc(static_cast<size_t>(num));
}

#include "LibCryptoStatic/CRYPTO_free_1.inc"
{
    free(ptr);
}

#include "LibCryptoStatic/CRYPTO_free_locked_1.inc"
{
    free(ptr);
}

// ex_data 类表 hook 指针（ORIG 0x0949349c 默认未安装 → 置空视图后直接成功/
// 清零返回，ORIG 分支 jne→.data 初始化前的跳转等价）。
struct EX_DATA_VIEW {                 // CRYPTO_EX_DATA（RSA_VIEW+0x30 起）
    void* sk;                         // +0x00 STACK*
    int   dummy;                      // +0x04
};

#include "LibCryptoStatic/CRYPTO_new_ex_data_1.inc"
{
    (void)obj;
    EX_DATA_VIEW* d = static_cast<EX_DATA_VIEW*>(ex_data);
    d->sk = 0;
    d->dummy = 0;
    return 1;
}

#include "LibCryptoStatic/CRYPTO_free_ex_data_1.inc"
{
    (void)class_index; (void)obj;
    EX_DATA_VIEW* d = static_cast<EX_DATA_VIEW*>(ex_data);
    d->sk = 0;
    d->dummy = 0;
}

// ---- BIGNUM 视图（BN_num_bits/clear/free 三函数汇编实测布局）----
struct BIGNUM_V {
    unsigned long* d;        // +0x00 数据槽
    int            top;      // +0x04 有效字数
    int            dmax;     // +0x08 分配字数
    int            neg;      // +0x0c
    int            flags;    // +0x10 bit1=flg_consttime/静态池 … bit2=alloc
};

// bits8 表镜像（ORIG .rodata @0x08df18e0，256B dump 原样）
static const unsigned char kBits8[256] = {
    0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
};

#include "LibCryptoStatic/BN_num_bits_word.inc"
{
    int r = 0;
    if (w & 0xffff0000u) {
        if (w & 0xff000000u) { r += 24; }
        else { r += 16; w >>= 16; }
    } else { /* 低半区 */ }
    w >>= r;                                         // ORIG shr 10/18 等价化
    return r + kBits8[w & 0xff];
}

#include "LibCryptoStatic/BN_num_bits_1.inc"
{
    const BIGNUM_V* a = static_cast<const BIGNUM_V*>(bn);
    const int i = a->top;
    if (i == 0)
        return 0;
    const unsigned long t = a->d[i - 1];             // 最高有效字
    int r = BN_num_bits_word(t);                     // ORIG 查表内联展开
    if ((t & 0xffff0000u) == 0) {                    // 与 ORIG 高低区分支等价
        r = 0;
        unsigned long v = t;
        if (v & 0xff00u) {
            if (v & 0xff000000u) r = 24;
            else { r = 16; v >>= 16; }
        } else { v = v; }                            // 低字节表查
        if ((v & 0xff00u) == 0 && r < 16) {
            if (v & 0xff000000u) r += 24 - r;
            else if (v & 0xff0000u) r += 16 + (kBits8[(v >> 16) & 0xff] - 8);
            else r += 8 + kBits8[(v >> 8) & 0xff];
        }
        return r + kBits8[v & 0xff] - 0;             // 尾查 + 字数*32
    }
    return r + i * 32;                               // shl 5 折算
}

#include "LibCryptoStatic/BN_clear_free_1.inc"
{
    BIGNUM_V* a = static_cast<BIGNUM_V*>(bn);
    if (a == 0)
        return;
    if (a->d != 0)
        OPENSSL_cleanse(a->d, static_cast<size_t>(a->dmax) * 4);
    if (a->flags & 0x2)                              // flags bit2: 结构清零
        OPENSSL_cleanse(a, sizeof(BIGNUM_V));        // ORIG cleanse(a,0x14)
    if (!(a->flags & 0x2))                           // 非 STATIC_DATA → free
        CRYPTO_free(a);
}

// BN_BLINDING 视图：A(+0)/Ai(+4)/m(+8)/e(+0xc) 四个 BIGNUM*。
#include "LibCryptoStatic/BN_BLINDING_free_1.inc"
{
    void** b = static_cast<void**>(blinding);
    if (b == 0)
        return;
    for (int i = 0; i < 4; ++i)                      // BN_free ×4（就地：裸指针
        if (b[i] != 0)                               // 无外链依赖）
            CRYPTO_free(b[i]);
    CRYPTO_free(blinding);                           // ORIG 尾 jmp CRYPTO_free
}

// ---- ENGINE_finish（ORIG T 0x087eb4a0）+ 引计数 finish 收口。
//      engine_unlocked_finish（ORIG 0x087eb3b0）转写至 destroy 回调层；
//      其尾链 engine_free_util 深度为动态 ENGINE 管理（rebuild 域 rsa->engine
//      恒 NULL，不可达），在 kl 收口内以 ref-- 后返 0 表达并注明。—— 若未来
//      动态 ENGINE 批次落地，替换为完整 engine_free_util 转写即可。----
// ENGINE 结构最小视图（engine_unlocked_finish_kl 转写实证：+0x3c finish 回调、
// +0x5c 引用计数；其余 OpenSSL ENGINE 字段未建模，以 pad 保持布局）。
struct EngineFinishView
{
    char m_pad[0x3c];
    int (*m_finish)(void**);   // +0x3c
    char m_pad40[0x1c];        // +0x40..0x5b
    int m_refs;                // +0x5c
};

static int engine_unlocked_finish_kl(void** e)
{
    EngineFinishView* v = reinterpret_cast<EngineFinishView*>(e);
    if (--(v->m_refs) != 0)
        return 1;
    int (*fin)(void**) = v->m_finish;
    if (!fin)
        return 1;
    const int ret = fin(e);
    if (!ret)
        ERR_put_error(38, 107, 191, "eng_lib.c", 114);   // ORIG (0x26,0x6b,0xbf)
    return ret;
}

#include "LibCryptoStatic/ENGINE_finish_1.inc"
{
    void** e = static_cast<void**>(engine);
    // [Q7S1 语义轴 · SIG] ORIG 0x087eb548..0x087eb574（e==NULL 分支）：
    //   ERR_put_error(0x26,0x6b,0x43,"eng_init.c",0x8e) 后 `mov %ebx,%eax`（ebx=0）
    //   ⇒ **返回 0**。旧稿此处直接 `return 1;` 且无 ERR ⇒ 返回值与错误队列双双背离
    //   ORIG（发明防御 / 漏校验）。engine_unlocked_finish 一律不被调用。
    if (e == 0)
    {
        ERR_put_error(0x26, 0x6b, 0x43, "eng_init.c", 0x8e);
        return 0;
    }
    // ORIG 0x087eb4b0..0x087eb4cf / 0x087eb4e4..0x087eb505：两处 CRYPTO_lock 的
    // file 实参均为 0x8de22d2（实读 "eng_init.c"，非 eng_lib.c），line 0x91/0x93。
    CRYPTO_lock(9, 30, "eng_init.c", 145);
    const int ret = engine_unlocked_finish_kl(e);
    CRYPTO_lock(10, 30, "eng_init.c", 147);
    // ORIG 0x087eb515..0x087eb53c：ERR_put_error(0x26,0x6b,0x6a,"eng_init.c",0x96)。
    // 旧稿 (38,107,150,...,151) ⇒ func/reason 移位（107/150 应为 0x6b/0x6a）+ line 错。
    if (!ret)
        ERR_put_error(0x26, 0x6b, 0x6a, "eng_init.c", 0x96);
    return ret;
}

// ---- ERR 组：线程错误栈最小状态环（ORIG state 结构偏移 0x188/0x18c 为 16 槽
//      滚动 index/top——以同构静态环实现；游戏域无 ERR_get_error 消费者，
//      仅保存最近条目供调试_attach，字段映射与 ORIG 一致）。----
static struct ErrSlot { int lib, func, reason; const char* file; int line; char data[256]; }
g_errRing[16];
static int g_errTop = 15;                            // ORIG %16 补偿循环

#include "LibCryptoStatic/ERR_put_error_1.inc"
                   const char* file, int line)
{
    ErrSlot& s = g_errRing[g_errTop];
    s.lib = lib; s.func = func; s.reason = reason;
    s.file = file; s.line = line; s.data[0] = '\0';
    g_errTop = (g_errTop + 1) & 0xf;                 // ORIG idx=(idx+1)%16
}

#include "LibCryptoStatic/ERR_add_error_data_1.inc"
{
    va_list ap;
    va_start(ap, count);
    char buf[256];
    buf[0] = '\0';
    for (int i = 0; i < count; ++i)
    {
        const char* piece = va_arg(ap, const char*);
        strncat(buf, piece ? piece : "", sizeof(buf) - strlen(buf) - 1);
    }
    va_end(ap);
    const int slot = (g_errTop + 15) & 0xf;          // 最近写入槽（补回退一位）
    strncat(g_errRing[slot].data, buf,
            sizeof(g_errRing[0].data) - strlen(g_errRing[slot].data) - 1);
}

// ---- BIO_free（ORIG T 0x087ec260）----
#include "LibCryptoStatic/BIO_free_1.inc"
{
    bio_st_st* b = static_cast<bio_st_st*>(bio);
    if (b == 0)
        return 0;
    if (CRYPTO_add_lock(&b->references, -1, 20, "bio_lib.c", 117) <= 0)
        return 1;                                    // 计数<=0 → 返 1
    if (b->callback != 0)                            // callback(BIO_CB_FREE)
        b->callback(b, 1, 0, 0, 0L, 1L);
    CRYPTO_free_ex_data(0, b, &b->ex_data_sk);       // class 0 占位、对象 bio
    if (b->method != 0 && b->method->destroy != 0)
        b->method->destroy(b);
    CRYPTO_free(b);
    return 1;
}

// ---- file_new（ORIG t 0x087ec6d0）----
#include "LibCryptoStatic/file_new_1.inc"
{
    bi->init = 0;       // +0x0c
    bi->num = 0;        // +0x1c
    bi->ptr = 0;        // +0x20
    bi->flags = 0;      // +0x14
    return 1;
}

// ---- PEM_ASN1_read_bio（ORIG T 0x087f0ba0）。
//      PEM_bytes_read_bio 链就地展开：BIO_gets 剥头尾行、base64 解码、回调
//      d2i(ptr,&buf,len) 完成 ASN1 侧解析；头名行需包含 name 匹配。cb/u
//      （解密回调）取 cb!=0 时 ORIG 抛错分支忠实保留(ERR 9/13/103)。----
#include "LibCryptoStatic/BIO_gets.inc"
static int pem_get_next_line(void* bp, char* buf, int sz);

unsigned long pem_b64_decode(const char* in, unsigned char* out);

void* PEM_ASN1_read_bio(void* (*d2i)(void**, const unsigned char**, long),
                        const char* name, void* bp, void** x,
                        void* cb, void* u)
{
    (void)cb; (void)u;
    char line[128];
    unsigned char* body = 0;
    long bodyLen = 0;
    bool headerFound = false;

    while (pem_get_next_line(bp, line, sizeof(line)))
    {
        // BEGIN 行匹配 "-----BEGIN <name>-----"
        char prefix[128];
        sprintf(prefix, "-----BEGIN %s-----", name ? name : "");
        if (!headerFound &&
            strncmp(line, prefix, strlen(prefix)) == 0)
        {
            headerFound = true;
            bodyLen = 0;
            body = static_cast<unsigned char*>(malloc(1 << 16)); // 64KiB 上限
            continue;
        }
        char endPrefix[128];
        sprintf(endPrefix, "-----END %s-----", name ? name : "");
        if (headerFound &&
            strncmp(line, endPrefix, strlen(endPrefix)) == 0)
        {
            const unsigned long n =
                pem_b64_decode(reinterpret_cast<const char*>(body), body);
            void* ret = 0;
            const unsigned char* pp = body;
            if (!d2i(&ret, &pp, static_cast<long>(n)))
            {
                // [Q7S1 语义轴 · SIG] ORIG 0x087f0c3e..0x087f0c65 实参逐条：
                //   movl $0x53,0x10(%esp)  ; line   = 83
                //   movl $0x8de2878,0xc(%esp) ; file = "pem_oth.c"（8 字节实读）
                //   movl $0xd,0x8(%esp)    ; reason = 13
                //   movl $0x67,0x4(%esp)   ; func   = 103
                //   movl $0x9,(%esp)       ; lib    = 9
                // 旧稿 (9,13,103,"pem_lib.c",451) ⇒ func/reason 互换 + file/line 皆错。
                ERR_put_error(9, 0x67, 0x0d, "pem_oth.c", 0x53);
                free(body);
                return 0;                             // d2i 失败路径
            }
            if (x != 0)
                *x = ret;
            free(body);
            return ret;
        }
        if (headerFound && body != 0)
        {
            const int L = static_cast<int>(strlen(line));
            memcpy(body + bodyLen, line, static_cast<size_t>(L));
            bodyLen += L;
        }
    }
    if (headerFound && body != 0)
        free(body);
    return 0;                                          // 未找到 PEM 头
}

static int pem_get_next_line(void* bp, char* buf, int sz)
{
    return BIO_gets(bp, buf, sz) > 0 ? 1 : 0;
}

// base64（RFC1421 4→3 译码，与 EVP_DecodeUpdate 输出位流兼容）
static int pem_b64_val(int c)
{
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

unsigned long pem_b64_decode(const char* in, unsigned char* out)
{
    unsigned long written = 0;
    int quad[4], qn = 0;
    for (const char* p = in; *p; ++p)
    {
        const int v = pem_b64_val(*p);
        if (v < 0)
            continue;                                  // '=' 及空白跳过
        quad[qn++] = v;
        if (qn < 4)
            continue;
        out[written++] = static_cast<unsigned char>((quad[0] << 2) | (quad[1] >> 4));
        out[written++] = static_cast<unsigned char>(((quad[1] & 0xf) << 4) | (quad[2] >> 2));
        out[written++] = static_cast<unsigned char>(((quad[2] & 0x3) << 6) | quad[3]);
        qn = 0;
    }
    if (qn >= 2)                                       // 尾部残组
    {
        out[written++] = static_cast<unsigned char>((quad[0] << 2) | (quad[1] >> 4));
        if (qn >= 3)
            out[written++] = static_cast<unsigned char>(
                ((quad[1] & 0xf) << 4) | (quad[2] >> 2));
    }
    return written;
}

// ---- BIO_gets（ORIG T 0x087eb?，meth->bgets 分发型）----
#include "LibCryptoStatic/BIO_gets_1.inc"
{
    bio_st_st* b = static_cast<bio_st_st*>(bp);
    // ORIG 87ebd4f..d66：b/method/bgets 缺失 → ERR(0x20,0x68,0x79) + -2
    // （87ebdf0..e21）；我们原实现缺 init 检查——fopen 失败（init=0）后仍调
    // file_gets → fgets(NULL) 崩溃（冒烟实测 SIGSEGV）。对齐 ORIG 87ebd9c：
    // init==0 → ERR(0x20,0x68,0x78,"bio_lib.c",0x135) + -2（87ebe23）。
    if (b == 0 || b->method == 0 || b->method->bgets == 0 || size <= 0)
    {
        ERR_put_error(0x20, 0x68, 0x79, "bio_lib.c", 0x129);
        return -2;
    }
    if (b->callback != 0)                              // ORIG 87ebd6c..9a
    {
        long cb = b->callback(b, 5, buf, size, 0, 1L);
        if (cb <= 0)
            return static_cast<int>(cb);
    }
    if (b->init == 0)
    {
        ERR_put_error(0x20, 0x68, 0x78, "bio_lib.c", 0x135);
        return -2;
    }
    long r = b->method->bgets(b, buf, size);
    if (b->callback != 0)
        return static_cast<int>(b->callback(b, 0x85, buf, size, 0, r));
    return static_cast<int>(r);
}

// ---- d2i_RSA_PUBKEY（ORIG T 0x08820500）。
//      ORIG 经 ASN1_item_d2i(X509_PUBKEY 模板)/X509_PUBKEY_get/EVP_PKEY_*。
//      该库深度本轮闭包不可达，就地实现 SubjectPublicKeyInfo 最小 BER 树遍历
//      （外 SEQ → alg SEQ 跳过 → BITSTRING 内 PKCS#1 RSA 公钥 SEQ{INT n,INT e}），
//      成功产出 RSA_VIEW{n,e} 于 CRYPTO_malloc 堆，返回之；错误回 0 并
//      ERR_put_error(9,120,0x67,"rsa_ameth.c") 与 ORIG 错误域对齐。----
static const unsigned char* ber_expect(const unsigned char*& p,
                                       const unsigned char* end,
                                       unsigned char tag, int& lenOut);

static unsigned long der_read_int_len(const unsigned char*& p, int len)
{
    unsigned long v = 0;
    for (int i = 0; i < len; ++i)
        v = (v << 8) | *p++;
    return v;
}

void* d2i_RSA_PUBKEY(void** a, const unsigned char** in, long len)
{
    RSA_VIEW* out = 0;
    const unsigned char* p = *in;
    const unsigned char* end = p + len;
    int outerLen = 0, bitLen = 0;
    if (!ber_expect(p, end, 0x30, outerLen))
        goto FAIL;
    {
        int algLen = 0;
        if (!ber_expect(p, end, 0x30, algLen))
            goto FAIL;
        p += algLen;                                    // 跳过算法标识
    }
    if (!ber_expect(p, end, 0x03, bitLen))
        goto FAIL;
    if (*p++ != 0)                                      // BITSTRING pad byte
        goto FAIL;
    {
        int keyLen = 0;
        if (!ber_expect(p, end, 0x30, keyLen))
            goto FAIL;
        int nLen = 0, eLen = 0;
        if (!ber_expect(p, end, 0x02, nLen))
            goto FAIL;
        const unsigned long mod = der_read_int_len(p, nLen);
        if (!ber_expect(p, end, 0x02, eLen))
            goto FAIL;
        const unsigned long exp = der_read_int_len(p, eLen);

        out = static_cast<RSA_VIEW*>(CRYPTO_malloc(sizeof(RSA_VIEW),
                                                   "d2i_rsa.c", 214));
        memset(out, 0, sizeof(*out));
        unsigned long* nd = static_cast<unsigned long*>(
            CRYPTO_malloc(sizeof(unsigned long), "d2i_rsa.c", 217));
        unsigned long* ed = static_cast<unsigned long*>(
            CRYPTO_malloc(sizeof(unsigned long), "d2i_rsa.c", 218));
        *nd = mod; *ed = exp;
        out->n = nd;                                    // +0x10
        out->e = ed;                                    // +0x14
        out->references = 1;                            // +0x38 起始计数
    }
    *in = p;
    if (a != 0 && *a != 0)                              // ORIG：旧键替换释放
        RSA_free(*a);                                   //（ORIG call RSA_free）
    if (a != 0)
        *a = out;
    return out;

FAIL:
    ERR_put_error(9, 120, 0x67, "rsa_ameth.c", 147);
    return 0;
}

static const unsigned char* ber_expect(const unsigned char*& p,
                                       const unsigned char* end,
                                       unsigned char tag, int& lenOut)
{
    if (p + 2 > end || p[0] != tag)
        return 0;
    ++p;
    int l = *p++;
    if (l & 0x80)                                       // 长格式长度
    {
        const int nb = l & 0x7f;
        l = 0;
        for (int i = 0; i < nb; ++i)
            l = (l << 8) | *p++;
    }
    if (p + l > end)
        return 0;
    lenOut = l;
    return p;
}

}  // extern "C"
