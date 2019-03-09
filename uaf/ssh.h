#ifndef __SSH_H__
#define __SSH_H__
#include <libssh2.h>


enum token_type
{
	TOKEN_TYPE_NONE,
	TOKEN_TYPE_RET_CHAR,
	TOKEN_TYPE_WIN_RET_CHAR,
	TOKEN_TYPE_STRING
};

struct token
{
	size_t length;
	enum token_type type;
	char *data;
	char c;
};

struct tokenizer
{
	int count;
	char* at;
};

struct ssh
{
	const char *fingerPrint;
	LIBSSH2_SESSION *session;
	LIBSSH2_CHANNEL *channel;
	int socket;
};


struct token GetToken(struct tokenizer* tokenizer)
{
	struct token tok={};
	char c = 0;
	i8 tokAddr = NULL;

	tok.length = 1;

	if (tokenizer->at)
	{
		tok.data = tokenizer->at;
		c = tokenizer->at[0];
		tok.c = c;

		if (c != '\0')
		{
			++tokenizer->count;
			++tokenizer->at;
		}
		switch(c)
		{
			case '\0': {tok.type=TOKEN_TYPE_NONE;break;}
			case '\n': {tok.type=TOKEN_TYPE_RET_CHAR;break;}
			case '\r': {tok.type=TOKEN_TYPE_WIN_RET_CHAR;break;}
			default: { tok.type=TOKEN_TYPE_STRING;break;}
		}

	}

	return tok;
}

typedef void (*SSH_ReceivedMessage)(char*);

void SSH(struct ssh *ssh,char* hostname, int port, char* username, char *password, SSH_ReceivedMessage receivedMsgs, bool *doneFlag)
{
	ssh->socket = CreateSocket(hostname,port);

	if (ssh->socket != -1)
	{
		ssh->session = libssh2_session_init();
		if (ssh->session)
		{	
			int sessionHandShake = libssh2_session_handshake(ssh->session,ssh->socket);
			if (sessionHandShake == 0)
			{
				ssh->fingerPrint = libssh2_hostkey_hash(ssh->session,LIBSSH2_HOSTKEY_HASH_SHA1);
				if (ssh->fingerPrint)
				{
					printf("Authorizing..\n");
					if (libssh2_userauth_password(ssh->session, username, password)) 
					{
						fprintf(stderr, "\tAuthentication by password failed!\n");
					} else {
						printf("requesting shell\n");
						ssh->channel = libssh2_channel_open_session(ssh->session);

						if (ssh->channel)
						{
							if (libssh2_channel_request_pty(ssh->channel,"vanilla") == 0)
							{
								if (libssh2_channel_shell(ssh->channel) == 0) 
								{
									bool isDone = false;
									int count = 69;
									char buffer[515] = {};
									s32 *currentString=NULL;
									char *messages = NULL;
									int oldCount = -1;

									while (count != -1 && *doneFlag == false)
									{
										struct token token = {};
										struct tokenizer tokenizer = {};

										memset(buffer,0,512);
										count =  libssh2_channel_read(ssh->channel,buffer,512);

										tokenizer.at = buffer;

										for (int i=0;i<count;i++)
										{
											token =  GetToken(&tokenizer);

											switch(token.type)
											{
												case TOKEN_TYPE_RET_CHAR:
												case TOKEN_TYPE_WIN_RET_CHAR:
												{

													receivedMsgs(currentString);

													if (currentString)
													{
														Free(currentString);
														currentString = NULL;
													}
													break;
												} 
												default:
												{
													char tempC[5] = {};
													tempC[0] = token.c;

													if (currentString == NULL)
													{
														currentString = S32(tempC);
													} else {
														s32 *tempString = currentString;
														currentString = S32Cat(tempString,1,tempC);

														if (tempString)
														{
															Free(tempString);
															tempString = NULL;
														}
													}
													break;
												}
											}

										}

										if (count != 512)
										{
											if (currentString)
											{
												receivedMsgs(currentString);

												if (currentString)
												{
													Free(currentString);
													currentString=NULL;
												}
											}
										}
										
									}


									/*

									while (count != 0 && isDone == false)
									{
										struct marray *results;
										int lastPosition = 0;


										memset(buffer,0,512);
										count =  libssh2_channel_read(ssh->channel,buffer,512);
										
										results = FindString("\n",buffer);

										if (results->count > 0)
										{
											for (int resultIndex = 0; resultIndex < results->count;resultIndex++)
											{
												int *nPosition = GetFromMArray(results,resultIndex);

												if (nPosition != NULL)
												{
													char *part = MidString(buffer,lastPosition,(*nPosition));

													if (part)
													{
														if (messages)
														{
															char *temp = messages;
															messages = S32Cat(temp,1,part);

															receivedMsgs(messages);

															if (messages)
															{
																Free(messages);
																messages=NULL;
															}

														} else {
															messages = S32(part);
														}
													} 
												}

												lastPosition= (*nPosition);
											}

											if (lastPosition != Strlen(buffer))
											{
												if (messages == NULL)
												{
													messages = MidString(buffer,lastPosition,Strlen(buffer));
												} else {
													char *catThis = MidString(buffer,lastPosition,Strlen(buffer));
													char *tempMessages = messages;

													messages = S32Cat(tempMessages,1,catThis);
												}

											}

										} else {
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
										}
										

									}
									*/

								}
							} else {
								printf("unable to spawn shell\n");
							}
						} else {
							printf("Unable to spawn shell\n");
						}
						
					}

				}
			} else {
				printf("Can not complete handmadeshake! Error %i\n", sessionHandShake);
			}
		} else {
			printf("Unable to init session!\n");
		}
	} else {
		printf("error connection to host\n");
	}

	//NOTES(): Clean here
	if (ssh->channel)
	{
		libssh2_channel_free(ssh->channel);
	}
	libssh2_session_disconnect(ssh->session,"session ended");
	libssh2_session_free(ssh->session);

	libssh2_init(0);
}
#endif
