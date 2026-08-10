// Restored from zergsvr binary disassembly (2026-08-10).
// All 14 comm_mml_command.cpp symbols reconstructed:
//   MML_Console_Command(char const*) / () / ~(), GetMMLString x2, GetMMLCommand x2,
//   IsHaveCmdOption, GetCmdParameter, ParseMMLCommandString2 (line 91),
//   ParseMMLCommandString (line 226).
// Error codes observed in the oracle: option/parameter missing -> 0x13ab,
// empty/parse error -> 0x13ac.
#include "src/commlib/framework/comm_predefine.h"
#include <ctype.h>
#include <strings.h>
#include "src/commlib/framework/comm_mml_command.h"

using std::string;
using std::set;
using std::map;

// line 24
void MML_Console_Command::GetMMLString(string &mml_string) const {
    mml_string.assign(mml_string_);
}

// line 29
const char * MML_Console_Command::GetMMLString() const {
    return mml_string_.c_str();
}

// line 34
int MML_Console_Command::GetMMLCommand(string &mml_cmd) const {
    if (mml_command_.empty())
        return 0x13ab;
    mml_cmd.assign(mml_command_);
    return 0;
}

// line 45
const char * MML_Console_Command::GetMMLCommand() const {
    return mml_command_.c_str();
}

// line 51
int MML_Console_Command::IsHaveCmdOption(const string &mml_option) const {
    set<string, LessofCaseString>::const_iterator option_iter = mml_cmd_option_.find(mml_option);
    return option_iter == mml_cmd_option_.end() ? 0x13ab : 0;
}

// line 64
int MML_Console_Command::GetCmdParameter(const string &para_key, string &para_value) const {
    map<string, string, LessofCaseString>::const_iterator para_iter = mml_cmd_parameter_.find(para_key);
    if (para_iter == mml_cmd_parameter_.end())
        return 0x13ab;
    para_value.assign(para_iter->second);
    return 0;
}

// line 16
MML_Console_Command::MML_Console_Command() {
}

// line 9
MML_Console_Command::MML_Console_Command(const char *mml_cmd) {
    mml_string_.assign(mml_cmd, strlen(mml_cmd));
    mml_command_.reserve(0x20);
    ParseMMLCommandString(mml_cmd);
}

// line 19
MML_Console_Command::~MML_Console_Command() {
}

// line 91
int MML_Console_Command::ParseMMLCommandString2(const char *mml_cmd_string) {
    const char *part_start = mml_cmd_string;
    const char *part_end = part_start;

    mml_string_.assign(mml_cmd_string, strlen(mml_cmd_string));
    mml_command_.assign("", 0);
    mml_cmd_option_.clear();
    mml_cmd_parameter_.clear();

    while (isspace((unsigned char)*part_start)) {
        part_start++;
    }
    part_end = part_start;
    while (*part_end != 0 && !isspace((unsigned char)*part_end)) {
        part_end++;
    }
    if (part_end == part_start)
        return 0x13ac;
    mml_command_.replace(0, mml_command_.length(), part_start, part_end - part_start);

    part_start = part_end;
    while (isspace((unsigned char)*part_start)) {
        part_start++;
    }
    if (*part_start == 0)
        return 0;

    string key;
    string value;
    while (1) {
        if (*part_start == 0)
            return 0;
        while (isspace((unsigned char)*part_start)) {
            part_start++;
        }

        part_end = part_start;
        while (*part_end != 0 && *part_end != '=' && !isspace((unsigned char)*part_end)) {
            part_end++;
        }
        if (part_end == part_start)
            return 0x13ac;
        key.replace(0, key.length(), part_start, part_end - part_start);

        if (*part_end == 0) {
            mml_cmd_option_.insert(key);
            part_start = part_end;
            continue;
        }
        while (isspace((unsigned char)*part_end)) {
            if (*part_end == 0)
                break;
            part_end++;
        }
        if (*part_end != '=') {
            mml_cmd_option_.insert(key);
            part_start = part_end;
            continue;
        }

        part_start = part_end + 1;
        while (isspace((unsigned char)*part_start)) {
            if (*part_start == 0)
                break;
            part_start++;
        }
        if (*part_start == '"') {
            if (part_start[1] == 0 || part_start[1] == '"')
                return 0x13ac;
            part_end = part_start + 2;
            while (*part_end != '"' && *part_end != 0) {
                part_end++;
            }
            value.replace(0, value.length(), part_start + 1, part_end - (part_start + 1));
        } else {
            part_end = part_start;
            while (*part_end != 0 && !isspace((unsigned char)*part_end)) {
                part_end++;
            }
            if (part_end == part_start)
                return 0x13ac;
            value.replace(0, value.length(), part_start, part_end - part_start);
        }
        mml_cmd_parameter_[key] = value;
        while (isspace((unsigned char)*part_end)) {
            if (*part_end == 0)
                break;
            part_end++;
        }
        part_start = part_end;
    }
}

// line 226
int MML_Console_Command::ParseMMLCommandString(const char *mml_cmd_string) {
    const char CMD_SEPARATOR = ':';
    const char END_SEPARATOR = ';';
    const char OPTION_SEPARATOR = ',';
    const char KEY_SEPARATOR = '=';
    const char STRING_SEPARATOR = '"';
    const char *part_start = mml_cmd_string;
    const char *part_end = part_start;

    mml_string_.assign(mml_cmd_string, strlen(mml_cmd_string));
    mml_command_.assign("", 0);
    mml_cmd_option_.clear();
    mml_cmd_parameter_.clear();

    while (isspace((unsigned char)*part_start)) {
        part_start++;
    }
    part_end = part_start;
    while (*part_end != 0 && *part_end != CMD_SEPARATOR && *part_end != END_SEPARATOR) {
        part_end++;
    }
    if (part_end == part_start)
        return 0x13ac;
    mml_command_.replace(0, mml_command_.length(), part_start, part_end - part_start);

    if (*part_end == 0 || *part_end == END_SEPARATOR)
        return 0;
    part_start = part_end + 1;

    string key;
    string value;
    while (1) {
        while (isspace((unsigned char)*part_start)) {
            if (*part_start == 0 || *part_start == END_SEPARATOR)
                return 0;
            part_start++;
        }
        if (*part_start == 0 || *part_start == END_SEPARATOR)
            return 0;

        part_end = part_start;
        if (*part_end == OPTION_SEPARATOR || *part_end == KEY_SEPARATOR
                || *part_end == 0 || *part_end == END_SEPARATOR)
            return 0x13ac;
        while (*part_end != OPTION_SEPARATOR && *part_end != KEY_SEPARATOR
                && *part_end != 0 && *part_end != END_SEPARATOR) {
            part_end++;
        }
        while (part_end != part_start && isspace((unsigned char)*part_end)) {
            part_end--;
        }
        if (part_end == part_start)
            return 0x13ac;
        key.replace(0, key.length(), part_start, part_end - part_start);

        if (*part_end != KEY_SEPARATOR) {
            mml_cmd_option_.insert(key);
            part_start = part_end + 1;
            if (*part_start == 0 || *part_start == END_SEPARATOR)
                return 0;
            continue;
        }

        part_start = part_end + 1;
        while (isspace((unsigned char)*part_start)) {
            if (*part_start == 0 || *part_start == END_SEPARATOR)
                break;
            part_start++;
        }
        if (*part_start == STRING_SEPARATOR) {
            if (part_start[1] == 0 || part_start[1] == END_SEPARATOR)
                return 0x13ac;
            part_end = part_start + 1;
            while (*part_end != STRING_SEPARATOR && *part_end != 0
                    && *part_end != END_SEPARATOR) {
                part_end++;
            }
            if (*part_end != STRING_SEPARATOR)
                return 0x13ac;
            value.replace(0, value.length(), part_start + 1, part_end - (part_start + 1));
        } else {
            part_end = part_start;
            if (*part_end == OPTION_SEPARATOR || *part_end == 0
                    || *part_end == END_SEPARATOR)
                return 0x13ac;
            while (*part_end != OPTION_SEPARATOR && *part_end != 0
                    && *part_end != END_SEPARATOR) {
                part_end++;
            }
            while (part_end != part_start && isspace((unsigned char)*part_end)) {
                part_end--;
            }
            if (part_end == part_start)
                return 0x13ac;
            value.replace(0, value.length(), part_start, part_end - part_start);
        }
        mml_cmd_parameter_[key] = value;
        part_start = part_end + 1;
        if (*part_start == 0 || *part_start == END_SEPARATOR)
            return 0;
    }
}
