#include "cmds.h"
#include "serial.h"
#include "timer.h"
#include "utils/stm_snprintf.h"

#include <stdint.h>
#include <string.h>


static void cli_help(int argc, char *argv[]);
static void cli_time(int argc, char *argv[]);
static void cli_crash(int argc, char *argv[]);

struct cmd_t commands[] = {
    {"help", "Print available commands and exit", cli_help},
    {"time", "Show current ticks counter", cli_time},
    {"crash", "Crash application by de-referencing NULL pointer", cli_crash},
};


static void cli_help(int argc, char *argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    char buf[128] ={0};
    for (int indx = 0; indx < ARRAY_LENGTH(commands); indx++) {
        const char *desc = commands[indx].desc;
        snprintf(buf, sizeof(buf), "%-10.10s:\t%-64.64s", commands[indx].name,
                 desc ? desc : "");
        uart_put_line(buf);
    }
}


static void cli_time(int argc, char *argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    char buf[32] = {0};
    uint32_t ticks = get_systick();
    int ret = snprintf(buf, sizeof(buf) - 1, "Ticks %u", ticks);
    if (ret <= 0 || ret >= (int)sizeof(buf)) {
        return;
    }
    uart_put_line(buf);
}


static void cli_crash(int argc, char *argv[]) {
    UNUSED(argc);
    UNUSED(argv);
    uart_put_line("Application is going to crash!");
    volatile char test = *(char *)0;
    UNUSED(test);
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
