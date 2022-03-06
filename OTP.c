/************************************************
OTP-Encryptor 1.3               
Licenced under the GNU GeneralPublic Licence v.2
        
This program encrypts a  file using a random key    
and  generates an output file with the resulting    
cipher.  Decryption is achieved  by  running the    
output file as source file  with the same key.      
                            
WARNING: The security of the encryption provided    
by this program is entirely dependent on the key    
file.  The keyfile should meet the  requirements    
below:                          
    - Be of the same size or larger than the    
      source file.                  
    - Be completely random and  generated by 
      a Hardware Random Number Generator.
    - NEVER be reused!              
                            
The  author takes no  responsibility  for use of    
this program.               
                                                                    
Compile: gcc OTP.c -o OTP 
Versions:
1.3 - Add offset argument (default: 0)
    - Removed mlockall to run on MAcOS
    - Removed choice to delete files
1.2 - Option to run  program with sourcefile 
      shorter than keyfile removed. 
    - Memory leaks in  failed  if  functions 
      removed
1.1 Improved syntax and error handling.
1.0 Initial release.
**************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
    struct stat statbuf, keybuf;

    char buffer[20];
    char ans;
    int key = 0, data = 0, output = 0, count = 0, offset = 0;
    int *buf;
    FILE *keyfile, *sourcefile, *destfile;

    if(argc < 4)
    {
        puts("OTP-Encryptor 1.3 \n");

        puts("Licenced under the GNU GeneralPublic Licence v.2\n");

        puts("This program encrypts a  file using a random key");
        puts("and  generates an output file with the resulting");
        puts("cipher.  Decryption is achieved  by  running the");
        puts("output file as source file  with the same key.\n");

        puts("WARNING: The security of the encryption provided");
        puts("by this program is entirely dependent on the key");
        puts("file.  The keyfile should meet the  requirements");
        puts("below:");
        puts("    - Be of the same size or larger than the");
        puts("      source file.");
        puts("    - If offset is implemented, then sourcefile");
        puts("      must be <= than keyfile + offset.");
        puts("    - Be completely random, preferably generated");
        puts("      by a Hardware Random Number Generator.");
        puts("    - NEVER be reused!\n");
        puts("The  author takes no  responsibility  for use of");
        puts("this program.\n");
        puts("Usage:");
        puts("OTP <source file> <output file> <keyfile> <offset (default 0)>");
        return (0);
    }

    /* Allocate memory required by processes */
    buf = (int*) malloc (sizeof(int));
    if (!buf)
    {
        perror("Error");
        free (buf);
        return -3;
    }

    /* Check if sourcefile can be opened. */
    if((sourcefile = fopen(argv[1], "rb")) == NULL)
    {
        puts("Can't open source file.");
        perror("Error");
        free (buf);
        return -5;
    }

    /* Get size of sourcefile */
    fstat(fileno(sourcefile), &statbuf);

    /* Check if keyfile can be opened. */
    if((keyfile = fopen(argv[3], "rb")) == NULL)
    {
        puts("Can't open keyfile.");
        perror("Error");
        free (buf);
        return -6;
    }

    /* Get size of keyfile */
    fstat(fileno(keyfile), &keybuf);

    /* Get the offset */
    if (argv[4] != NULL)
    {
        offset = atoi(argv[4]);
        printf("Offset set to %d .", offset);
    }

    /* Check if keyfile is the same size as, or bigger than the sourcefile */
    if((keybuf.st_size) <= (statbuf.st_size + offset))
    {
        puts("Source file is larger than keyfile + offset");
        puts("Action not allowed.");
        puts("Exiting...");
        free (buf);
        return -7;
    }

    /* Check if destfile can be opened. */
    if ((destfile = fopen(argv[2], "wb")) == NULL)
    {
        puts("Can't open output file.");
        perror("Error");
        free (buf);
        return -8;
    }
        
    /* Set offset position on keyfile*/
    fseek(keyfile, offset, SEEK_SET );
    
    /* Encrypt/Decrypt and write to output file. */
    puts("Encrypting/Decrypting...");
    while (count < (statbuf.st_size))
    {
        key=fgetc(keyfile);
        data=fgetc(sourcefile);

        output=(key^data);

        fputc(output,destfile);
        count++;
    }

    /* Close files. */
    fclose(keyfile);
    fclose(sourcefile);
    fclose(destfile);

    puts("Encryption/Decryption Complete.");

    /* cleanup */
    puts("Releasing memory.");
    free (buf);
    return(0);
}