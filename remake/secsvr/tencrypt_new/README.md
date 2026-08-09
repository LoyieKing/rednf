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
| src/{rc4,tean,idea,gost,des,3way,rc5,rc6,rc2,safer,mmb}.cpp | ✅ 已实现（行为对照二进制逐字节一致） |
| src/TenCrypt.cpp、TencBase.cpp、其余块密码 ~34 个 | ⏳ 下一阶段 |
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
- `md5_process` 在二进制符号表中绑定为 `t`，但 mangled 名为全局形式
  `_Z11md5_processP11md5_state_sPKh`，判定为 strip/localize 产物，保留全局定义。

## 构建

```
make            # 生成 libtencrypt_new.a（-m32 -O2 -DNDEBUG -pthread）
make test       # 编译并运行测试向量
```

> 注：本机工具链为 GCC 13.3（支持 -m32）；原构建为 GCC 4.1.2。符号级一致以
> mangled 名为准，助记符/指令级差异在验收阶段单独评估。
