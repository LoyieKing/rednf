// Auto-generated header stub from DWARF info
// Original path: output/formmog/secagent/release/include/secagent_game_exe.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_GAME_EXE_H_H_
#define SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_GAME_EXE_H_H_

// sizeof = 20971528
struct SecagentGameExe { // line 13
public:
static const unsigned int MAX_EXEDATA_LEN;
unsigned int version_;
unsigned int exe_data_size_;
unsigned char exe_data_[];
SecagentGameExe(); // line 15
~SecagentGameExe(); // line 16
int load(const char *arg0); // line 19
int load_version(const char *arg0); // line 20
const unsigned int get_version() const; // line 21
};
extern const unsigned int MAX_EXEDATA_LEN; // line 27


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_OUTPUT_FORMMOG_SECAGENT_RELEASE_INCLUDE_SECAGENT_GAME_EXE_H_H_
