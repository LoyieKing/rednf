/* ==========================================================================
 * dnf_script.l — __dnf_script__FlexLexer 词法定义（描述性）。
 * --------------------------------------------------------------------------
 * 本文件供 source/toolchain/gen_dnflex_engine.sh 用 flex 2.5.35 生成
 * FlexLexerEngine.cpp 的引擎骨架（构造/析构/yy_create_buffer/yy_get_next_buffer/
 * yy_get_previous_state/yyinput/yyunput/yy_switch_to_buffer/yypush/pop_buffer_state/
 * yyensure_buffer_stack/yy_push_state/yy_pop_state/yy_top_state/switch_streams/
 * LexerInput/LexerOutput/LexerError/yywrap + 基类 yylex 桩）。
 *
 * 规则集仅需满足 flex 语法（≥1 条规则）；真实 DFA 跳转表（yy_accept/def/base/
 * chk/nxt/NUL_trans/rule_can_match_eol）的权威来源是 ORIG 二进制
 * （.rodata 0x8e2f180..0x8e30400，DNFLexCore.cpp 提供 g_yy_*），生成脚本将其
 * 替换；真正的 token/字符串/注释语义由 DNFLex::yylex（DNFLexCore.cpp）按 ORIG
 * 反汇编实现。因此本文规则为占位/文档性质。
 *
 * %option yyclass="__dnf_script__FlexLexer" 使生成的基类 yylex 成为
 * "invoked but %option yyclass used" 桩（ORIG 0x8ad1920，29 字节）——真实
 * DNFLex::yylex 覆写位于 DNFLexCore.cpp，共享同一组 g_yy_* 表。
 *
 * 版本判定：flex 2.5.35（2008，与 gcc 4.4 同期）。特征串
 *   "yyFlexLexer::yylex invoked but %option yyclass used"、yyensure_buffer_stack()、
 *   yy_create_buffer malloc(sizeof(yy_buffer_state))==0x30、YY_BUF_SIZE==0x4000、
 *   yywrap()==1、LexerError 写 std::cerr 且 exit(2)，与 ORIG 二进制逐一吻合。
 * ========================================================================== */

%option yyclass="__dnf_script__FlexLexer" yylineno

/* 起始条件：BEGIN(1)→yy_start=3、BEGIN(2)→yy_start=5、BEGIN(3)→yy_start=7。 */
%x C1 C2 C3

%{
#define YY_NO_UNISTD_H 1
%}

%%

	.	ECHO;

%%
