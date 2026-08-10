// Minimal rebuild for zergsvr reconstruction builds (layout per zergsvr DWARF):
//   mml_string_@0 mml_command_@4 mml_cmd_option_(set<string,LessofCaseString>)@8
//   mml_cmd_parameter_(map<string,string,LessofCaseString>)@32  sizeof = 56
// Original path: src/commlib/framework/comm_mml_command.h
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_MML_COMMAND_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_MML_COMMAND_H_H_

#include <map>
#include <set>
#include <string>

// sizeof = 1
struct LessofCaseString { // line 15
public:
    bool operator()(const std::string &arg0, const std::string &arg1) const {
        return strcasecmp(arg0.c_str(), arg1.c_str()) < 0;
    }
};

// sizeof = 56
struct MML_Console_Command { // line 33
protected:
    std::string mml_string_;                                        // 0x00
    std::string mml_command_;                                       // 0x04
    std::set<std::string, LessofCaseString> mml_cmd_option_;        // 0x08
    std::map<std::string, std::string, LessofCaseString> mml_cmd_parameter_; // 0x20
public:
    MML_Console_Command(const char *arg0); // line 53
    MML_Console_Command(); // line 54
    ~MML_Console_Command(); // line 55
    void GetMMLString(std::string &mml_string) const; // line 60
    const char * GetMMLString() const; // line 61
    int GetMMLCommand(std::string &mml_cmd) const; // line 64
    const char * GetMMLCommand() const; // line 65
    int IsHaveCmdOption(const std::string &mml_option) const; // line 68
    int GetCmdParameter(const std::string &para_key, std::string &para_value) const; // line 71
    int GetCmdParameter(const std::string &arg0, char *arg1) const; // line 73
    int ParseMMLCommandString(const char *mml_cmd_string); // line 79
    int ParseMMLCommandString2(const char *mml_cmd_string); // line 82
};

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_MML_COMMAND_H_H_
