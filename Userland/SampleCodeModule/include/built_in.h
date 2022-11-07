
#ifndef TP2_SO_BUILT_IN_H
#define TP2_SO_BUILT_IN_H
void wait_pid_command(uint64_t arg_c, const char ** arg_v);
void help(uint64_t arg_c, const char ** arg_v);
void mem(uint64_t arg_c, const char ** arg_v);
void ps(uint64_t arg_c, const char ** arg_v);
void loop(uint64_t arg_c, const char ** arg_v);
void kill(uint64_t arg_c, const char ** arg_v);
void nice_command(uint64_t arg_c, const char ** arg_v);
void block(uint64_t arg_c, const char ** arg_v);
void unblock(uint64_t arg_c, const char ** arg_v);
void sem(uint64_t arg_c, const char ** arg_v);
void cat(uint64_t arg_c, const char ** arg_v);
void wc(uint64_t arg_c, const char ** arg_v);
void filter(uint64_t arg_c, const char ** arg_v);
void pipe_info(uint64_t arg_c, const char ** arg_v);
void phylo(uint64_t arg_c, const char ** arg_v);
void write_pipe_name(uint64_t arg_c, const char ** arg_v);
void read_pipe_name(uint64_t arg_c, const char ** arg_v);
void write_pipe_common(uint64_t arg_c, const char ** arg_v);
void read_pipe_common(uint64_t arg_c, const char ** arg_v);

#endif //TP2_SO_BUILT_IN_H
