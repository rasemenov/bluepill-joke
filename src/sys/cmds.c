#include "cmds.h"
#include "serial.h"

#include <string.h>

static void cli_help(int argc, char *argv[]);

struct cmd_t commands[] = {
    {"help", "Print available commands and exit", &cli_help},
};


static void cli_help(int argc, char *argv[]) {
    for (int indx = 0; indx < ARRAY_LENGTH(commands); indx++) {
        const char *desc = commands[indx].desc;
        if (desc) {
            uart_put_line(desc);
        }
    }
}


void process_cmd(int argc, char *argv[]) {
    if (argc == 0 || !argv) {
        uart_put_line("skip");
        return;
    }
    if (strlen(argv[0]) == 0) {
        return;
    }
    const char *name = argv[0];
    for (int indx = 0; indx < ARRAY_LENGTH(commands); indx++) {
        struct cmd_t cmd = commands[indx];
        if (!strncmp(cmd.name, name, sizeof(cmd.name) - 1)) {
            cmd.handler(argc, argv);
            return;
        }
    }
    uart_put_line("No such command");
}
