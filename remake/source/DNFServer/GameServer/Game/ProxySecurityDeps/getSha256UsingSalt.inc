// ORIG T _Z18getSha256UsingSaltPKhiPc @0x0808cc6a sz 0x18c：
//   null→false；首字节 '\0'→true（不处理）；
//   buf=new uchar[len+4]; memset(0); strcpy(src); strcat("nhs");
//   digest[0x20]={0}; CreateCryptoChecksum(CRYPTO_CHECKSUM_TYPE(1))；
//   虚槽 +0xc（仓库 ChecksumAlgo.h 槽序为 MakeChecksumFromMemory，直呼同方法——
//   槽位偏移差异属共享头排列豁免，行为一致）：
//     MakeChecksumFromMemory(buf, len+4, digest, 0x20)；==0x6fffffff → ok=true；
//   for i in 0..31: sprintf(out+i*2, "%02X", digest[i])；
//   DestroyCryptoChecksum(&ck); delete[] buf; return ok。
// [b28d-N7 复验 + round5 五修] ORIG 113 vs our 110：
//   本轮按报告逐段对齐后已修：①buf 清零改 `new[]`+显式 memset（ORIG 0x0808cccd
//   为 call memset，非 value-init 内联循环）；②digest 改 `= {0}` 内联 word 循环
//   （ORIG 0x0808cd11..0x0808cd34 `and $0xfffffffc` 循环，非 memset call）；
//   ③补 3 条原码死写入 int（0x20/0x41/0x4，全函数无读取点，P171 照抄）——
//   补齐后 ok/-0x1d、3×int/-0x1c..-0x11、salt/-0x21..-0x1e 与 ORIG 逐槽全等；
//   ④循环尾测改 `(unsigned int)i < 0x20` 复现 `mov slot;cmp;setbe`（P310）。
//   ⑤[round5] 补 `int i = 0;` + `for (i = 0; …)` 双初始化：ORIG 0x0808cd87/
//   0x0808cd8e 连发**两条** `movl $0x0,-0xc(%ebp)`（P191 配套）⇒ our 109→110。
//   剩余 3 条 = N7 族：ORIG 虚函数指针提前入 %esi（push/pop esi + 帧 0x60）且
//   `len+4` 走 mov+add，4.4.7 恒 `call *%eax` + lea 折叠（帧 0x64）——间接 call 目标
//   寄存器角色差。**round5 路由实测：447 / 仅 446 后端 / 446 后端+446 头集 三态
//   逐函数读数全同（diff=2）⇒ 路由变更无收益（P230 补的「4.4.6 可翻」本 TU 不成立）。**
//   p167 CALLSITE CLEAR。
bool getSha256UsingSalt(const unsigned char* src, int len, char* out)
{
    if (src == 0)
        return false;
    if (src[0] == '\0')
        return true;

    bool ok = false;                              // ORIG -0x1d
    // ORIG 0x0808cc9a/0x0808cca1/0x0808cca8：ok 预清后连发 3 条
    // `movl $0x20/$0x41/$0x4,-0x1c/-0x18/-0x14`，全函数**无任何读取点**
    // ⇒ 原码遗留死写入（P171 照抄；非语义字段）。
    int deadA = 0x20;                             // ORIG -0x1c（死写入）
    int deadB = 0x41;                             // ORIG -0x18（死写入）
    int deadC = 0x4;                              // ORIG -0x14（死写入）
    unsigned char salt[4];                        // ORIG -0x21..-0x1e 逐字节置值
    salt[0] = 'n';
    salt[1] = 'h';
    salt[2] = 's';
    salt[3] = '\0';

    // ORIG 0x0808cccd..0x0808ccec：`call _Znaj` 后存槽，再 **call memset** 清零
    // 全长（`mov len+4,0x8(%esp); movl $0,0x4(%esp); mov buf,(%esp); call`）
    // ⇒ 原码是 `new[]` + 显式 `memset(buf,0,len+4)`，非 value-init 内联循环。
    unsigned char* buf = new unsigned char[len + 4];      // ORIG _Znaj
    memset(buf, 0, len + 4);                              // ORIG call memset
    strcpy(reinterpret_cast<char*>(buf),
           reinterpret_cast<const char*>(src));
    strcat(reinterpret_cast<char*>(buf),
           reinterpret_cast<const char*>(salt));

    // ORIG 0x0808cd11..0x0808cd34：digest 清零是**内联 word 循环**
    // （`lea -0x48,%edx; mov $0x20,%eax; and $0xfffffffc,%ebx; loop mov/add/cmp/jb`）
    // ⇒ 原码是栈数组零初始化 `= {0}`（P214③），非 memset call。
    unsigned char digest[0x20] = {0};             // ORIG -0x48（内联 word 循环）

    IChecksum* checksum = CreateCryptoChecksum((CRYPTO_CHECKSUM_TYPE)1);
    if (checksum->MakeChecksumFromMemory(buf, len + 4, digest, 0x20) ==
        static_cast<int>(0x6fffffff))
        ok = true;

    // ORIG 0x0808cdc2..0x0808cdcb：尾测为**两段寄存器形**
    // （`mov -0xc(%ebp),%eax; cmp $0x1f,%eax; setbe %al`）⇒ 下标局部 int +
    // 显式无符号比较（P310：4.4 对 unsigned 局部恒折槽直比 ⇒ 不改局部类型）。
    // ORIG 0x0808cd87/0x0808cd8e 连发**两条** `movl $0x0,-0xc(%ebp)`（idx 0x119/0x124）
    // ⇒ 原码是「`int i = 0;` + `for (i = 0; …)`」双初始化形（P191 配套）。
    int i = 0;
    for (i = 0; (unsigned int)i < 0x20; ++i)
        sprintf(out + i * 2, "%02X", digest[i]);

    DestroyCryptoChecksum(checksum);
    delete[] buf;                                 // ORIG _ZdaPv
    return ok;
}