#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

// Simplifed xv6 shell.

#define MAXARGS 10

// All commands have at least a type. Have looked at the type, the code
// typically casts the *cmd to some specific cmd type.
struct cmd {
  int type;          //  ' ' (exec), | (pipe), '<' or '>' for redirection
};

struct execcmd {
  int type;              // ' '
  char *argv[MAXARGS];   // arguments to the command to be exec-ed
};

struct redircmd {
  int type;          // < or >
  struct cmd *cmd;   // the command to be run (e.g., an execcmd)
  char *file;        // the input/output file
  int mode;          // the mode to open the file with
  int fd;            // the file descriptor number to use for the file
};

struct pipecmd {
  int type;          // |
  struct cmd *left;  // left side of pipe
  struct cmd *right; // right side of pipe
};

int fork1(void);  // Fork but exits on failure.
struct cmd *parsecmd(char*);

// Execute cmd.  Never returns.
void
runcmd(struct cmd *cmd)
{
  int p[2], r;
    int write_fd;
    int read_fd;
  struct execcmd *ecmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;
  printf("hi");
  if(cmd == 0)
    exit(0);
    //printf("%d\n",cmd->type);
  switch(cmd->type){
  default:
    fprintf(stderr, "unknown runcmd\n");
    exit(-1);

  case ' ':
    ecmd = (struct execcmd*)cmd;
         // printf("---%s",(ecmd->argv[3]));
          //printf("---%d",strlen((ecmd->argv)));
    if(ecmd->argv[0] == 0)
      exit(0);
    //fprintf(stderr, "exec not implemented\n");
   char *path = (char *) malloc(1 + strlen("/bin/")+ strlen(ecmd->argv[0]) );
   strcpy(path, "/bin/");
   strcat(path, ecmd->argv[0]);

          //printf("%s\n",path);
         // printf("%s",(char *)ecmd->argv[0]);
        //  char *toCall=ecmd->argv;
         // ecmd->argv[2]='\0';
         // emcd->argv[strlen(emcd)]
          execv(path ,ecmd->argv);
    // Your code here ...
    break;

  case '>':
          printf("hell6");
          rcmd = (struct redircmd*)cmd;
                   printf("%s",rcmd->file);
                  // int buf[512];
                   write_fd=open(rcmd->file,O_WRONLY|O_CREAT|O_TRUNC,0777);
                   dup2(write_fd,1);
                  // close(0);
             //fprintf(stderr, "redir not implemented\n");

             // Your code here ...
             runcmd(rcmd->cmd);
          break;
  case '<':
	  printf("hello5");
    rcmd = (struct redircmd*)cmd;
          printf("%s",rcmd->file);
          int buf[512];
          read_fd=open(rcmd->file,O_RDONLY,0777);
          dup2(read_fd,0);
         // close(0);
    //fprintf(stderr, "redir not implemented\n");

    // Your code here ...
    runcmd(rcmd->cmd);
    break;

  case '|':
    pcmd = (struct pipecmd*)cmd;
    printf("inside pipe");
   printf("%d",pcmd->left->type);
    //int pipe1[2];
    //pipe(pipe1);
   // dup2(pipe1[1],1);
   // close(pipe1[1]);
  //  fprintf(stderr, "pipe not implemented\n");
    // Your code here ...
    break;
  }
  exit(0);
}

int
getcmd(char *buf, int nbuf)
{

  if (isatty(fileno(stdin)))
    fprintf(stdout, "238P$ ");
  memset(buf, 0, nbuf);
  fgets(buf, nbuf, stdin);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

int
main(void)
{
  static char buf[100];
  int fd, r;
 // printf("hello");

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
      // Clumsy but will have to do for now.
      // Chdir has no effect on the parent if run in the child.
      buf[strlen(buf)-1] = 0;  // chop \n
      if(chdir(buf+3) < 0)
        fprintf(stderr, "cannot cd %s\n", buf+3);
      continue;
    }
   // printf("hello1");
    if(fork1() == 0)
      runcmd(parsecmd(buf));
    wait(&r);
  }
  exit(0);
}

int
fork1(void)
{
  int pid;

  pid = fork();
  if(pid == -1)
    perror("fork");
  return pid;
}

struct cmd*
execcmd(void)
{
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = ' ';
  return (struct cmd*)cmd;
}

struct cmd*
redircmd(struct cmd *subcmd, char *file, int type)
{
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = type;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->mode = (type == '<') ?  O_RDONLY : O_WRONLY|O_CREAT|O_TRUNC;
  cmd->fd = (type == '<') ? 0 : 1;
  return (struct cmd*)cmd;
}

struct cmd*
pipecmd(struct cmd *left, struct cmd *right)
{
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = '|';
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

// Parsing

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>";

int
gettoken(char **ps, char *es, char **q, char **eq)
{
  char *s;
  int ret;

  s = *ps; // points to first char in *ps ie first char of our input string
  while(s < es && strchr(whitespace, *s)) // skips whitespace
    s++;
  // q=double pointer  q points to address of passed q which points to first char in passed q
  if(q)
    *q = s;
  //q is pointing to s ie first char in input
  ret = *s; // ret has the first value of string
  switch(*s){ //check first char
  case 0:
    break;
  case '|':
  case '<':
    s++;
    break;
  case '>':
    s++;
    break;
  default:
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  if(eq)
    *eq = s;

  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return ret;
}
// this function looks for blank spaces and moves ahead. And check whteher the toks vlaue passed is equal to the first char
int
peek(char **ps, char *es, char *toks)
{
  char *s;
// ps=double pointer to the command passed for 'ls'
  //es= length of ls just to check pointer dosent go beyond es=s+len(s)
  s = *ps;
  while(s < es && strchr(whitespace, *s)) // seeing whether the first char is blank space or not. Trying to skip blank spaces.
    s++;
  *ps = s;// assign value after jumping the blank spaces
  //*ps points to the first char after blank spaces.
  return *s && strchr(toks, *s); // ######3not excatly understood how it works###########3
  //strchr checks fpor the occurence of char in toks="<>"
  //*s=l(accoridng to our example which is 'ls'
}

struct cmd *parseline(char**, char*);
struct cmd *parsepipe(char**, char*);
struct cmd *parseexec(char**, char*);

// make a copy of the characters in the input buffer, starting from s through es.
// null-terminate the copy to make it a string.
char
*mkcopy(char *s, char *es)
{
  int n = es - s;

  char *c = malloc(n+1);
  assert(c);
  strncpy(c, s, n);
  c[n] = 0;
  return c;
}

struct cmd*
parsecmd(char *s)
{
  char *es;
  struct cmd *cmd;

  es = s + strlen(s);
 // printf("hey");
  cmd = parseline(&s, es);
  peek(&s, es, "");
  if(s != es){
    fprintf(stderr, "leftovers: %s\n", s);
    exit(-1);
  }
  return cmd;
}

struct cmd*
parseline(char **ps, char *es)
{
  struct cmd *cmd;
  cmd = parsepipe(ps, es);
  return cmd;
}

struct cmd*
parsepipe(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parseexec(ps, es);
  if(peek(ps, es, "|")){
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  return cmd;
}

struct cmd*
parseredirs(struct cmd *cmd, char **ps, char *es)
{
  int tok;
  char *q, *eq;

  while(peek(ps, es, "<>")){ // looking for I/O redirection
    tok = gettoken(ps, es, 0, 0);
    if(gettoken(ps, es, &q, &eq) != 'a') {
      fprintf(stderr, "missing file for redirection\n");
      exit(-1);
    }
    switch(tok){
    case '<':
      cmd = redircmd(cmd, mkcopy(q, eq), '<');
      printf("hello");
      break;
    case '>':
      cmd = redircmd(cmd, mkcopy(q, eq), '>');
      break;
    }
  }
  return cmd;
}

struct cmd*
parseexec(char **ps, char *es)
{
  char *q, *eq;
  int tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;

  ret = execcmd();
  cmd = (struct execcmd*)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  while(!peek(ps, es, "|")){
    if((tok=gettoken(ps, es, &q, &eq)) == 0)
      break;
    if(tok != 'a') {
      fprintf(stderr, "syntax error\n");
      exit(-1);
    }
    cmd->argv[argc] = mkcopy(q, eq);
    argc++;
    if(argc >= MAXARGS) {
      fprintf(stderr, "too many args\n");
      exit(-1);
    }
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  return ret;
}
