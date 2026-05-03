#ifndef COMMAND_H
#define COMMAND_H

typedef void (*cmd_handler_t)(int argc, char *argv[]);

typedef struct {
    const char *name;
    cmd_handler_t handler;
} cmd_t;

// Command handlers (defined in main.c)
extern void cmd_help(int argc, char *argv[]);
extern void cmd_motor(int argc, char *argv[]);
extern void cmd_temp(int argc, char *argv[]);

void cmd_init(void);

#endif
