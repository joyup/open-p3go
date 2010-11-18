/********************** BEGIN LICENSE BLOCK ************************************
 *
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright (c) Ingenic Semiconductor Co. Ltd 2005. All rights reserved.
 * 
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * http://www.ingenic.cn 
 *
 ********************** END LICENSE BLOCK **************************************
 *
 *  Author:  <zzhang@ingenic.cn>  
 *
 *  Create:   2008-10-31, by zhang zhi
 *            
 *  Maintain:
 *            
 *
 *******************************************************************************
 */
 
#if RELEASE==0 

#define MAX_COMMAND_NUM    10

#define ERRORCOMMAND    255

#define MaxLenComBuf	100
#define Max_ComBuf_Num	10

struct
{
	char *name;
	void (*CommandFunc)(char argc,char **argv);
} ShellComms[MAX_COMMAND_NUM];
struct
{
	char commandBuf[MaxLenComBuf+1];
	char commandNum;
} Command[Max_ComBuf_Num];
static char *argv[10];
static unsigned char argc;

/*  This is an example of how to write a command.
void HelloFunc(char argc,char **argv)
{
	int i;
	printf("\n\rHello! I am mini shell");
	printf("\n\ryour argv is :");
	if(argc>0){
	for(i=0;i<argc;i++){
		printf("\n\r");
		printf(argv[i]);
		}
  	}
}
*/
void init_command()
{
	int i;
	for(i = 0; i<MAX_COMMAND_NUM; i++ ) 
 	ShellComms[i].CommandFunc = 0; 
 	ShellComms[i].name = 0;
}
	
int request_command(void (*handler)(char argc,char **argv),char *name)
{  
	int i = 0;
	int pointer = 0;
	for(i =0;i<MAX_COMMAND_NUM;i++ ) 
 	{
 		if (ShellComms[i].CommandFunc == handler )
 		{
			printf("\n\r This command function is already requested as command %s!",ShellComms[i].name);
			return ;
		} 
	}
	for(i =0; i<MAX_COMMAND_NUM; i++ ) 
 	{
 		if (ShellComms[i].name == name)
 		{
			printf("\n\r The command name %s is already requested !",ShellComms[i].name);
			return ;
		} 
	}
 	for(i = 0; i<MAX_COMMAND_NUM; i++ ) 
 	{
 		if (ShellComms[i].CommandFunc == 0 )
 		{
			ShellComms[i].CommandFunc = handler;
			ShellComms[i].name = name;
			return ;
		} 
	} 	
	printf("\n\rCommandBuffer is full!");
	return ;
}
int free_command(void (*handler)(char argc,char **argv))
{
	int i = 1;
 	for(i = 0; i<MAX_COMMAND_NUM; i++ ) 
 	{
 		if (ShellComms[i].CommandFunc == handler )
 		{
			ShellComms[i].CommandFunc = 0;
			ShellComms[i].name = 0;
			return ;
		} 
	} 	
	printf("\n\rThis command is not in buffer!");
	return ;
}
static char seek(char command)
{ static char end=0,current=0,full=0;
   switch(command)
   {
	   	case 65://up
	   		if ( full == 10 )
	   			{
 	  			if( current == 0)
  	 				current = 10;
   				else
  	 				current--; 
  	 			}
  	 		else
   				if ( current == 0)
   					current = end; 
   				else
   					current--;		
   			break;
   		case 66://down
   			if ( full == 10 )
   				{
   				if( current == 10)
   					current = 0;
   				else
   					current++; 
   				}
   			else
   				if ( current == end)
   					current = 0; 
   				else
   				current++;		
   			break;
 	  	case 1://end 
 	  		if (full == 10)
	   		{
   				end++;
   				if (end ==11)
   					end = 0;
   				current = end;  
   			}
   			else if (full > 0)
   			{
   				full++;
	   			end++;
   				current = end;
				
  			}
  			else 
  				{full++;
  				current = end;
  				}
  			break ;
	}
	return current;
}
   			
	

static char CommandAnalys(char *Buf)
{
	int i = 0;
	int pointer;
	char num;
	char name[20];		//command name length <20

	argc = 0;              //argc is global
	pointer = 0;
	num = 0;
	while((Buf[pointer]!=' ') && (Buf[pointer]!='\0') && pointer<20 )
	{
		name[pointer]=Buf[pointer];
		pointer++;
	}	
	name[pointer] = '\0';	//now got the command name, and pointer is to the first space in the Buf
	for(i=0;i<MAX_COMMAND_NUM;i++ )
	{    
		if(ShellComms[i].name != 0 )
		if(!strcmp(name,ShellComms[i].name))
		{
			num = i;
			break;
		}				//find the command number
	}			
						
	if (i == MAX_COMMAND_NUM)//not find it 
		return ERRORCOMMAND;
					
	while(Buf[pointer] != '\0')
	{
		if(Buf[pointer] == ' ')
		{
			if(argc>0)
			{
				Buf[pointer] = '\0';			//end of last argv
			}
			pointer++;
			argv[argc] = &Buf[pointer];			//add a parameter for every space
			argc++;
		}
		else
		{
			pointer++;
		}
	}//while
	return num;
}

void shelltask()
{
unsigned char i = 0,t,num,err;		/*i is the pointer of commandbuf */
char ch;
void (*Func)( char argc,char **argv);
char CommandBuf[MaxLenComBuf+1];	/*store '\0'*/
char currentCommand;
//CommRxIntEn(UART0);
//InitCommands();
CommandBuf[0] = '\0';

//to do: add some lib functions for ucosII ,like clear screen .
//clrscr();
printf("\n\r***********************************************");
printf("\n\r*         Welcom to minios shell              *");
printf("\n\r***********************************************\n");

printf("\n\rshell>");
	for(;;)
	{
		do	{					//only accept a-z,0-9,A-Z,.,space,/,-
			ch = serial_getc();
		}while(!((ch==27)||(ch>='0'&&ch<='9')||(ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||(ch=='.')||(ch==' ')||(ch=='-')||(ch=='/')||(ch==13)||(ch=='\b')||(ch==',')));	
		
		switch(ch)
		{
			case 13:				//enter
				if (i==0)
				{      						//commandbuf is null,begin a new line
					printf("\n\rshell>");
				}
				else
				{
					currentCommand=seek(1);//store current command
					Command[currentCommand].commandNum=i;
					for(t=0;t<=MaxLenComBuf;t++)
					Command[currentCommand].commandBuf[t]= CommandBuf[t];
					if(CommandBuf[i-1] == ' ') 
						i--;			//get rid of the end space
					CommandBuf[i] = '\0';
					num = CommandAnalys(CommandBuf);	//analys the argv in the commandbuf
					if(num == ERRORCOMMAND)
					{             	//error or none exist command
						i = 0;
						CommandBuf[i] = '\0';
						printf("\n\rCommand not found");
						printf("\n\rshell>");
					}
					else
					{
						Func = ShellComms[num].CommandFunc;	//call corresponding CommandFunc
						Func(argc,argv);
						i = 0;
						CommandBuf[i] = '\0';
						printf("\n\rshell>");
					}
					seek(66);
				}
			break;
		
			case '\b':				//backspace
				if ( i==0 )
				{		//has backed to first one
						//do nothing
				}
				else
				{
					i--;			//pointer back once
					serial_putc('\b');		//cursor back once
					serial_putc(' ');	      //earse last char in screen
					serial_putc('\b');		//cursor back again
				}
			break;
			
			case ' ':               //don't allow continuous or begin space(' ')
				if((CommandBuf[i-1] == ' ')||(i==0)||(i>MaxLenComBuf))
				{
						//do nothing
				}
				else
				{
					CommandBuf[i] = ch;
					i++;
					serial_putc(ch);  //display and store ch
				}
			break;
			case 27:               
				while(!serial_tstc());
				ch = serial_getc();
				while(!serial_tstc());
				ch = serial_getc();
				for(t=0;t<i;t++)
				{
					serial_putc('\b');		//cursor back once
					serial_putc(' ');	      //earse last char in screen
					serial_putc('\b');		//cursor back again
				}
				currentCommand=seek(ch);
				i = Command[currentCommand].commandNum;
				for(t=0;t<i;t++)
				{
					serial_putc(Command[currentCommand].commandBuf[t]);
					CommandBuf[t] = Command[currentCommand].commandBuf[t];
			   	}
			break;
			
		
			default:				//normal key
				if (i>MaxLenComBuf)
				{	//the buf reached MAX 
					//do nothing
				}			
				else
				{
					CommandBuf[i] = ch;
					i++;
					serial_putc(ch);  //display and store ch
				}
			break;
		}  //switch
	}//for(;;)
}

#endif
