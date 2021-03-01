#ifndef CMDS_H
#define CMDS_H

#define CMD_NAME_SIZE 16

struct cmd_t {
    const char name[CMD_NAME_SIZE];
    const char desc[64];
    void (*handler)(int argc, char *argv[]);
};

void process_cmd(int argc, char *argv[]);


#endif
