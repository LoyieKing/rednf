// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Script/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include "Script.h"

namespace nsl {

#include "Script/Script.inc"
    : tok(), mLoadScriptLock(), mParentNames(), mChildNames(), mChildValues()
{
}

#include "Script/dtor_Script.inc"
{
}

#include "Script/load.inc"
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(mLoadScriptLock);
    char buf[0x401];
    memset(buf, 0, 0x401);
    Clear();
    depth = 0;
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        return false;
    }
    bool ret;
    for (;;)
    {
        ret = fgetln(fp, buf);
        parse(buf);
        if (!(ret == true))
        {
            break;
        }
    }
    fclose(fp);
    return true;
}

#include "Script/fgetln.inc"
{
    int i = 0;
    while (i <= 0x3ff)
    {
        int code = fgetc(fp);
        if (feof(fp) != 0)
        {
            buf[i] = '\0';
            return false;
        }
        buf[i] = (char)code;
        if (buf[i] == '\n')
        {
            buf[i] = '\0';
            if ((i > 0) && (buf[i - 1] == '\r'))
            {
                buf[i - 1] = '\0';
                return true;
            }
            return true;
        }
        i = i + 1;
    }
    return true;
}

#include "Script/parse.inc"
{
    remove_comment(line);
    if (!get_key_val(line))
    {
        puts("\xC5\xE4\xC5\xAB\xC0\xBB \xBE\xF2\xBE\xEE \xBF\xC0\xB4\xC2\xB5\xA5 \xBD\xC7\xC6\xD0 \xC7\xCF\xBF\xB4\xBD\xBF");
        exit(1);
    }
    return true;
}

#include "Script/remove_comment.inc"
{
    int i = 0;
    while (i <= 0x3fd)
    {
        int n = 0;
        for (int j = 0; j < 2; j = j + 1)
        {
            if (line[i + j] == "//"[j])
            {
                n = n + 1;
            }
        }
        if (n == 2)
        {
            memset(line + i, 0, 0x400 - i);
            return true;
        }
        i = i + 1;
    }
    return false;
}

#include "Script/get_key_val.inc"
{
    tok.get_token(line, strlen(line));
    int n_tok = tok.get_n_token();
    switch (n_tok)
    {
    case 1:
        mParentNames.push_back(std::string(tok.get_context(0)));
        mChildNames.push_back(std::vector<std::string>());
        mChildValues.push_back(std::vector<std::string>());
        tok.free_token(0);
        break;
    case 2:
        mChildNames[mParentNames.size() - 1].push_back(std::string(tok.get_context(0)));
        mChildValues[mParentNames.size() - 1].push_back(std::string(tok.get_context(1)));
        tok.free_token(0);
        tok.free_token(1);
        break;
    case 0:
        break;
    default:
        return false;
    }
    return true;
}

#include "Script/get_sections.inc"
{
    const char* p = 0;
    unsigned int i = 0;
    for (; i < mParentNames.size(); i = i + 1)
    {
        unsigned int j = 0;
        for (; j < mChildNames[i].size(); j = j + 1)
        {
            p = mChildValues[i][j].c_str();
            if (p == 0)
            {
                return false;
            }
        }
    }
    return true;
}

#include "Script/Clear.inc"
{
    mParentNames.clear();
    mChildNames.clear();
    mChildValues.clear();
}

} // namespace nsl
