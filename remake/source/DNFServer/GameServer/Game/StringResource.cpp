// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 StringResource/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
}

#include "StringResource/set.inc"
}

#include "StringResource/get.inc"
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
#include "StringResource/loadFromString.inc"
}

// ---------------------------------------------------------------------------
// loadFromResource —— ORIG T 0x08ad4dfa（size 0x0a）：空函数（push ebp /
// mov $0,%eax / pop ebp / ret）——真实 ORIG 即恒返 false（资源型装载入口
// 在 ORIG 中未启用），转写为空实现。签名 (void*, int, CharString*)。
// ---------------------------------------------------------------------------
#include "StringResource/loadFromResource.inc"
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
#include "StringResource/loadFromBinary.inc"
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
#include "StringResource/loadFromFile.inc"
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
#include "StringResource/convertFromUtf8.inc"
}

// ---------------------------------------------------------------------------
// app::getStr —— ORIG T 0x08ad54cc：get(id) 命中 → 返回表项拷贝；
// 未命中 → CharString::format("getStr(%d) == none", id)。
// ---------------------------------------------------------------------------
// [b28d-solo-StringResource AE: 32=32 逐条全等（CharString::format 静态 sret 化后；旧
// 非静态形 32/55 系共享头签名错） | ORIG 0x08ad54cc]
#include "StringResource/getStr.inc"
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
#include "StringResource/getStr_1.inc"
}

// ---------------------------------------------------------------------------
// CStringManager::setStringResourceLoaded —— ORIG T 0x08ac36d2：置位即返回。
// ---------------------------------------------------------------------------
#include "StringResource/setStringResourceLoaded.inc"
}
