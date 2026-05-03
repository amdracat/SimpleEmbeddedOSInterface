#include "Command.h"
#include "os.h"
#include "TemperatureSensor.h"
#include "MotorCtrl.h"
#include "I2C.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_COMMANDS 16
#define MAX_INPUT_LEN 256
#define MAX_ARGS 16

typedef void (*cmd_handler_t)(int argc, char *argv[]);

typedef struct {
    const char *name;
    cmd_handler_t handler;
} cmd_t;


static cmd_t g_commands[MAX_COMMANDS];
static int g_command_count = 0;
static pthread_t g_cmd_thread;




/* ========================
 * コマンドハンドラ
 * ======================== */

void cmd_help(int argc, char *argv[]) {
    printf("Available commands:\n");
    printf("  help              - Show this help message\n");
    printf("  motor <speed>     - Set motor speed (0=stop, 1=low, 2=high)\n");
    printf("  temp              - Get current temperature\n");
    (void)argc;
    (void)argv;
}

void cmd_motor(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: motor <speed>\n");
        return;
    }
    int speed = atoi(argv[1]);
    MotorCtrl_set_speed(speed);
    printf("Motor speed set to %d\n", speed);
}

void cmd_temp(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: motor <speed>\n");
        return;
    }
    int temp = atoi(argv[1]);
    temp= temp * 16; // 温度を16倍してI2Cに書き込む
    uint8_t temp_data[2];
    temp_data[0] = temp & 0xFF;
    temp_data[1] = (temp >> 8) & 0xFF;
    I2C_write(0x48, temp_data, 2);

}




static void* command_thread(void *arg) {
    char input[MAX_INPUT_LEN];
    char *argv[MAX_ARGS];
    int argc;
    
    (void)arg;
    
    while (1) {
        printf("> ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            continue;
        }
        
        // Remove newline
        input[strcspn(input, "\n")] = 0;
        
        // Skip empty lines
        if (strlen(input) == 0) {
            continue;
        }
        
        // Parse command and arguments
        argc = 0;
        char *copy = malloc(strlen(input) + 1);
        strcpy(copy, input);
        
        char *token = strtok(copy, " \t");
        while (token && argc < MAX_ARGS) {
            argv[argc++] = strdup(token);
            token = strtok(NULL, " \t");
        }
        free(copy);
        
        // Find and execute command
        int found = 0;
        for (int i = 0; i < g_command_count; i++) {
            if (strcmp(g_commands[i].name, argv[0]) == 0) {
                g_commands[i].handler(argc, argv);
                found = 1;
                break;
            }
        }
        
        if (!found) {
            printf("Unknown command: %s\n", argv[0]);
        }
        
        // Free arguments
        for (int i = 0; i < argc; i++) {
            free(argv[i]);
        }
    }
    
    return NULL;
}

void cmd_init(void) {
    // Only initialize in ASYNC mode
    if (os_get_mode() != OS_MODE_ASYNC) {
        return;
    }
    
    // Define commands
    cmd_t commands[] = {
        {"help", cmd_help},
        {"motor", cmd_motor},
        {"temp", cmd_temp}
    };
    int count = sizeof(commands) / sizeof(cmd_t);
    
    // Copy commands
    g_command_count = (count < MAX_COMMANDS) ? count : MAX_COMMANDS;
    for (int i = 0; i < g_command_count; i++) {
        g_commands[i] = commands[i];
    }
    
    // Create command thread
    pthread_create(&g_cmd_thread, NULL, command_thread, NULL);
    pthread_detach(g_cmd_thread);
}
