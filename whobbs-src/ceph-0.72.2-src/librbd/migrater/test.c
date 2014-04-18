#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dlfcn.h>
#include <stdio.h>

int main() {
  pid_t pid;
  pid = fork();
  char *argv[] = {"hello"};
  if(pid < 0) {
  printf("fork error!\n");
  return -1;
  } else if(pid == 0) {
  printf("in son pthread\n");
  if(execl("./TutorialClient", argv[0], NULL) < 0) {
  printf("execl error\n");
  }
  } else {
  wait(NULL);
  }
    return 0;

}
