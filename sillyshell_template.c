#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <termios.h>



// Function: void parse(char *line, char **argv1)
// Purpose : This function takes in a null terminated string pointed to by 
//           <line>.  It also takes in an array of pointers to char <argv1>.
//           When the function returns, the string pointed to by the 
//           pointer <line> has ALL of its whitespace characters (space, 
//           tab, and newline) turned into null characters ('\0').  The
//           array of pointers to chars will be modified so that the zeroth
//           slot will point to the first non-null character in the string
//           pointed to by <line>, the oneth slot will point to the second 
//           non-null character in the string pointed to by <line>, and so
//           on. In other words, each subsequent pointer in argv1 will point
//           to each subsequent "token" (characters separated by white space)
//           IN the block of memory stored at the pointer <line>.  Since all
//           the white space is replaced by '\0', every one of these "tokens"
//           pointed to by subsequent entires of argv1 will be a valid string
//           The "last" entry in the argv1 array will be set to NULL.  This 
//           will mark the end of the tokens in the string.
//    

int background;       

void  parse(char *line, char **argv1)
{
     // We will assume that the input string is NULL terminated.  If it
     // is not, this code WILL break.  The rewriting of whitespace characters
     // and the updating of pointers in argv1 are interleaved.  Basically
     // we do a while loop that will go until we run out of characters in
     // the string (the outer while loop that goes until '\0').  Inside
     // that loop, we interleave between rewriting white space (space, tab,
     // and newline) with nulls ('\0') AND just skipping over non-whitespace.
     // Note that whenever we encounter a non-whitespace character, we record
     // that address in the array of address at argv1 and increment it.  When
     // we run out of tokens in the string, we make the last entry in the array
     // at argv1 NULL.  This marks the end of pointers to tokens.  Easy, right?
    
     while (*line != '\0') // outer loop.  keep going until the whole string is read
        { // keep moving forward the pointer into the input string until
          // we encounter a non-whitespace character.  While we're at it,
          // turn all those whitespace characters we're seeing into null chars.

         

          while (*line == ' ' || *line == '\t' || *line == '\n' || *line == '\r' || *line == '&')
           {
             if(*line == '&')
              {
                background = 1;
              }             
              *line = '\0';     
                 line++;

            }
              
                
             
        

          // If I got this far, I MUST be looking at a non-whitespace character,
          // or, the beginning of a token.  So, let's record the address of this
          // beginning of token to the address I'm pointing at now. (Put it in *argv1)
          // then we'll increment argv1 so that the next time I store an address, it 
          // will be in the next slot of the array of integers.
            if(*line != 0)
                {
                 *argv1++ = line;
                } 
                 /* save the argument position     */

          // Ok... now let's just keep incrementing the input line pointer until
          // I'm looking at whitespace again.  This "eats" the token I just found
          // and sets me up to look for the next.

          while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n' && *line !='\r' && *line !='&') 
              { 
                 line++;
              }             /* skip the argument until ...    */
        

     // Heh, I ran out of characters in the input string.  I guess I'm out of tokens.
     // So, whatever slot of the array at argv1 I'm pointing at?  Yeah, put a NULL
     // there so we can mark the end of entries in the table.

     *argv1 = NULL;                 /* mark the end of argument list  */
         }
}



void execute(char **argv1)
{

  
      pid_t fork_returned_pid = fork();
      int status;
    
      if (fork_returned_pid < 0)
      {
         printf("\033[1;31m");
          printf("The fork() didn't work!  Terminate\n");
           printf("\033[0m");
          return;
      }

      else if (fork_returned_pid > 0)// i am the parent
      {
        if(!background)// if no background
          {

            waitpid(fork_returned_pid,&status, 0);
            //wait(&status);
            background =  0;
          }
        else 
        {
          //wait(NULL);
          background =  0;
          printf("%d \n",fork_returned_pid);
          return;
        }
      }
       else if(fork_returned_pid ==0)// i am the child
      {
         if(execvp(argv1[0], argv1) == -1) 
         {
           printf("\033[1;31m");
          printf("%s error \n", argv1[0]);
          printf("\033[0m");
          exit(0);
         }
                
       }



}

void sighandler(int signum) {/**DO nothing*/}
  
     
int main(int argc, char **argv, char **envp)
{
  signal(SIGINT, sighandler);
  


     char  line[1024];   // This is the string buffer that will hold
                         // the string typed in by the user.  This 
                         // string will be parsed.  The shell will do
                         // what it needs to do based on the tokens it
                         // finds.  Note that a user may NOT type in 
                         // an input line of greater than 1024 characters
                         // because that's the size of the array.
                                     
     char  *argv1[64];    // This is a pointer to an array of 64 pointers to
                         // char, or, an array of pointers to strings. 
                         // after parsing, this array will hold pointers
                         // to memory INSIDE of the string pointed to by 
                         // the pointer line.  argv1[0] will be the string
                         // version of the first token inside of line... 
                         // argv1[1] will be the second... and so on... 
                         // See the routine parse() for details.

     char shell_prompt[15]; // This string will hold the shell prompt string

     // set the default prompt
     strcpy(shell_prompt, "SillyShell");

     // The shell by default goes forever... so... while forever ;)


     while (1) 
       {
         printf("\033[01;33m");  //yellow
         printf("%s",shell_prompt);  // display the shell prompt
         printf("\033[0m");  //reset
         printf(":");
         printf("\033[1;36m"); //bluegreenish 
         printf("~> "); 
         printf("\033[0m"); // reset

          if(fgets(line, 1024, stdin)==NULL)
          {
                printf("Exiting %s. \n",shell_prompt);
                exit(0);
              
          }          // use the safe fgets() function to read 
                                     // the user's command line.  Why wouldn't
                                     // we use gets() here?
                                    //** Because gets() does not protect against 
                                   //    Buffer Overflow because it does not limit
                                  //     the input size.                                      

         line[strlen(line)-1]='\0'; // This is a dirty hack.  Figure it out maybe?
                                    //** sets the last thing in the line array to \0 
                                    // whitch is the \n char. smart lol

          if (*line != '\0') // If something was actually typed, then do something...
            { // First, get all the addresses of all of the tokens inside the input line
              parse(line, argv1);     //   parse the line to break it into token references

              // Check the first token to see if there are any built in commands
              // we want to handle directly.  Do this with an "if/then/else" ladder.
              // if we hit the end of the ladder, we assume the command line was requesting
              // an external program be run as a child process and do that....  
     
              if (strcmp(argv1[0], "exit") == 0)
              { 
                exit(0); 
              }
              else if (strcmp(argv1[0], "done")  == 0)
              {
               exit(0);
              } 
              else if (strcmp(argv1[0], "newprompt") == 0) 
                  { 
                    if (argv1[1] != NULL)
                       {
                        strncpy(shell_prompt, argv1[1], 15); 
                       }
                    else
                      {
                        strncpy(shell_prompt, "SillyShell", 15);
                      }
                } 
                else if (strcmp(argv1[0], "printenv") == 0)
                {
                  char *token;
                  while (*envp != NULL)
                    { token = strtok(*envp, " =");
                      printf("Variable Name: %s\n", token);
                      token = strtok(NULL, "\n");
                      printf("Variable Value: %s\n", token);
                      printf("\n");
                      envp++;
                    }
                }
                else 
                {
                  execute(argv1);  
                }         /* otherwise, execute the command */

            }

     }
}

                

