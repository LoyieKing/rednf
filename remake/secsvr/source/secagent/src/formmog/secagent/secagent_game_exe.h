// Auto-generated header stub from DWARF info
// Original path: src/formmog/secagent/secagent_game_exe.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_GAME_EXE_H_H_
#define SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_GAME_EXE_H_H_

// sizeof = 20971528
struct SecagentGameExe { // line 13
public:
SecagentGameExe() {} // line 15（内联，二进制无 out-of-line 符号）
~SecagentGameExe() {} // line 16
int load(const char *data_path); // line 19
int load_version(const char *version_cfg_path); // line 20
const unsigned int get_version() const { return version_; } // line 21
static const unsigned int MAX_EXEDATA_LEN = 0x1400000; // line 27
unsigned int version_; // line 29
unsigned int exe_data_size_; // line 31
unsigned char exe_data_[MAX_EXEDATA_LEN]; // line 33
};

#endif // SECSVR_SRC_FORMMOG_SECAGENT_SECAGENT_GAME_EXE_H_H_
