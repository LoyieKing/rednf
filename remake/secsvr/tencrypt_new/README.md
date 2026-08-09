# tencrypt_new —— 闭源加密库复刻（Tencent TSS SDK）

原始工程：`/data/yunfeiyang/dev/branch/src/common/tencrypt_new`（GCC 4.1.2 SUSE，
独立于主工程构建后静态链接进三个服务二进制）。gunnersvr/zergsvr 仅链接哈希子集
（12 个 TU），secagent 链接全部 59 个 TU（含对称加密）。

## 复刻依据

- **头文件**：`inc/*.h`、`include/TenCrypt.h` 由 DWARF 类型信息重建
  （`toolchain/gen_headers.py`），类型名按二进制 mangled 名校正
  （如 `MD4_CTX`、`__md2`、`_SHA1_CTX`、`md5_state_s`）。
- **算法实现**：全部为公开标准算法，采用公版参考实现
  （RFC 1319/1320/1321/3174、zlib、RIPEMD/HAVAL/GOST 参考代码），
  按 DWARF 桩的类结构/成员签名/局部变量名封装。
- **验收**：
  1. 编译后 `nm` 符号与二进制（`nm -C`）对照；
  2. 标准测试向量（本目录 `test_hash.cpp`）；
  3. 行为级对照二进制反汇编（逐函数，后续阶段）。

## 状态（2026-08-09，哈希族完成）

| 文件 | 状态 |
|---|---|
| inc/{md5,sha1,md2,md4,crc32,adler32}.h | ✅ 已重建并校正 |
| src/{md5,sha1,md2,md4,crc32,adler32}.cpp | ✅ 已实现（测试向量通过） |
| src/{ripemd160,ripemd128}.cpp | ✅ 已实现（官方参考，测试向量通过） |
| src/{haval128,haval160}.cpp | ✅ 已实现（Tcl TRF 官方认证向量，3 组全过） |
| src/gosthash.cpp | ✅ 已实现（Saarinen 官方参考，t1 向量过） |
| src/TenHash.cpp | ✅ 已实现 11/11 个 Ten* 哈希封装 |
| src/{rc4,tean,idea,gost,des,3way,rc5,rc6,rc2,safer,mmb,mbc,cast,loki89,loki91,feal8,nsea}.cpp | ✅ 已实现（行为对照二进制逐字节一致） |
| src/TenCrypt.cpp、TencBase.cpp、其余块密码 ~28 个 | ⏳ 下一阶段 |
| include/TenCrypt.h（ITsLocal 等） | ✅ 头已重建，实现待做 |

## 哈希族验收结果

- 测试向量：11 个哈希算法 ALL PASS（`make test && ./test_hash`）。
- 符号级对照：104/104 个哈希族符号（Ten* + 类成员 + md5_*）与 secagent
  二进制 mangled 名完全一致，零缺失。
- HAVAL 参考镜像说明：milahu/haval 编译输出与官方认证数据不符（判定有缺陷），
  最终采用 Tcl TRF 归档中的官方认证向量，已存
  `../third_party/references/haval_cert_data.txt`。

## 对称加密族进展（2026-08-09 晚）

| 文件 | 算法 | 要点 | 验证 |
|---|---|---|---|
| rc4.cpp | RC4 | 固定 16 字节密钥，8 字节对齐缓冲 | 行为对照通过 |
| tean.cpp | TEA 变体 | 非标准：y 先于 sum+=DELTA；表达式 `((v<<4)^(v>>5))+(v^sum)+k[idx]` | 行为对照通过 |
| idea.cpp | IDEA | en_key_idea 标准三分支；**cipher_file 大端读→LE 写，decipher_file 镜像** | 行为对照通过 |
| gost.cpp | GOST | S 盒提取自 .rodata 0x081ccf00；32 轮正序×3+逆序；f=替换+循环左移 11 | 行为对照通过 |
| des.cpp | DES | **Applied Cryptography 附录 B 版**（非 libdes）：DesKey=PC1→轮转→PC2(bigbyte)→cookey 32 子钥；DesBase=合并 IP/FP desfunc+大端读写；SP 表从 0x081be0e0 提取（行序 SP8..SP1，成员 SP1←row7…SP8←row0）；benc!=0 时子钥逆序 | AC 官方向量 c95744256a5ed31d + 二进制 DesKey/DesBase 逐字节一致 |
| 3way.cpp | 3-Way | Daemen 原始 C 规范（块 12 字节，11 轮，STRT_E=0x0b0b/STRT_D=0xb1b1） | uni_call 加解密逐字节一致 |
| rc5.cpp | RC5-32/12/16 | 26 子钥、16 字节密钥、78 步混合；包装按 16 字节分块但只加密前 8 字节（二进制怪癖） | uni_call 逐字节一致 |
| rc6.cpp | RC6 | Gladman 结构；set_key 仅用密钥前 8 字节（逐字节装入 L[0..7]）；解密输出 (a, b-k0, c, d-k1) | uni_call 逐字节一致 |
| rc2.cpp | RC2 | 公版；set_key 无有效位数缩减（仅 xkey[0]=permute[xkey[0]]）；16 字节密钥 | uni_call 逐字节一致 |
| safer.cpp | SAFER 变体 | 16 字节块自定义 PHT/置换；SetKey 双/单 expf 查找 + lk 回绕 | do_fr/do_ir 8 组随机输入逐字节一致，加解密一致 |
| mmb.cpp | MMB | 标准 MMB 但 f/g 去掉 x[3] delta 条件；SetKey 自定义派生（0x33cfc738/0x19860719 + iv_tbl[64]） | uni_call 逐字节一致 |
| mbc.cpp | MBC | 自定义：SetKey 8 组 iv_tbl 扩展 + 字节表置换 key_exp[32]；Encrypt 16 轮（st/链/白化表从反汇编提取） | key_exp 32 项全对，加解密逐字节一致 |
| cast.cpp | CAST-128 | Steve Reid 公版（12/16 轮，S1-S8 表从 0x081b8c20 提取） | uni_call 逐字节一致 |
| loki89.cpp | LOKI89 | AC 参考；P/sfn 表提取；f/s 标准；16 轮 + 首尾密钥混合 | uni_call 逐字节一致 |
| loki91.cpp | LOKI91 | AC 参考；16 子钥（ROL12/ROL13）；**s() 用 t=(c+((r*17)^0xff))&0xff（与 89 不同）** | uni_call 逐字节一致 |
| feal8.cpp | FEAL-8 | NTT 1989；Rot2 表提取（=rotl(x,2)）；**f() 的 S1 需 +1（曾漏写）** | uni_call 逐字节一致 |
| nsea.cpp | NSEA | AC 参考两轮 16 步 + CBC；**自定义密钥派生：initTempKey=MSVC rand LCG；initSBoxes 两轮（第一轮 LCG tempKey 置换，第二轮用 key 覆盖 tempKey 后再次置换）** | S 盒与加解密逐字节一致 |
| mars.cpp | MARS | 加解密为 Gladman 标准 f_mix/f_ktr/b_mix/r_ktr；**setkey 完全自定义：15 项 t_key（IM1/IM2/IM7/I4M 表驱动置换）+ 4 轮外层（t_key[i]=rotl3(t_key[IM2[i]]^t_key[IM7[i]])^t_key[i]^(4i+j) + 4 轮 rotl9 混合 + 按 I4M 前 10 项拷贝到 ll_key）+ 密钥修正无 bit31 修补** | ll_key/t_key 全状态与加解密逐字节一致（10 组随机向量） |
| diamond.cpp | Diamond2 Lite | AC 公版（DIAMOND/DIAMOND2.CPP）结构；**差异：Init 固定分配 Ccitt32Table(0x400)+s/si(0xf000)，setkey 不分配；keyrand 无 sbox（始终直取 key）；makeonebox 无 sbox 链；setkey 返回 0/-1/-2；Diamond 包装固定 (key16,3轮,!benc,8块)=Lite 模式** | s/si 全状态与加解密逐字节一致（10 组随机向量） |
| frog.cpp | FROG | AC 公版（FROG/frog.c）make_perm/make_ikey/enc/decrypt；**差异：SetKey 只用 sim_key（无 loc_key/无二次加密），kb=min(keylen>>3,32)；seed 为 Init 分配的堆副本（表=AC seed[256]）；Frog 包装 16 字节块循环 + 余数交 CreateTsLocal()->Seattos** | sim_key 全状态与加解密逐字节一致（10 组随机向量） |
| hpc.cpp | Hasty Pudding Cipher | AC 公版（HPC/hpc.c）set_key/encrypt/decrypt/mult_64；**Init 固定分配 l_key(0x8f0)/spice(0x40)/p119/e19/r220；Hpc 包装 setKey(key,0x20)+16 字节块循环+TsLocal 余数** | l_key 全状态与加解密逐字节一致（10 组随机向量） |
| gene.cpp | Gene / GeneNew | 自定义流式密码（反汇编还原）：SetGeneKey 的 table[i]=i 后 MSVC rand LCG 洗牌（*0x343fd+0x269ec3，交换 table[i-1] 与 table[(seed>>16)&0xff]），buf=key^固定8字节；Gene 逐字节 ^i/^kbuf/查表/^kbuf/^i；GeneNew 逐字节 ^i^key[i&7] | Gene/GeneNew 逐字节一致（8 组随机向量） |
| seattos.cpp | Seattos | 自定义流式密码（反汇编还原）：SetseattosKey 按 seattosIP[64] 对密钥 64 位比特置换；Seattos 逐字节 v1=buf^t16[i&15]^key[i&7]，benc 时 v2=v1+t16 否则 v2=v1-t16，输出=(t16^v2)^key | SetseattosKey/Seattos 逐字节一致（10 组随机向量） |
| adder.cpp / subtracter.cpp | Adder / Subtracter | 自定义流式密码：SetKey 均按 64 项 IP 表做密钥比特置换（两表相同）；逐字节 v=k8[i&7]，benc 计数=v（v>=0x80 或 0 跳过）dec 计数=256-v，Adder 循环加 v、Subtracter 循环减 v（互补还原） | SetKey/Adder/Subtracter 逐字节一致（10 组随机向量） |
| shift.cpp / ring.cpp | Shift / Ring | Shift：SetshiftKey 用 SHIFTIP+SHIFTBBIT 置换；shiftCrypt 收集/展开（enc 在 buf=local 后返回，dec 走展开循环，内部用 SHIFTBB）；Ring：SetRingKey 表=Gene 同款 LCG 洗牌，块变换逐字节 ^rotr(k8[i],i)^T[8]，加密前/解密后 ^256 表 | Shift/Ring 全部函数逐字节一致（6 组随机向量） |
| gramary.cpp | Gramary | 自定义块密码：gra_round 按 16 模式把 8 参数放入槽位后 s0|s1<<1|...|s7<<6|s6<<7（oracle 提取排列表）；gra_crypt 结构与 shiftCrypt 同构，主循环两次 gra_round（mode=key 低/高半字节，dec 反序） | gra_round 全 16 模式与 gra_crypt 逐字节一致 |
| desnew.cpp | DesNew | 自定义 8 字节块密码：NF 256 字节 S 盒；encrypt 9 轮（密钥索引 +7 mod 15），out4..out0 密钥索引 r/(r+1)/.../(r+6) mod 15，si 初始 0 并依次切到 key[7]/key[8]/key[9]，r+4==15 退出；decrypt 镜像（r-7 mod 15，si=key[9]→key[8]→key[7]→0） | encrypt/decrypt 与加解密闭环逐字节一致（8 组随机向量） |
| blowfish.cpp | Blowfish | 标准 Blowfish（P/S 初始表从二进制提取，与公版一致）；BlowFish8/16/24/32/56 为 8/16/24/32/56 字节密钥 + 8 字节块 ECB + TsLocal 余数；Encrypt/Decrypt flag0=ECB flag1=CBC(m_oChain) | BlowFish/块/包装层逐字节一致（6 组随机向量） |
| d3des.cpp | D3DES 族 | 自实现三密钥 3DES 变体（非 libdes）：deskey=PC1→totrot→PC2+bigbyte[0..23]→cookey 掩码公式；desfunc=经验提取 IP/FP 置换 + 8 轮 R^=F(ror4(L),k0,k1) 交错；des3key 解密模式密钥顺序 (key+8,EN0)→(key,DE1)→(key+16,DE1)；D3des161/162=24 字节密钥 + 8/16 字节块 ECB + TsLocal 余数，D3des24=72 字节密钥 + 24 字节块；TenD3desN 包装 key 在前 | D3des 加解密 + TenD3des161/162/24 与二进制逐字节一致 |
| desbig.cpp | CDesBig | 自定义大表 DES：des_ky=密钥逐位反转（nibble/字节/16 位三段交换）→28 位半密钥按 KS_TAB 右移 1/2 →8 个 6 位索引查 P2_TAB[8][64] 交叉拼 64 位轮子钥；F 轮 = ror4(x)^sk_hi / x^sk_lo 分 8 字节查 SX_TAB[8][256]（偶数组取 a 字节、奇数组取 t 字节）OR 合并；des_ec/des_dc=自定义 IP（非标准 DES，奇数位→X、偶数位→Y）→16 轮（dc 子钥逆序 sk15..sk0）→自定义 FP（先 ror2(B)）；des_ecm=无置换核心；X_{r+1}=F(X_r,sk_r)^X_{r-1}，输出 (X15,X16)；TenDesBig 包装 | des_ky/ec/dc/ecm 160 向量逐字节一致；往返闭环；TenDesBig 对齐块 24/24 |
| des2p.cpp | CDes2p | libdes 位操作版（des_set_key/des_ecb_encrypt/des_set_odd_parity/des_func）：pkbit/unbit 64 位比特数组，E 展开 + PC2 子钥查表 | 与二进制逐字节一致 |
| despc.cpp | CDesPC | Phil Karn DES：pc_perminit 由 1 基位号表（IP/FP）构建 perm 查表；**PC_NIBB 掩码坑（PC2 半字节组）** | 与二进制逐字节一致 |
| destoo.cpp | CDesToo | 4.3BSD des.c：fsf=E 展开（ror(r,1) 分 6 位段 ⊕ 子钥字节）→ 静态 FUCKING_TOO_SP[8][64]；**静态表，成员表为死代码** | 与二进制逐字节一致 |
| desmo.cpp | CDesMo | Baldwin 式单函数 DES：pc1m/pcrn 置换 + 8 轮 nibble 旋转 + 最终散布 | mo_crypt 加解密与二进制逐字节一致 |
| fastdes.cpp | CFastDes | Eric Young 老版 libdes “fast” DES：fsetkey PC1 查表+28 位轮转+7 组 hKS/lKS PC2；fencrypt IP/16 轮 D_ENCRYPT/FP | fsetkey/fencrypt 与二进制逐字节一致 |
| desdea.cpp | DesDea / des_sched / des_dea3 | **3DES-EDE2（16 字节密钥）**：des_sched=dea_pc1 查表 OR 出两个 32 位状态 → 28 位右旋（dea_ls 1/1/2/2/.../1）→ 每轮 8 字节查 dea_pc2 合成 8 字节轮子钥（小端写）；des_dea3=48 轮展开（E_K1→swap→D_K2→swap→E_K1，解密镜像），IP 用 dea_ip、SP 用 s_and_p（idx0=(R<<1)&0x3e，R bit31 置位时 |1）、FP 用 ip_inv（输入 R LE++L LE）；**坑：段边界 L/R 交换（swap），非单链直续**；DesDea 包装 des_sched(key)+des_sched(key+8) → 8 字节块循环 des_dea3 → TsLocal 余数 | des_sched/des_dea3/DesDea 与二进制逐字节一致（C++ 交叉 100 组 x2）；往返闭环 |
| amoeba.cpp | CAmoeba | 自定义 64 位位级密码：Init 惰性分配 amobbit（多比特掩码 {0x81,0x40,0x22,...}）/amobb/amoIP 三表；amoKey=按 amoIP 位号从 key 提 64 位（key[amoIP[i]>>3] & amobbit[amoIP[i]&7]）；amoEnCrypt/amoDeCrypt=IP 位提取 → 8 轮（加密密钥 1..8、位 6..0 再 7；解密镜像 8..1、位 0..6 再 7），每轮按密钥位选 SET/CLEAR 块（字节旋转 + NOT 扩散 + 交叉），bit7 另有特殊/普通块；**坑：SET/CLEAR 字节起始=8*b、交叉目标=CLEAR 时 B+7、解密扩散循环含 eax=0**；Amoeba 包装 amoKey 生成 Add → 8 字节块循环（Add 作块密钥）→ TsLocal 余数 | amoKey/amoEnCrypt/amoDeCrypt/Amoeba 与二进制逐字节一致（C++ 交叉 100 组 x2）；往返闭环 |

### DES 还原要点（Applied Cryptography 附录 B）

- `DesKey(key, benc, KnL)`：pc1 表→pc1m 位提取→totrot 循环左移→pc2+bigbyte
  生成 16 对 24 位子钥→**cookey 变换**（`(raw&0xfc0000)<<6 | (raw&0xfc0)<<10 |
  (raw1&0xfc0000)>>10 | (raw1&0xfc0)>>6` 等）输出 32 个 32 位“熟化”子钥。
  **benc!=0 → m=(15-i)<<1（逆序），benc==0 → m=i<<1**（与 AC 的 EN0/DE1 相反）。
- `DesBase(KnL, in, out)`：scrunch（大端装 2×32）→ desfunc（合并 IP/FP 置换、
  8 轮×4 子钥，S 盒顺序 SP7,SP5,SP3,SP1,SP8,SP6,SP4,SP2）→ unscrun（大端写出）。
- `Init`：先清零 SP1..SP8 成员，再逐个惰性 `new unsigned int[64]`，从静态表
  `DES_SP[8][64]`（行序 SP8..SP1）复制；`UnInit` 逐个 `delete[]` 并置空。
- 验证：AC 官方向量（key 0123456789abcdef / plain 0123456789abcde7 →
  cipher c95744256a5ed31d）✓；uni_call 调二进制 DesKey 两种模式 KnL 逐字节一致；
  DesBase 输出逐字节一致（0011223344556677 → cadb6782ee2b4823）；解密闭环 ✓。

### 复刻过程中的方法论要点

- **“二进制即 oracle”**：每实现一个算法，用 `toolchain/uni_call.py` 直接调二进制的
  函数，8 字节对齐缓冲 + 随机输入对拍；不匹配时 hook 二进制内部状态逐轮对比定位。
- **Init 必须先清零成员指针**（栈对象未初始化成员导致跳过分配 → 崩溃），
  二进制 Init 均先置零再惰性分配。
- **rotl/ror 方向**：x86 `ror $0x1b` = rotl 5，极易搞反；移位计数需 &31。
- **宏参数顺序**：自定义宏若参数较多（如 MBC 的 MB_ROUND 13 个参数），
  调用与签名顺序必须一致（曾因 w/wt 顺序颠倒导致 UB 越界）。
- **局部变量不能依赖 &x0 连续排布**（-O2 寄存器分配下是 UB），必须用真数组 x[4]。
- **白化目标要跟踪 ecx/edi 指针**：编译器会穿插 `mov -0xc(%ebp),%ecx` 重载指针，
  手动读反汇编时极易把 x1/x3 目标看反（MBC R12-R15 为此反复修正）。
- `md5_process` 在二进制符号表中绑定为 `t`，但 mangled 名为全局形式
  `_Z11md5_processP11md5_state_sPKh`，判定为 strip/localize 产物，保留全局定义。

## 构建

```
make            # 生成 libtencrypt_new.a（-m32 -O2 -DNDEBUG -pthread）
make test       # 编译并运行测试向量
```

> 注：本机工具链为 GCC 13.3（支持 -m32）；原构建为 GCC 4.1.2。符号级一致以
> mangled 名为准，助记符/指令级差异在验收阶段单独评估。
