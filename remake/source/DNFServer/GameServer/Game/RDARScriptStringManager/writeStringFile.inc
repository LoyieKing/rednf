bool RDARScriptStringManager::writeStringFile(int table)
{
    // ORIG 0x8aa50bc：写 "Script/<文件名>"，逐条 "key>value\n"，value 内 '\n' 转义
    // 为 "\\n"。ORIG 对 GetFileName 结果无判空（P171 照抄）。
    char path[512];
    ss_sprintf(path, "Script/%s", fileList_.GetFileName(table));
    std::FILE* fp = std::fopen(path, "w");
    // ORIG 0x8aa510b：fp 槽比零物化 `setne %al; test %al,%al; je <尾 mov $0>`
    // ⇒ while+return 形（P225/P194）。
    while (fp != 0)
    {
        if (static_cast<unsigned>(table) < dictionaries_.size())
        {
            // ORIG -0x28 槽存 operator[] 引用地址（P157/P183），循环尾每轮重调
            // getCount（无 n 具名局部）。
            StringDictionary* const& d = dictionaries_[table];
            for (int i = 0; i < d->getCount(); ++i)
            {
                const char* key = d->getKey(i);
                char buf[1024];
                // ORIG：getValue 结果直接作 strcpy 第 2 实参（无三目/无具名 value）。
                std::strcpy(buf, d->getValue(i));

                if (key != 0)
                {
                    // '\n' → "\\n"：从 '\n' 处右移剩余串，插入 '\' 和 'n'。
                    for (char* p = buf; *p != '\0'; ++p)
                    {
                        if (*p == '\n')
                        {
                            // ORIG 槽深（浅→深）q(-0x10) < r(-0x14) < len(-0x18)
                            // ⇒ 声明序取反序（后声明浅，P212 槽向）。
                            int len = 1;
                            char* r;
                            char* q = p + 1;
                            while (*q != '\0')
                            {
                                ++len;
                                ++q;
                            }
                            r = p + len;
                            // ORIG 0x5200：增量段先 `subl $1,cnt` 后 `subl $1,r`
                            // ⇒ 逗号增量形（--cnt, --r）。
                            for (int cnt = len; cnt >= 0; --cnt, --r)
                            {
                                r[1] = *r;
                            }
                            *p = '\\';
                            ++p;
                            *p = 'n';
                        }
                    }
                    std::fprintf(fp, "%s>%s\n", key, buf);
                }
            }
        }
        std::fclose(fp);
        return true;
    }
    return false;
