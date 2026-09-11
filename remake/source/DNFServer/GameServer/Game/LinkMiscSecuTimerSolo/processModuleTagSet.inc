// ===========================================================================
// processModuleTagSet  ORIG T 0x08a97637（RDARScriptSkill.cpp 模块标签域）
// 解析 "<module>"/"<sub>" 标签对：moduleId/subId 输出 + 前置 tag 消费。
// type 覆写为 3（ORIG 直接改参数）；标签表 MODULE_NAME_TAG[0x20]（开/闭对）。
// batch-12 逐条对位修正：
//  ① dungeon 分支 ORIG 797 `movzbl -0x15;xor$1;test;je tail` ⇔ subMatched==1
//     跳过赋值直落尾部（return 0），即 `if (!subMatched) {subId=3; log}`；
//     原实现条件反了。
//  ② 扫描子标签 j 循环：ORIG 861 循环体地址 `mov 0x8(%ebp); mov (%eax);
//     add;add1;shl2` 索引 = 2*moduleId+1（固定元素，逐 token 推进 *tag），
//     j（-0xc）仅作循环计数；原实现误用 2*j+1。
//  ③ 尾部消费条件 ORIG 900 `movzbl -0x1c(flag);xor$1;test;je ret1` ⇔
//     flag==0 才进 `if (moduleId != -1) ScanType`，即 `if (!flag && ...)`；
//     原实现条件反了。
//  ④ do 块：ORIG 83a foundSub 初始化仅一次，matched（-0xd）首次写入在
//     ScanType 之后（854），回边条件 `cmpb $0,-0xd; je 岛`（89f）；
//     for(;;)+尾部 if(matched)break 复现该形。
//  ⑤ 命中块第三与条件（tag 相等检查）写嵌套 if：4.4 对三目 && 链尾操作数
//     物化 bool 岛（+5），ORIG 718 为 `call;test;je` 直短路；
//     嵌套形消物化且块排布同 ORIG。
// 表型（Main 批准，batch-12）：MODULE_NAME_TAG 改 [17][2] pair 表（ORIG
// B 0x88 = 17 对，切片 `2*i` 寻址折 `shl $3` stride-8 实证），本函数以
// [i][0]/[i][1] 形寻址逐处对位。残留 N：pvp `type==9||type==5`
// ORIG 4.1 常量岛物化 test%al（N3，4.4 无合法写法）；73b/73e/741 空块
// nop;jmp 中继（跨版本 jump-threading，N）。
// ===========================================================================
// ORIG T 0x08a97637（mangled _Z19processModuleTagSetRiS_PSsib：int&,int& 形参）。
bool processModuleTagSet(int& moduleId, int& subId, std::string* tag,
                         int type, bool flag)
{
    type = 3;
    bool subMatched = false;
    moduleId = -1;
    subId = -1;
    int i = 0;
    for (; i <= 0x10; ++i)
    {
        if (*tag == MODULE_NAME_TAG[i][0] ||
            *tag == MODULE_NAME_TAG[i][1])
        {
            moduleId = i;
            subMatched = (*tag == MODULE_NAME_TAG[i][1]);
            if (i != type && type <= 0x10)
            {
                // 操作数序 [i] 左 [type] 右：4.4 二元比较 RHS 先求值 ⇒
                // type 槽先装载，与 ORIG 6f5-6fa（mov KE;shl3;lea/mov SE;shl3）
                // 行序一致。
                if (MODULE_NAME_TAG[i][0] == MODULE_NAME_TAG[type][0])
                    moduleId = type;
            }
            break;
        }
    }

    if (moduleId != -1)
    {
        if (moduleId == type)
        {
            if (!subMatched)
                subId = type;
        }
        else if (MODULE_NAME_TAG[moduleId][0] == "[dungeon]")
        {
            if (!subMatched)
            {
                subId = 3;
                LogManager::logFormat(0, "../RDARScriptSkill.cpp",
                                      __FUNCTION__, 0xc0,
                                      "skillscript separating : \xbc\xba\xb0\xf8 : \xb1\xe2\xba\xbb\xb0\xaa \xb8\xf0\xb5\xe2\xc5\xc2\xb1\xd7\xc0\xcc\xb9\xc7\xb7\xce \xc1\xf8\xc7\xe0 tag %s",
                                      tag->c_str());
            }
        }
        // ORIG 8a9780c..8a9782a：`call eq("[pvp]");test;je 8a97823;cmpl $9,type;
        // je 8a9781c;cmpl $5,type;jne 8a97823;8a9781c: mov $1,%eax;jmp 8a97828;
        // 8a97823: mov $0,%eax;8a97828: test %al,%al;je 8a9783a(else ScanType 段)`
        // ⇒ 源码是**单条** `if (tag=="[pvp]" && (type==9||type==5)) subId=4;`：
        // 整个 && 链在 4.1 里统一物化 bool 到 %al（**零栈槽**），且「pvp 命中但
        // type∉{9,5}」与「tag≠pvp」共用同一假出口 = **直接落 else(ScanType) 段**。
        // 原实现（嵌套 if + 具名 bool）双重错：a) 语义轴 —— pvp 命中而 type∉{9,5}
        // 时被 else-if 截住、**不执行 ScanType/foundSub 路径**（ORIG 会走 else）；
        // b) 4.4 对具名 bool 恒落槽 `mov %al,-0xf;cmpb $0,-0xf`（+1 条 ⇒ DIFF）。
        // 4.4 三形实测（探针 /tmp/p1/probe_bool3.cpp、probe_bool2.cpp）：
        // ①&& 链尾纯比较（本形）= ORIG 逐条同（`call;test;je;cmpl $9;je;cmpl $5;
        //   jne;mov $1,%eax;jmp;mov $0,%eax;test %al,%al;je`，零槽）；
        // ②嵌套 if + 具名 bool = +1 槽存取；③`register bool` = 改用 %ebx
        //   （push/mov/pop +3）；④裸 `if (type==9||type==5)` = 4.4 折成
        //   `cmpl $9;je;cmpl $5;jne`（208 条，gap −5）；⑤语句表达式/条件声明/
        //   `?:`/`==true`/`==false`/`|` 各族均折或改用 setcc。
        else if (MODULE_NAME_TAG[moduleId][0] == "[pvp]" &&
                 (type == 9 || type == 5))
        {
            subId = 4;
        }
        else
        {
            bool foundSub = false;
            bool matched;
            for (;;)
            {
                ScanType(*tag, false);
                matched = false;
                for (int j = 0; j <= 0x10; ++j)
                {
                    if (*tag == MODULE_NAME_TAG[moduleId][1])
                    {
                        foundSub = true;
                        matched = true;
                        break;
                    }
                }
                if (matched)
                    break;
            }
            if (!foundSub)
            {
                LogManager::logFormat(0, "../RDARScriptSkill.cpp",
                                      __FUNCTION__, 0xf3,
                                      "ERROR : skillscript separating : \xbf\xa1\xb7\xaf : \xc0\xdf\xb8\xf8\xb5\xc8 \xb8\xf0\xb5\xe2\xb1\xb8\xba\xd0\xc5\xc2\xb1\xd7 \xbb\xe7\xbf\xeb %s",
                                      tag->c_str());
            }
            return 0;
        }
    }

    if (subMatched)
        return 0;
    if (!flag && moduleId != -1)
        ScanType(*tag, false);
    return 1;
}