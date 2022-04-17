#ifndef CMDS_H
#define CMDS_H

#define CMD_NAME_SIZE 16
#define CMD_DESCRIPTION_SIZE 64
#define UNUSED(x) (void)(x)

struct cmd_t {
    const char name[CMD_NAME_SIZE];
    const char desc[CMD_DESCRIPTION_SIZE];
    void (*handler)(int argc, char *argv[]);
};

void process_cmd(int argc, char *argv[]);


#endif
