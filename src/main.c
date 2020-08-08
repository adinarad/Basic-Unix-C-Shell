/***************************************************************************//**
  @file         main.c
  @author       Aditya Narad
  @date         Thursday,  8 August 2019
  @brief        LSH (Linux Shell)
*******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>


/**
 * Function Declarations for builtin shell commands:
 */
int ush_cd(char **args);
int ush_help(char **args);
int ush_exit(char **args);
int ush_echo(char **args);
int ush_history(char **args);
int ush_pwd(char **args);

/**
 * List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = { "cd", "help", "exit", "echo", "history", "pwd"};

int (*builtin_func[]) (char **) = {ush_cd, ush_help, ush_exit, ush_echo, ush_history, ush_pwd};

/**
   @brief  Find number of builtin commands in shell.
   @return Number of elements in builtin_str array.
 */
int ush_num_builtins()
{
  return (sizeof(builtin_str) / sizeof(char *));
}

/*
 *Builtin commands' function implementations.
*/

/**
   @brief Builtin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int ush_cd(char **args)
{
    if(args[1] == NULL){
        fprintf(stderr, "ush: expected argument with \"cd\"\n");
    }
    else if(chdir(args[1]) != 0){
        perror("ush");
    }
    return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int ush_help(char **args)
{
    printf("Aditya Narad's Linux Shell\n");
    printf("How to Use Shell: Type the commands followed by arguments(if any) and press Enter.\n");
    printf("Following are the builtin commands :\n");
    int num_builtin_commands = ush_num_builtins();

    for (int i = 0; i < num_builtin_commands; i++){
        printf("\t%s\n", builtin_str[i]);
    }
    return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int ush_exit(char **args)
{
  return 0;
}

/**
   @brief Builtin command: print input string.
   @param args List of args.  args[0] is "echo".  Rest values in args is the input string.
   @return Always returns 1, to continue executing.
 */
int ush_echo(char **args)
{
    for (int i = 1; args[i]!=NULL; i++){
        printf("%s ",args[i]);
    }
    printf("\n");
    return 1;
}

/**
   @brief Builtin command: current directory.
   @param args List of args.  args[0] is "pwd".
   @return Always returns 1, to continue executing.
 */
int ush_pwd(char **args)
{
    long path_max;
    unsigned int size;
    char *buf;
    char *ptr;


    path_max = pathconf(".", _PC_PATH_MAX);
    if (path_max == -1)
        size = 1024;
    else if (path_max > 10240)
        size = 10240;
    else
        size = path_max;


    for (buf = ptr = NULL; ptr == NULL; size *= 2)
    {
      buf = realloc(buf, size);
      if (buf == NULL){
        fprintf(stderr, "ush: allocation error\n");
        exit(EXIT_FAILURE);
      }


      ptr = getcwd(buf, size);
      if (ptr == NULL && errno != ERANGE){
        perror("ush");
        return 1;
      }
    }
    printf("%s", ptr);
    free (buf);
    return 1;
}

#define USH_MAX_HISTORY_COUNT 20
char* history_str[USH_MAX_HISTORY_COUNT];
size_t history_pos = 0;

/**
   @brief Builtin command: shows a list of the commands entered since the start of session..
   @param args List of args.  args[0] is "history".
   @return Always returns 1, to continue executing.
 */
int ush_history(char **args)
{
  size_t history_num = 1;
  size_t i = history_pos;

  do
  {
    if(history_str[i] != NULL){
      printf("%4lu   %s\n", history_num, history_str[i]);
      ++history_num;
    }
    i = (i + 1) % USH_MAX_HISTORY_COUNT;
  } while (i != history_pos);

  return 1;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int ush_launch(char **args)
{
  pid_t pid = fork();
  if(pid == 0){
    //Child Process
    if(execvp(args[0], args) == -1){
      perror("ush");
      exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
  }
  else if(pid < 0){
    //Couldn't fork.
    perror("ush");
  }
  else{
    //Parent Process
    //Waiting for child to terminate.
    wait(NULL);
  }
  return 1;
}


/**
 * @brief Adds input to history.
 * @param line The input string
 */
void add_to_history_util(char *line)
{
  free(history_str[history_pos]);

  size_t line_size = strlen(line);
  char* history_entry = (char*)malloc(line_size * sizeof(char));

  if(history_entry == NULL){
    fprintf(stderr, "ush: allocation error\n");
    exit(EXIT_FAILURE);
  }
  strcpy(history_entry, line);

  history_str[history_pos] = history_entry;

  history_pos = (history_pos + 1) % USH_MAX_HISTORY_COUNT;
}


/**
   @brief Calls shell built-in or launch program. Used to execute internal and external commands.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int ush_execute(char **args)
{
  if(args[0] == NULL){
    //Empty Command
    return 1;
  }

  int num_builtin_commands = ush_num_builtins();

  for (int i = 0; i < num_builtin_commands; i++){
    if(strcmp(args[0], builtin_str[i]) == 0){
      //Command entered is Internal Command
      return (builtin_func[i](args));
    }
  }
  //Command entered is External Command
  return ush_launch(args);
}

/**
 * @brief Read a line of input from stdin.
 * @return The line raed from stdin
 */

char* ush_read_line()
{
  char* inp_line = NULL;
  size_t bufsize = 0;
  getline(&inp_line, &bufsize, stdin);

  return inp_line;
}

#define USH_TOK_BUFSIZE 64
#define USH_TOK_DELIM " \"\t\r\n\a()"
/**
 * @brief Split the input line into tokens.
 * @param line The input line.
 * @return Null-terminated array of tokens.
 */
char** ush_split_line(char* line)
{
  size_t bufsize = USH_TOK_BUFSIZE;
  size_t pos = 0;
  char** tokens = (char**)malloc(sizeof(char*) * bufsize);
  char** ptr = NULL;
  char* token;

  if(tokens == NULL){
    fprintf(stderr, "ush: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, USH_TOK_DELIM);
  while(token!=NULL){
    tokens[pos++] = token;

    if(pos == bufsize){
      bufsize = bufsize * 2;
      ptr = (char**)realloc(tokens, bufsize);

      if(ptr == NULL){
        free(tokens);
        fprintf(stderr, "ush: allocation error\n");
        exit(EXIT_FAILURE);
      }
      tokens = ptr;
    }

    token = strtok(NULL, USH_TOK_DELIM);
  }
  tokens[pos] = NULL;
  return tokens;
}

/**
 * @brief Loop for getting input and excuting it.
 */
void ush_loop()
{
  char* line;
  char** args;
  int status;

  do
  {
    fputs("\n> ", stdout);
    line = ush_read_line();
    args = ush_split_line(line);

    if(args[0] != NULL){
      add_to_history_util(line);
    }
    status = ush_execute(args);

    free(line);
    free(args);
  } while (status);
}

/**
 * @brief Main entry point.
 * @param argc Argument count.
 * @param argv Argument(Command Line) vector.
 * @return status code.
 */
int main(int argc, char** argv)
{

  for (int i = 0; i < 80; i++){
    fputc('*', stdout);
  }
  puts("\n Welcome to Linux Shell.");
  puts("\nDeveloped in C by Aditya Narad - CSE 1");
  for (int i = 0; i < 80; i++){
    fputc('*', stdout);
  }

  //Initialize history_str array with NULL values.
  for (size_t i = 0; i < USH_MAX_HISTORY_COUNT; i++){
    history_str[i] = NULL;
  }

  //Run command loop.
  ush_loop();

  puts("\nGoodBye!!!");

  return EXIT_SUCCESS;
}
