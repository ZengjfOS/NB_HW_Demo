#include "zengjf.h"

// command
#define CMD_BUF_SIZE 512

char cmd_buf[CMD_BUF_SIZE] = {0};
int cmd_buf_count = 0;

void get_cmd_parser_char(void) 
{
    char ch = getchar();
    
    if (ch == 127 || ch == 8) {
        --cmd_buf_count;
        
        if (cmd_buf_count < 0)
            cmd_buf_count = 0;

        if (cmd_buf_count > CMD_BUF_SIZE - 1)
            cmd_buf_count = CMD_BUF_SIZE - 1;
        
        cmd_buf[cmd_buf_count] = 0;
        printf("\033[1K\rAplexOS # %s", cmd_buf);

        return;
    } 
    else if (ch == '\r')
    {     
        if (strlen(cmd_buf) != 0) 
        {
            shell_process(cmd_buf);
            printf("\r\n");
        }
        
        printf("AplexOS # ");

        memset(cmd_buf, 0, strlen(cmd_buf));
        cmd_buf_count = 0;
        
        return;
    }
    
    cmd_buf[cmd_buf_count++] = ch;
}

char* welcome_msg =
    "\r\n\r\nHardware Auto Detect System v0.0.1 (" __DATE__ ")\r\n"
    "\r\n               ---- Designed By AplexOS Team \r\n\r\n";

int shell_cmd_help(char *args);
int shell_cmd_reset(char *args);

shell_cmds microcli_shell_cmds =
{
    .count = 3,
    .cmds  = {
        {
            .cmd     = "h",
            .desc    = "help info",
            .func    = shell_cmd_help,
        },
        {
            .cmd     = "help",
            .desc    = "List available",
            .func    = shell_cmd_help,
        },
        {
            .cmd     = "reset",
            .desc    = "reset system",
            .func    = shell_cmd_reset,
        },
    },
};

int shell_cmd_help(char *args) 
{
    printf("execute %s function.\r\n", __func__);
    return 0;
}

int shell_cmd_reset(char *args)
{
    __set_FAULTMASK(1);//关闭总中断
    NVIC_SystemReset();//请求单片机重启   
    
    return 0;
}

int shell_process(char *cmd_line) 
{
    int i = 0;
    for (i = 0; i < microcli_shell_cmds.count; i++) 
    {
        if (strncmp(microcli_shell_cmds.cmds[i].cmd, cmd_line, strlen(microcli_shell_cmds.cmds[i].cmd)) == 0) 
        {
            if (cmd_line[strlen(microcli_shell_cmds.cmds[i].cmd)] == ' ') 
            {
                microcli_shell_cmds.cmds[i].func(&cmd_line[strlen(microcli_shell_cmds.cmds[i].cmd) + 1]);
                return 0;
            } else if (strcmp(microcli_shell_cmds.cmds[i].cmd, cmd_line) == 0 ) {
                microcli_shell_cmds.cmds[i].func(NULL);
                return 0;
            }
        }
        
    }
    
    printf("unsupport command.");
    return -1;
}

