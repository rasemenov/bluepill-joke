#include "cmds.h"
#include "serial.h"
#include "timer.h"
#include "../utils/stm_snprintf.h"

#include <stdint.h>
#include <string.h>


static void cli_help(int argc, char *argv[]);
static void cli_time(int argc, char *argv[]);

struct cmd_t commands[] = {
    {"help", "Print available commands and exit", &cli_help},
    {"time", "Show current ticks counter", &cli_time},
};


static void cli_help(int argc, char *argv[]) {
    for (int indx = 0; indx < ARRAY_LENGTH(commands); indx++) {
        const char *desc = commands[indx].desc;
        if (desc) {
            uart_put_line(desc);
        }
    }
}

static void cli_time(int argc, char *argv[]) {
    char buf[32] = {0};
    uint32_t ticks = get_systick();
    int ret = snprintf(buf, sizeof(buf) - 1, "Ticks %u", ticks);
    if (ret <= 0 || ret >= sizeof(buf)) {
        return;
    }
    uart_put_line(buf);
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
