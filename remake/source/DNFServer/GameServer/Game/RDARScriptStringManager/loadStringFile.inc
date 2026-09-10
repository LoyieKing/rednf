bool RDARScriptStringManager::loadStringFile(std::istream* in, const char* path,
                                             int /*table*/,
                                             std::vector<ErrorLog>* err)
{
    // ORIG 0x8aa529e：① skip 是表达式短路物化（`je L;je L;call bad;test;je L2`
    // + `mov $1/$0,%eax`，无具名 bool 槽，P0）；② auto_ptr 默认构造后 reset
    // （ORIG 0x52ec `auto_ptr(0)` → new → C1 → reset）；③ isJpn/isKor 为
    // 「预清 + 条件置位」（P194）；④ ErrorLog 第三字段 = lineNo（e.m_flag）；
    // ⑤ 重复检测先 getKey(i+1) 后 getKey(i)、无判空、第三处为
    // `e.m_detail = getKey(i)`（ORIG `add $0x8,%eax` + operator=，非 +=）。
    if (in == 0 || path == 0 || in->bad())
        return false;

    std::auto_ptr<StringDictionary> dict;
    dict.reset(new StringDictionary());

    bool isJpn = false;                                          // local_32
    if (std::strstr(path, "Creature.jpn.str") != 0)
        isJpn = true;
    bool isKor = false;                                          // local_31
    if (std::strcmp(path, "Etc/Etc.kor.str") == 0)
        isKor = true;

    int lineNo = 0;
    char lineBuf[0x2928];
    int r;

    while (!in->eof())
    {
        ++lineNo;
        in->getline(lineBuf, 0x2928);

        char* key = 0;
        char* value = 0;
        r = parseStringLine(lineBuf, &key, &value);

        if (r == -2)
        {
            if (err != 0)
            {
                ErrorLog e;
                e.m_code = static_cast<int>(0x80000001);
                e.m_message = path;
                e.m_flag = lineNo;
                err->push_back(e);
            }
            return false;
        }
        else if (r == -1)
        {
            if (err != 0)
            {
                ErrorLog e;
                e.m_code = static_cast<int>(0x80000000);
                e.m_message = path;
                e.m_flag = lineNo;
                err->push_back(e);
            }
            return false;
        }

        if (key != 0 && value != 0)
        {
            if (isKor)
            {
                // [b29-Lane21] N78/P214③：块内 `char cpBuf[4000] = { 0 };` 令 4.4 内联
                // `lea; mov $0,%eax; mov $0x3e8,%edx; rep stos`（ORIG 0x0835… 形）；
                // 原函数级声明 + memset 恒发 `call memset`（多 4 条）。
                char cpBuf[4000] = { 0 };
                std::strcpy(cpBuf, value);
                CodePage::script2Database(value, cpBuf);
                value = cpBuf;
            }

            // ORIG 0x5535 `cmpb $0,isJpn; je <else>` ⇒ 显式 ==false（N122：!b 恒 xor 物化）
            if (isJpn == false)
            {
                dict->add(toTString(key), toTString(value));
            }
            else
            {
                // ORIG 0x5549 `test %al,%al; setne %al; test %al,%al; je <NULL 分支>`
                // ⇒ 正极性包裹（P214①）；toTString 结果无槽往返（P0 实参内联）
                if (*toTString(value) != '\0')
                    dict->add(toTString(key), toTString(value));
                else
                    dict->add(toTString(key), "NULL");
            }
        }
    }

    dict->freeze();

    // 重复连续 key 检测
    bool foundDuplicate = false;
    int count = dict->getCount() - 1;
    for (int i = 0; i < count; ++i)
    {
        // ORIG 0x564b：strcmp/printf 的 getKey 均无具名槽（P0 实参内联），
        // 第 2 实参（i+1）先求值。
        if (std::strcmp(dict->getKey(i), dict->getKey(i + 1)) == 0)
        {
            std::printf("%s, %s \n", dict->getKey(i), dict->getKey(i + 1));
            foundDuplicate = true;
            if (err != 0)
            {
                ErrorLog e;
                e.m_code = static_cast<int>(0x80000002);
                e.m_message = path;
                e.m_detail = dict->getKey(i);
                err->push_back(e);
            }
        }
    }

    if (!foundDuplicate)
    {
        dictionaries_.push_back(dict.release());
        return true;
    }
    return false;
