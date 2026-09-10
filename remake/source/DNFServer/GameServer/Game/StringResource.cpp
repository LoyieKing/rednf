// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StringResource/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— StringResource / CStringManager 实现（报告依据见 .h 文件头）。
// 逐段转写自 ORIG objdump（地址见各函数注释）；static helper
// parseStringLine（ORIG t _ZL15parseStringLinePcPiPS_ @0x08ad490f）同 TU。  

#include "StringResource.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <vector>

// ---------------------------------------------------------------------------
// StringResource C1/D1 —— ORIG 仅构造/析构 std::map<int,CharString> 成员。
// ---------------------------------------------------------------------------
#include "StringResource/StringResource.inc"
#include "StringResource/dtor_StringResource.inc"

// ---------------------------------------------------------------------------
// parseStringLine —— ORIG t _ZL15parseStringLinePcPiPS_ @0x08ad490f。
// 行格式 "<key数字> > <value>"；'/' 开头("//")为注释；'\r'/'\n' 开头空行；
// 返回：0=成功（*key=atoi(key 区)、*value=value 起）、-1=key 含非数字、
// -2=key 位数>11。value 内 "\n" 转义为真实换行并左移，
// 其余 '\' 删除前缀字符（与 ORIG 循环形态一致），行结束('\r'/'\n')截断。
// 错误码经调用方 "Parse error : line:%d reason:%d" 记入 error。
// ---------------------------------------------------------------------------
// [b28d-solo-StringResource AE: 155=155 逐条全等（P264 归一后） | ORIG 0x08ad490f]
static int parseStringLine(char* line, int* key, char** value)
{
    char* p = line;
    // 前导空白（' '/'\t'）跳过；空行/换行起始 → 失败（ORIG 返回非解析态）
    while (*p)
    {
        if (*p == '\r' || *p == '\n')
            return 0;                   // ORIG @0x8ad4931 共享 return-0 岛
        if (*p == ' ' || *p == '\t')
            ++p;                        // 免 continue：p++ 作循环体尾 fall 回条件
        else
            break;
    }
    // ORIG @0x8ad496d：*p==0（全空白行）与 "//" 注释共享同一 return-0 岛
    //（`test; je island` + `//` 双 cmp 短落到同一 `mov $0` 块），故合并短路或。
    if (*p == '\0' || (p[0] == '/' && p[1] == '/'))
        return 0;

    char* keyStart = p;
    char* valueStart = 0;
    while (*p)
    {
        if (*p == '>')
        {
            *p = '\0';
            ++p;
            valueStart = p;
            break;
        }
        if (*p <= '/' || *p > '9')
            return -1;                  // key 非 [0-9] 字符（ORIG 共享 -1 岛）
        ++p;
    }
    // ORIG @0x8ad49ec：valueStart==0（未找到 '>'）与空 key（len==1）共享
    // 同一 return -1 岛（`cmpl $0; je` 短落到 -1 块），故用 || 合并。
    if (valueStart == 0 || (int)(valueStart - keyStart) == 1)
        return -1;

    if ((int)(valueStart - keyStart) - 1 > 10)
        return -2;                      // ORIG cmp $0xa 上限

    // value 内扫描：'\r'/'\n' 终结该行；"\n" 转义真实换行并左移补位。
    // ORIG @0x8ad4a4f 对 '\' 后非 'n' 一律不处理（两 jne 直落 i++，无删
    // 反斜杠逻辑）——此前 EXTRA 的删除循环系误读，已删。
    // 不写 `p = valueStart;`：key 扫描 break 时 p 恰停在 valueStart（ORIG
    // @0x8ad49b3 `mov %eax,-0x10` 后即复用 p），显式重载会多 3 条
    //（mov %eax; mov %eax; jmp），现复用循环尾 p 值（见我们汇编 value 段入口）。
    while (*p)
    {
        if (*p == '\r' || *p == '\n')
        {
            *p = '\0';
            break;
        }
        // ORIG @0x8ad4a5e-0x163：命中 "\n" 转换+左移后与 miss 路径共享同一条
        // p++（@0x163），不 continue 跳过步进。
        if (*p == '\\')
        {
            if (p[1] == 'n')
            {
                *p = '\n';
                // 左移覆盖被吃掉的第二个字符（ORIG copy 循环）。
                for (char* q = p + 1; *q; ++q)
                    *q = *(q + 1);
            }
        }
        ++p;
    }

    *key = atoi(keyStart);
    *value = valueStart;
    return 0;
}

// ---------------------------------------------------------------------------
// add —— ORIG T 0x08ad47d0：map.insert(make_pair(id,value))，返回新增与否。
// set —— ORIG T 0x08ad48e6。
// get —— ORIG T 0x08ad4878：find != end 时 *out=&second 返回 1，否则返回 0。
// ---------------------------------------------------------------------------
#include "StringResource/add.inc"
{
    // ORIG @0x8ad47d0：insert(std::pair<int, CharString>(id, value)) —— 临时
    // 值 pair 直构（C1IRiRKS0_ 引用实参形）→ EOS_ 右值转 pair<const int,...>
    // → insert。make_pair 会先物化 make_pair 调用改变装载序（P61/P81）。
    return m_strings.insert(std::pair<int, CharString>(id, value)).second;
}

#include "StringResource/set.inc"
{
    m_strings[id] = value;
}

#include "StringResource/get.inc"
{
    std::map<int, CharString>::const_iterator it = m_strings.find(id);
    // [b28c-lane17] ORIG 0x08ad4898：`operator!=` + 成功块直落、失败 mov$0 归尾块
    // ⇒ 正极性包裹（P220 迭代器比较极性）。
    if (it != m_strings.end())
    {
        *out = &it->second;
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// loadFromString —— ORIG T 0x08ad4ac2。
// clear 表后逐行解析；成功且 key!=0 → add(key, CharString(value))；
// key==0 的条目跳过；重复 key 与解析错误写入 errOut（format 串逐字一致：
//   "Parse error : line:%d reason:%d"
//   "Duplicated key : line:%d key:%d" )。
// 行终结：'\n' 直接进下一行；'\r\n' 连续则一并消费（ORIG i++ 补偿）。
// i==len 时对最后一行就地 NUL 终结后解析。遍历边界 i<=len（setle 条件）。
// ---------------------------------------------------------------------------
// [b28d-solo-StringResource N(N93/N98/N18b/N104): 块内局部槽方向相反（ORIG 声明序深→浅
// val-0x34/key-0x30/msg-0x2c/v-0x28/dupMsg-0x24；4.4 首次使用序 -0x24/-0x28/-0x2c/-0x30/-0x34，
// P212 全头部声明实测劣化 165→205）+ r 槽比较 4.4 恒 mem-cmpl（N18b，unsigned 化无效）+
// EH 少 1×_Unwind_Resume（N104 块合并）；163/165，p167 MISSING 1×_Unwind_Resume，语义完成
// | ORIG 0x08ad4ac2]
// [M2S5StringRaw 2026-09-11 复核] 3 处返回物化 `mov $0/$1,%eax`(ORIG) vs
// `mov $0/$1,%ebx` + 尾 `mov %ebx,%eax`(4.4)：探针 /tmp/probeD|E（auto_ptr 活跃
// 作用域内 return，带/不带显式块）实测 4.4 恒提前物化到 callee-saved %ebx ⇒
// N93 后端族（非局部作用域问题，源形不可翻）；P216/P339 不适用。
// 残余另有 N18b（r 槽 mem-cmpl vs ORIG 寄存器比较）与 N104（少 1 _Unwind_Resume）。
// 163/165 维持，p167 MISSING 1×_Unwind_Resume。
#include "StringResource/loadFromString.inc"
{
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N18b）→ 试=④：r 槽零比较 4.4 恒
    //   `cmpl $0x0,-0x10(%ebp)`（ORIG `mov -0x10(%ebp),%eax; test %eax,%eax`；int/unsigned
    //   两态逐字节同，旧实测已回退）+ 尾部返回物化 `mov $0/%1,%ebx` + 尾 `mov %ebx,%eax`
    //   （ORIG 直 `mov $0/%1,%eax`，探针 auto_ptr 作用域双形同折）——P212 全头部声明实测
    //   165→205 反劣。②prett ③strall CLEAR。⇒ 维持 N18b/N93/N104。
    m_strings.clear();

    // toTString(const char*)：ORIG _Z9toTStringPKc @0x08adeb75（恒等转换，
    // 定义 SyncScriptDeps.cpp）；本 TU 属字符串底座，按 InitScriptProducers_5
    // 先例局部声明避免拖入 SyncScriptDeps.h 依赖。
    const char* toTString(const char* pszSrc);

    char* val;                      // ORIG -0x34（P212 全头部声明，先声明占深槽）
    int key;                        // ORIG -0x30
    int lineNo = 0;                 // -0x20
    char* lineStart = text;         // -0x1c
    int i = 0;                      // -0x18
    while (i <= length)
    {
        // ORIG @0x2c-0x4a：c 为 byte 槽（-0x11，mov %al）；三条件
        // cmpb je/cmpb je/cmp jne 全部直跳，共享行末 i++（@0x1fd）——
        // 非行内联 \0/补偿块。
        const char c = text[i];
        if (c == '\n' || c == '\r' || i == length)
        {
            ++lineNo;                   // 行号在边界块首自增（@0x50）
            text[i] = '\0';
            val = 0;                    // ORIG @0x5d 唯一 0 初始化（块内）
            // [D01 · C05② 细化负例 2026-09-10] `test %eax,%eax` 零比较子型的
            // 类型旋钮实测无效：本点 ORIG 为 `mov -0x10(%ebp),%eax; test %eax,%eax`
            // （载寄存器两段），而 4.4 对具名局部恒折 `cmpl $0x0,-0x10(%ebp)`，
            // **int 与 unsigned 两态同形**（改 unsigned 后 fresh .o 逐字节不变、
            // 四数 43/63/1/3 不变）⇒ 已回退 int，本点归 N18b 常量比较子型。
            int r = parseStringLine(lineStart, &key, &val);   // -0x10
            if (r != 0)
            {
                // ORIG：仅 error!=0 时报错并 return false；error==0 时
                // 忽略解析失败继续（@0x87 je →val 检查）。
                if (error != 0)
                {
                    // ORIG 0x08ad4b5c：format 为静态 sret，直写 -0x2c 局部槽，
                    // 无 C1Ev/无 op=（P215 展开形）。
                    // [N1F 2026-09-11] msg 作用域收口：ORIG 0x08ad4ba2-0x08ad4bad =
                    //   dtor(-0x2c) 先、`mov $0x0,%eax` 后（返回物化在清理之后）⇒
                    //   `return false;` 在 msg 作用域**之外**，4.4 才不发 %ebx 暂存。
                    {
                        CharString msg = CharString::format(
                            "Parse error : line:%d reason:%d", lineNo, r);
                        *error = msg;
                    }
                    return false;
                }
            }
            if (val != 0)               // ORIG @0xf5 判 val（非 key）
            {
                // [N1F 2026-09-11] v 作用域收口（P0/EH 轴）：ORIG 0x08ad4bf8-0x08ad4bfe 在
                //   `call add`（0x08ad4bf0）后**立即**内联销毁 -0x28（v），随后才算
                //   0x08ad4c03 `movzbl -0x9(%ebp),%eax`（!ok）⇒ ORIG 中 v 的 dtor 不在
                //   「返回/续循环」共享清理路径上，且返回物化是 `mov $0,%eax`（0x08ad4bad/0x08ad4c92）
                //   而非 4.4 的 %ebx 暂存 + %esi 选择子。源形 = v 收进内层块、ok 提到块外。
                bool ok;                           // -0x9 byte 槽（ORIG 0x08ad4bf5 mov %al,-0x9(%ebp)）
                {
                    CharString v(toTString(val));  // -0x28（toTString 恒等）
                    ok = add(key, v);
                }
                if (!ok)
                {
                    // 重复 key：写 err 后**整表加载失败返回**（@0x16c-0x1d5）。
                    if (error != 0)
                    {
                        CharString dupMsg = CharString::format(
                            "Duplicated key : line:%d key:%d", lineNo, key);
                        *error = dupMsg;
                    }
                    return false;
                }
            }
            // \r\n 连续补偿在 val 处理之后（@0x1d7），随后 lineStart 前移。
            if (c == '\r' && text[i + 1] == '\n')
                ++i;
            lineStart = text + i + 1;
        }
        ++i;
    }
    return true;
}

// ---------------------------------------------------------------------------
// loadFromResource —— ORIG T 0x08ad4dfa（size 0x0a）：空函数（push ebp /
// mov $0,%eax / pop ebp / ret）——真实 ORIG 即恒返 false（资源型装载入口
// 在 ORIG 中未启用），转写为空实现。签名 (void*, int, CharString*)。
// ---------------------------------------------------------------------------
#include "StringResource/loadFromResource.inc"
{
    return false;
}

// ---------------------------------------------------------------------------
// loadFromBinary —— ORIG T 0x08ad4e04（报告全文；§6.2 大函数口径）。
// 【验收 batch-11】orig_insns=361 / our=363（gap −2）；P167 调用点差集归零
// （new[]/delete[]×2、memcpy×2、convertFromUtf8×2、convertToCharString×3、
// CharString D1×10/aSER×4/WString D1×6 全命中）。残余 2 行 = round4 的
// 4.1 sar/shr 有符号折叠 vs 4.4 cmovs（N81 族）与 XOR 循环体 shl+add vs
// 4.4 lea 折叠（N18a/P30），封顶勿再追。
// 分支一（SC01）：magic "S","C","0","1"；origSize=*(int*)(data+4)；
// payloadBytes=(size+3)/4*4（ORIG sar/shr 有符号 /4*4 形态 @0x069）；
// new[](payloadBytes) 后 memcpy(buf, data+8, size)（拷贝长度=size，
// P171 照抄）；running-key XOR：k0@-0x24 先于 new、cur/key 二次复制槽
// （-0x18/-0x14）、循环体三处地址重算（*p=*p^key; key^=*p，P38/P157）；
// convertFromUtf8(buf, origSize, ws) 结果落 byte 槽，delete[] 带显式判空
// 守卫，失败 return false（ws 具析构）；成功 content=convertToCharString(ws)。
// 分支二（UTF-8 BOM EF BB BF）：同形（无 buf）。
// 分支三（UTF-16LE BOM FF FE）：ORIG 直接 WideString(data+2,(size-2)/2)
// （wchar_t* 别名字节借用，i386 wchar=4B；P171 照抄，不做大小端重组）。
// 默认分支：具名 CharString(data,size) 临时 + op= + 具析构。
// 尾部统一（ORIG @0x3e3）：copyBuf=new char[content.length()+1];
// memcpy(copyBuf, content.c_str(), length+1); loadFromString(copyBuf,
// content.length(), error)（length 共调用 3 次，P48 无 CSE）;
// if (copyBuf) delete[] copyBuf; 返回结果 byte 槽聚合。
// ---------------------------------------------------------------------------
// [b28d-solo-StringResource N(N97/N82/N95): (size+3)/4*4 的 4.1 sar/shr/lea 有符号 lowering
// vs 4.4 lea/test/cmovs（N97）+ 标量槽基深 4（ORIG origSize -0x2c 起，4.4 -0x30 起，
// 帧同 0x60；未使用局部 pad 反致整体再深 4）+ XOR 循环地址重算（P205/P183）；
// 361/363 gap −2，p167 CLEAR 42=42，语义完成 | ORIG 0x08ad4e04]
// [M2S5StringRaw 2026-09-11 复核] gap −2 = N97（(size+3)/4*4 的 ORIG `sar/shr/lea`
// 有符号 lowering vs 4.4 `lea/test/cmovs`）+ N18a（XOR 循环 ORIG `shl $2,%eax;
// add <D>(%ebp),%eax` vs 4.4 `lea (%eax,%edx,1)` 地址折叠），均 4.4 指令选择，
// P329/P231/P226 无对应源形可翻 ⇒ 封顶维持。
#include "StringResource/loadFromBinary.inc"
{
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N18a/N97）→ 试=④P226/N97：
    //   `payloadBytes=(size+3)/4*4` 的 ORIG 形 `mov %eax,%edx; sar $0x1f,%edx; shr $0x1e,%edx;
    //   lea (%edx,%eax,1),%eax; sar $2; shl $2`（4.1 expand_divmod）vs 4.4
    //   `lea 0x3(%eax),%edx; test %eax,%eax; cmovs %edx,%eax; sar $2; shl $2`；显式位运算
    //   展开需引两次 `(size+3)`（净 +3 条、且 ORIG 的 sar/shr 是作用在已 +3 的 %eax 上）⇒
    //   反劣；XOR 循环 `shl $2; add <D>(%ebp)` vs 4.4 `lea (%eax,%edx,1)` 折叠同属指令选择。
    //   ②prett ③strall CLEAR。⇒ 维持 N97/N18a。
    CharString content;                              // -0x48

    if (size > 4 && data[0] == 'S' && data[1] == 'C' &&
        data[2] == '0' && data[3] == '1')
    {
        int origSize = *reinterpret_cast<const int*>(data + 4);     // -0x2c
        int payloadBytes = (size + 3) / 4 * 4;                      // -0x28
        unsigned int k0 = 0x5819af17u;                              // -0x24
        char* buf = new char[payloadBytes];                         // -0x20
        memcpy(buf, data + 8, size);

        // [N1F 2026-09-11 实测复核] 该未用 pad 局部是**必需**的：删除后本函数槽基
        //   整体上浮 4（ORIG content -0x48 vs 我方 -0x44），归一 diff 208 行 → 398 行
        //   ⇒ 该 pad 净收益为正，保留（N97/N18a 残余本身仍封顶）。
        int slotPad;
        char* cur = buf;                                            // -0x18
        unsigned int key = k0;                                      // -0x14
        for (unsigned int i = 0; i < (unsigned int)payloadBytes / 4; ++i)
        {
            *reinterpret_cast<unsigned int*>(cur + i * 4) =
                *reinterpret_cast<unsigned int*>(cur + i * 4) ^ key;
            key = key ^ *reinterpret_cast<unsigned int*>(cur + i * 4);
        }

        WideString ws;
        bool converted = convertFromUtf8(buf, origSize, &ws);
        delete[] buf;                        // ORIG @0x12b 单对判空守卫为
                                             // 编译器隐式形（P141），勿再显式
        if (!converted)
            return false;
        content = convertToCharString(ws);
    }
    // BOM 常量用无符号立即数值（ORIG cmp $0xef/$0xbb/$0xbf,%al）：
    // '\xef' 等 char 字面量为负值，与 uchar 提升值恒不等，4.4 折叠死分支。
    else if (size > 3 && data[0] == 0xef &&
             data[1] == 0xbb && data[2] == 0xbf)
    {
        WideString ws;
        bool converted = convertFromUtf8(
            reinterpret_cast<const char*>(data + 3), size - 3, &ws);
        if (!converted)
            return false;
        content = convertToCharString(ws);
    }
    else if (size > 2 && data[0] == 0xff && data[1] == 0xfe)
    {
        WideString ws(reinterpret_cast<const wchar_t*>(data + 2),
                      (size - 2) / 2);
        content = convertToCharString(ws);
    }
    else
    {
        CharString tmp(reinterpret_cast<const char*>(data), size);
        content = tmp;
    }

    char* copyBuf = new char[content.length() + 1];                 // -0x34
    memcpy(copyBuf, content.c_str(), content.length() + 1);
    bool result = loadFromString(copyBuf, content.length(), error); // -0x2d
    delete[] copyBuf;                    // 同上 P141：隐式单守卫
    return result;
}

// ---------------------------------------------------------------------------
// loadFromFile —— ORIG T 0x08ad4ce4：fopen(path,"rb")（模式串 @0x8e32c38）；
// 失败且 errOut!=0 → *errOut="Cannot open file"@0x8e32c3b 并返回 0。
// 成功：fseek end/ftell/回 0 → fread 整体 → fclose →
// loadFromBinary(buf,size,err) → delete[]，透传结果（size<0 失败返 0）。
// ---------------------------------------------------------------------------
// [b28d-solo-StringResource N80/P75: loadFromBinary 三实参 ORIG 右→左全 %eax 串行
// （error→0xc, fileSize→0x8, buf→0x4, this→0x0）、4.4 左→右先取 buf 入 %eax、arg2/3 走 %edx；
// A(int fileSize)/B(具名 binData)/C((int)cast)/D(B+C) 四变体实测均劣（具名形多 2 条且槽漂）
// | ORIG 0x08ad4ce4]
// [M2S5StringRaw 2026-09-11 复核] 尾部 loadFromBinary 3 行装载序（ORIG 右→左全
// %eax 串联；4.4 arg0 预载 %eax、arg2/arg3 走 %edx）：本波追加新钥匙探针
// /tmp/probeF1（现行形）/F2（P226 语句表达式 `({ ... })`）实测两形逐条相同
// ⇒ N80 封顶维持（旧 5 形见 /tmp/p26_lf*.cpp）。
#include "StringResource/loadFromFile.inc"
{
    FILE* fp = fopen(path, "rb");
    if (fp == NULL)
    {
        if (error)
            *error = "Cannot open file";
        return false;
    }
    fseek(fp, 0, SEEK_END);
    const long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (fileSize < 0)
    {
        fclose(fp);
        return false;
    }
    // ORIG 各使用点直接读 ftell 槽（-0x14）：无具名 int 中间量（P183）。
    // [N1F 2026-09-11] 缓冲区用 unsigned char*（ORIG 0x08ad4dbf 实参装载形：err→0xc/
    //   size→0x8/buf→0x4/this→0x0 逐条 `mov <src>,%eax; mov %eax,N(%esp)`，**无** cast
    //   节点的预取；带 reinterpret_cast 时 4.4 把 buf 预载入 %eax 并改走 %edx ⇒ 形态不符）。
    unsigned char* buf = new unsigned char[fileSize];
    fread(buf, 1, (size_t)fileSize, fp);
    fclose(fp);

    /* loadFromBinary 实参装载 N80 族（b26 NL26 旧结论「恒交错」已被上条重定类型推翻）：
     * 去 cast 后 4.4 逐条 `mov %eax` 直落，与 ORIG 同形。 */
    const bool result = loadFromBinary(buf, fileSize, error);
    delete[] buf;
    return result;
}

// ---------------------------------------------------------------------------
// app::stringResource —— ORIG T 0x08ad5439：guard 单例 + atexit 析构注册。
// C++98 局部静态生命周期与 ORIG guard/atexit 组合等价。
// ---------------------------------------------------------------------------
namespace app {

StringResource* stringResource()
{
    static StringResource sr;
    return &sr;
}

}  // namespace app

// STRING_FILE —— ORIG 单元内静态 _ZL11STRING_FILE（0x093f567c），恒 0。
static const char* STRING_FILE = 0;

// ---------------------------------------------------------------------------
// loadString —— ORIG T 0x08ac35cc：stringResource()->loadFromFile(path,err)。
// ---------------------------------------------------------------------------
#include "StringResource/loadString.inc"
{
    return app::stringResource()->loadFromFile(STRING_FILE, error);
}

// ---------------------------------------------------------------------------
// 编码原语。
//
// b28c-L8：ORIG _Z19convertToCharStringPKw @0x08adf54c 体为
// `mov &_ZL9gCodePage,%edx; mov %edx,0x8(%esp)`（内存读传参，非立即数）
// ⇒ 本 TU 有单元内 static 码页全局（与 StringConvert.cpp/GameScriptEnumHelpers.cpp
// 的同名 TU-local 各自独立，链接期不冲突）。旧形写常量 0 使真码页变更失效。
// ---------------------------------------------------------------------------
// convertToCharString(PKwj) —— ORIG T 0x08adf57f：wcstombs(NULL,src,0) 求长；
// 返回 (size_t)-1 或 0 → 返回空 CharString；否则 CharStringData::create(n)
// 缓冲内 wcstombs 写入并以 CharString(CharStringData*) 归还。
// convertToCharString(PKw)  —— ORIG T 0x08adf54c：转发 PKwj（ORIG 经单元内
// 码页全局传参，非立即数）。
// convertToCharString(RK10WideString) —— ORIG T 0x08adf624：sret，转发 c_str()。
// ---------------------------------------------------------------------------
// [b28d-Main R3 迁移·检查点13] gCodePage + convertToCharString(PKwj/PKw/RK10WideString)
// 已迁入 StringConvert.cpp（ORIG 归属 TU）；本 TU 不再持有码页副本。

// ---------------------------------------------------------------------------
// convertFromUtf8 —— ORIG T 0x08adf893。两遍式 UTF-8→wchar_t：pass1 统计宽
// 【验收 batch-11】orig=161 / our=162（gap −1）：修正旧实现多出的 4 字节分支
// （ORIG >0xef 一律 return false，@0x51-0x71 无第 4 档，此前语义偏差已消除）；
// ASCII 首块 js 直跳形、合成段 add 非 or、(unsigned short)(b<<12) 两段形均已
// 对齐。残 1 行 = ws 析构块 EH 聚合布局（4.1 共享 dtor tail vs 4.4 散布
// return 岛，跨版本负例族），封顶勿再追。
// 字符数（ASCII(js 直跳) / c<=0xdf&&i+1<len:2 / c<=0xef&&i+2<len:3 / 其余
// ——含 4 字节序列与截断——一律 return false，ORIG 无 4 字节分支 @0x51-0x71）；
// count==0 → *out=L""（operator=(const wchar_t*)）返 1；否则 createTerminated
// + getBuffer 得 wchar_t* 游标（*dst++，ORIG addl $4,-0x1c 形态）逐字合成，
// 具名 WideString tmp(data) + op= + 具析构后返 1。合成式与 ORIG 逐指令：
// 2 字节 (c&0x1f)<<6 | (signed)src[i+1]&0x3f；3 字节
// (unsigned short)(c<<12)（shl $0xc; movzwl %ax 两段形）| (b1&0x3f)<<6 | (b2&0x3f)。
// ---------------------------------------------------------------------------
// [b28d-solo-StringResource N(N82/N93): 槽位分配差（wideCount ORIG -0x24 vs 4.4 -0x1c、
// c ORIG -0x11 vs 4.4 -0xa；帧同 0x30，i 同 -0x18）+ 返回物化 %eax/%ebx；
// 161/162 gap −1，p167 CLEAR 8=8，语义完成 | ORIG 0x08adf893]
// [M2S5StringRaw 2026-09-11 复核] gap −1 的两条真因已定位并修复（见函数体内两处
// [M2S5StringRaw] 注释）：① 第二循环复用 i 令 4.4 压缩槽基；② tmp 未用显式块
// 令 4.4 把返回 1 提前物化到 %ebx。探针 /tmp/probeC2（4.4.7 默认路由）实测
// 161=ORIG 且 /tmp/seqcmp.py 归一化序列逐条全同（旧形 162）。
#include "StringResource/convertFromUtf8.inc"
{
    int wideCount = 0;                        // -0x24
    int i = 0;                                // -0x18
    while (i < srcLength)
    {
        const unsigned char b = (unsigned char)src[i];
        // ORIG 块排布：ASCII 为 fall-through 首块（test;js→多字节区 @0xf8）。
        if ((char)b >= 0)
        {
            wideCount += 1;
            i += 1;
        }
        else
        {
            if (b <= 0xdf && i + 1 < srcLength)
    {
            wideCount += 1;
            i += 2;
        }
            else if (b <= 0xef && i + 2 < srcLength)
        {
            wideCount += 1;
            i += 3;
        }
        else
                return false;
        }
    }
    if (wideCount == 0)
    {
        *out = L"";                           // ORIG operator=(wchar_t const*)
        return true;
    }

    WideStringData* data = WideStringData::createTerminated(wideCount);
    wchar_t* dst = data->getBuffer();
    // [M2S5StringRaw 2026-09-11] ORIG -0x10：第二循环是**独立具名局部**（i 槽
    // -0x18 不再被重置复用）。旧形 `i = 0` 令 4.4 复用 -0x18 槽并把整段槽基
    // 压缩 4（wideCount -0x1c vs ORIG -0x24…）。探针 /tmp/probeC1 亦证。
    int j = 0;
    while (j < srcLength)
    {
        const unsigned char b = (unsigned char)src[j];
        // 同上：ASCII 首块 + js 直跳多字节区；合成段间用加法（ORIG add
        // %eax,%edx，4.1 对无重叠位 or 发射 add；4.4 的 | 直出 or）。
        if ((char)b >= 0)
        {
            *dst++ = (wchar_t)b;
            j += 1;
        }
        else
        {
            if (b <= 0xdf && j + 1 < srcLength)
    {
                *dst++ = (wchar_t)(((b & 0x1f) << 6) +
                                   ((int)src[j + 1] & 0x3f));
            j += 2;
        }
            else if (b <= 0xef && j + 2 < srcLength)
        {
                *dst++ = (wchar_t)((unsigned short)(b << 12) +
                                   (((int)src[j + 1] & 0x3f) << 6) +
                                   ((int)src[j + 2] & 0x3f));
            j += 3;
        }
        else
                return false;
        }
    }
    // [M2S5StringRaw 2026-09-11] ORIG 0x08adfa5c `jmp 0x8adfa79`（tmp 析构块）
    // ⇒ tmp 的作用域先于 return 结束（显式块）：4.4 因此不再把返回 1 提前
    // 物化到 callee-saved %ebx（旧形多 `mov $1,%ebx` + 尾 `mov %ebx,%eax`）。
    // 探针 /tmp/probeC2/Game/StringResource.cpp 实测 161=ORIG 且归一化序列
    // 逐条全同（/tmp/seqcmp.py）。
    {
        WideString tmp(data);
        *out = tmp;
    }
    return true;
}

// ---------------------------------------------------------------------------
// app::getStr —— ORIG T 0x08ad54cc：get(id) 命中 → 返回表项拷贝；
// 未命中 → CharString::format("getStr(%d) == none", id)。
// ---------------------------------------------------------------------------
// [b28d-solo-StringResource AE: 32=32 逐条全等（CharString::format 静态 sret 化后；旧
// 非静态形 32/55 系共享头签名错） | ORIG 0x08ad54cc]
#include "StringResource/getStr.inc"
{
    const CharString* entry;        // ORIG 无初始化槽（@0x8ad54d6 直接取址传参）
    if (app::stringResource()->get(id, &entry))
        return *entry;
    // ORIG 0x08ad5508：静态 sret 直写返回对象，无局部 C1Ev/无拷贝。
    return CharString::format("getStr(%d) == none", id);
}

// ---------------------------------------------------------------------------
// CStringManager::getStr —— ORIG T 0x08ac35f8。
// 【验收 batch-11 尾项】orig=66 / our=66（gap 0）：errorMsg 改回 ORIG 的
// `static __thread char[32]`（%gs+lea 两段重算、无中间槽）+ ss_sprintf<32>
// 模板（W 定义/force 归 OnlyOrigBatch12.cpp，本 TU §8.1 声明）；is_sr_loaded
// 仅成功路径写位（旧实现无条件 `= loaded` 已纠正）；loaded 具名 bool 跨 err
// 析构存活。残差=4.1 把 loaded/return 聚合于 callee-saved %esi/%ebx 而 4.4
// 用栈槽（N82/N7 族），助记符行集等长。
// is_sr_loaded（ORIG _ZL12is_sr_loaded @0x0950189a）未置位时先加载一次；
// 加载失败返回 NULL（ORIG ebx=0 路径）；命中取表项 c_str()；未命中 sprintf
// "getStr(%d) == none"（格式串 @0x8e2e793）到静态 errorMsg 缓冲并返回。
// errorMsg 对应 ORIG getStr 内静态缓冲（bss 块 0x508）。
// ---------------------------------------------------------------------------
static bool is_sr_loaded = false;

// 模板版 ss_sprintf<N>（ORIG W _Z10ss_sprintfILj32EEiRAT__cPKcz 等）：定义与
// force 实例化归 OnlyOrigBatch12.cpp（属主批次），本 TU 按 §8.1 仅持等价声明，
// 链接期绑定其 W 实例。签名与 mangled 逐字对齐（RAT__c = char(&)[N]）。
template <size_t N>
#include "StringResource/ss_sprintf.inc"

// [b28d-solo-StringResource AE(源码 66=66 逐条全等): 唯一残差 = compare_tu 的 TLS_LE_LEA_RE
// 只归一「源/目标同寄存器」形，ORIG `lea -0x3c048(%eax),%ebx`（eax→ebx）未归一而 OURS
// `lea <A>(%eax),%ebx` 已归一 ⇒ 假 NEAR；已上报 Main 放宽该正则 | ORIG 0x08ac35f8]
#include "StringResource/getStr_1.inc"
{
    // ORIG _ZZN14CStringManager6getStrEiE8errorMsg：bss 0x20 字节 TLS 形式
    // 访问（asm `mov %gs:0x0,%eax; lea disp(%eax)` 两段、参数位与 return 各
    // 重算一次——__thread 局部静态，无共享槽缓存，P183）。
    static __thread char errorMsg[32];
    if (!is_sr_loaded)
    {
        // ORIG @0x30-0x48 双岛：return 0 在 err 作用域内 → 失败路径与正常路径
        // 合流到共享析构 cleanup（ebx=0/esi=0 vs esi=1，test %esi 决定走向）。
        {
            CharString err;
            if (loadString(&err))
            {
                is_sr_loaded = true;
            }
            else
                return 0;
        }
    }

    const CharString* entry;              // ORIG 无初始化（直接取址传参）
    if (app::stringResource()->get(id, &entry))
        return const_cast<char*>(entry->c_str());

    ss_sprintf(errorMsg, "getStr(%d) == none", id);
    return errorMsg;
}

// ---------------------------------------------------------------------------
// CStringManager::setStringResourceLoaded —— ORIG T 0x08ac36d2：置位即返回。
// ---------------------------------------------------------------------------
#include "StringResource/setStringResourceLoaded.inc"
{
    is_sr_loaded = true;
}
