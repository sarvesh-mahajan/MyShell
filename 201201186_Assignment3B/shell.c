
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*               Author : Sarvesh Mahajan
		 Roll NO. 201201186                                              */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/wait.h>
char curr_dir[60];
char *username;
typedef struct process
{
	pid_t id;
	char name[200];
}process;
process queue[1000];

int start=0,end=0;
int cpid=0;
void push(process a)
{
	queue[end++]=a;
}
void pop()
{
	start++;
}
int c;
void get_path_name(char *buf)
{
	int l=strlen(buf),j=1,i;
	if(buf[0]=='/')
	{
		strcpy(curr_dir,buf);
		return ;
	}

	for(i=6+strlen(username);i<=l;++i)
		curr_dir[j++]=buf[i];
}
void sig_handler(int signo)
{
	int i,fl=0;
	if(signo==SIGINT)
		;
	if(signo==SIGTSTP)
	{
		int status;
		if(cpid)
		kill(cpid,SIGTSTP);
	//	puts("Moving process to background");
		//pid_t pid=wait(&status);
		//kill(cpid,SIGCONT);
		process a;
		a.id=cpid;
		strcpy(a.name,"Process");
		a.name[1]='\0';
		push(a);
	}
	else if(signo==SIGCHLD)
	{
		//printf("chld\n");
		//	printf("Caught SIGCHLD\n");
		//	signal(SIGCHLD,sig_handler);
		int status;
		pid_t pid;
	//	pid=wait(&status);
		//printf("%d \n",pid);
		for(i=start;i<end;++i)
		{
			if(queue[i].id==pid)
			{
				fl=1;
				break;
			}
		}
		if(fl)
		{
			if(pid)
			printf("pid=%d exited with status %d \n",pid,status);
			queue[i].id=0;
		}
	}

}
void fg(pid_t pid,char name[])
{
	int *status;
//	signal(SIGCHLD,sig_handler);
	printf("Moved process %s with process id %d to foreground \n",name,pid);
	waitpid(pid,status,WUNTRACED);
	if(WIFSTOPPED(status))
	{
		kill(pid,SIGCONT);
		printf("Pushed to background \n");
	}
	//wait(NULL);
}


void pinfo(int k)
{
	int j;

	char str1[100],path[100],st[100],*arr[200],buffer[60],buffer2[200];
	snprintf(str1,100,"%d",k);
	path[0]='/';
	path[1]='p';
	path[2]='r';
	path[3]='o';
	path[4]='c';
	path[5]='/';
	path[6]='\0';



	strcat(path,str1);
	strcpy(st,path);
	strcat(st,"/stat");
	char buffer1[200];

	int file_desc=open(st,O_RDONLY);
	int m=read(file_desc,buffer,40);
	if(m==-1)
	{
		printf("No  process with process id : %d\n",k);
		return ;
	}
	printf("Process id --- %d\n",k);
	arr[0]=strtok(buffer," ");
	arr[1]=strtok(NULL," ");
	arr[2]=strtok(NULL," ");
	printf("Process status ---%s \n",arr[2]);
	close(file_desc);
	strcpy(st,path);
	strcat(st,"/status");
	file_desc=open(st,O_RDONLY);
	read(file_desc,buffer2,200);
	arr[0]=strtok(buffer2,"\n");
	for(j=1;j<=12;++j)
		arr[j]=strtok(NULL,"\n");
	puts(arr[10]);

	strcpy(st,path);
	strcat(st,"/maps");
	file_desc=open(st,O_RDONLY);
	read(file_desc,buffer1,180);
	arr[0]=strtok(buffer1," ");
	for(j=1;j<=4;++j)
		arr[j]=strtok(NULL," ");
	//	puts(arr[4]);
	arr[5]=strtok(NULL,"\n");
	if(arr[5]==NULL)
		arr[5]="Unknown";
	printf("Executable address---%s\n",arr[5]);


}

void redirect(char s[])
{
	int i,j;
	char *a[200],*array[2000];
	char *p,*q;
	p=strstr(s,"<");
	q=strstr(s,">");
	if(p!=NULL )
	{
	
			a[0]=strtok(s,"<"); //command
			a[1]=strtok(NULL,"<"); //file name
		
		if(q==NULL)
		{
		a[2]=a[1];    //Input file 
		a[3]=a[1];    //No semantics
		}
		else
		{
		a[2]=strtok(a[1],">");
		a[3]=strtok(NULL,">");
		}
	}
	else 
	{
		a[0]=strtok(s,">");           //command
		a[1]=strtok(NULL,">");        // file name
		a[2]=a[1];                    //No semantics
		a[3]= a[1];                   //Output file
	}
	        int l=strlen(a[2]);
		i=l-1;
		sscanf(a[2],"%s",a[2]);        //To remove extra spaces
	/*	while(i>0 && a[2][i]==' ')
		{
			a[2][i]='\0';
			i--;
		}*/

		sscanf(a[3],"%s",a[3]);
		
		

		array[0]=strtok(a[0]," ");
		//puts(array[0]);
		i=0;
		while((array[++i]=strtok(NULL," "))!=NULL);   //Array contains the command to be executed
                //for(j=0;j<i;++j)
		//	printf("%s ",array[j]);

                int paid=fork();
		if(paid>0)
		{
			int status;
			waitpid(paid,&status,0);
		}

		else if(paid==0)
		{
		if(p!=NULL)
		{
		int in=open(a[2],O_RDONLY);
		if(in<0)
		{
			perror("Error opening input file");
			return ;
		}
		
       		dup2(in,0);
		close(in);
		}
		if(q!=NULL)
		{
		int out=open(a[3],O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
		if(out<0)
		{
			perror("Error opening output file");
			return ;
		}
         	dup2(out,1);
		close(out);
		}
			execvp(array[0],array);
			_exit(0);
		}

	
	
}
char *temp[200];
char *args[200][200];


int get_current_command(int i)
{
int j,fl=0,k;
char ans[1000];
ans[0]='\0';
for(j=0;args[i][j]!=NULL;++j)
{
temp[j]=args[i][j];
//printf("%s ",temp[j]);
}
temp[j+1]=NULL;

return fl;
}



void handle_pipe(char s[])
{
	char inputfilename[200],outputfilename[200];
	int inp=0,out=0;
	char *a[2000];
	int fd[2000];
	int pid,i,k,l,j,n;
	char *p,*q;
	int r;
	 l=strlen(s);

	p=strstr(s,"<");
	q=strstr(s,">");
	
	if(p!=NULL)
	{
		r=p-&s[0];
//		printf("%c",s[r]);
	s[r]=' ';
	for(i=r+1;s[i]!='|';++i)
	{
		inputfilename[inp++]=s[i];
		s[i]=' ';
	}
	inputfilename[inp]='\0';
	sscanf(inputfilename,"%s",inputfilename);
	//puts(inputfilename);
	}
	
	if(q!=NULL)
	{
		r=q-&s[0];
		r++;
		s[r-1]=' ';
		for(i=r;i<l;++i)
		{
			outputfilename[out++]=s[i];
			s[i]=' ';
		}
		outputfilename[out]='\0';
		sscanf(outputfilename,"%s",outputfilename);
		
	}
	




		
	a[0]=strtok(s,"|"); //ls -l | head -n 10 | wc -l.
	i=0;
	while((a[++i]=strtok(NULL,"|"))!=NULL);
//	printf("%d\n",i);
	n=i-1;
	//a[0]=ls -l
	//a[1]=head -n 10
	//a[2]=wc -l
	//a[3]=NULL
	
	//char *args[200][200];
	
	for(j=0;j<100;++j)
	{
		for(k=0;k<100;++k)
	        args[j][k]=NULL;
		temp[j]=NULL;
	}

	for(j=0;j<i;++j)
	{
		args[j][0]=strtok(a[j]," ");
		k=0;
		while((args[j][++k]=strtok(NULL," "))!=NULL);
	}
	
	
//	args[0]={"ls","-l",NULL}
//	args[1]={"head","-n","10",NULL}
//	args[2]={"wc","-l",NULL}


	i=0;

		for(i=0;i<n;++i)
		{
			if(pipe(&fd[i*2])<0)
			{
				perror("Couldn't pipe");
				return ;
			}
		}

		int z=0;
		int ans;
		for(i=0;i<=n;++i)
		{
			ans=get_current_command(i);
		       // printf("ans: %d ",ans);
			pid=fork();
			if(pid==0)
			{
				if(i<n)
				{
					if(dup2(fd[z+1],1)<0)
					{
						perror(" Error in dup2");
						return ;
					}

				}
				else if(q!=NULL)
				{
					
		                int outp=open(outputfilename,O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
				if(outp<0)
				{
					perror("Error opening output file");
					return;
				}
				else
				{
					dup2(outp,1);
					close(outp);
				}
				}


				if(z)
				{
					if(dup2(fd[z-2],0)<0)
					{
						perror("Dup2 is causing error");
						return ;
					}
				}
				else if(p!=NULL)
				{
					int inpu=open(inputfilename,O_RDONLY);
		                        // int outp=open(outputfilename,O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
					 if(inpu<0)
					 {
						 perror("Error opening input file");
						 return ;
					 }
					 else
					 {
						 dup2(inpu,0);
						 close(inpu);
					 }
				}

				
				for(k=0;k<2*n;++k)
					close(fd[k]);
				/*if((strstr(temp,"<")!=NULL)||(strstr(temp,">")!=NULL))
				{
					redirect(temp);
					return ;
				}*/
				

				if(execvp(temp[0],temp)<0)
				{
					perror(temp[0]);
					return ;
				}
			}
				else if(pid<0)
				{
					perror("error");
					return ;
				}

				z+=2;
			}
		if(pid>0)
		{
		for(i=0;i<2*n;++i)
			close(fd[i]);
		int status;
		for(i=0;i<n+1;++i)
			wait(&status);
		}
}

int main()
{
	signal(SIGCHLD,sig_handler);
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	signal(SIGTSTP,sig_handler);
	pid_t paid;
	int i;
	char abs_curr_dir[60],*arr[60],str1[100];
	char system_name[60],buffer[60];
	gethostname(system_name,60);
	username=getenv("USER");
	getcwd(abs_curr_dir,120);
	curr_dir[0]='~';
	int l=strlen(abs_curr_dir),j=1;
	for(i=6+strlen(username);i<=l;++i)
		curr_dir[j++]=abs_curr_dir[i];


	//	printf("%d\n",l);
	int flag=0,control=1;
	char s[200],*str[200],buf[200],st[100],input_str[200];
	while(1)
	{
		//		if(control)
		printf("<%s@%s:%s> ",username,system_name,curr_dir);
		gets(s);

		char *p,*q;
		flag=0;
		if((p=strstr(s,"&"))==NULL)
			flag=1;
		else
			*p=' ';
                if(strstr(s,"|")!=NULL)
		{
			handle_pipe(s);
			continue;
		}
		

		else if((q=strstr(s,"<"))!=NULL)
		{
			redirect(s);
			continue;
		}
		
		else if((strstr(s,">"))!=NULL)
		{
			redirect(s);
			continue;
                }
		//------------------------------------------------------------------------------------------------------------------------------		
		strcpy(input_str,s);


		//For parsing the input 
		str[0]=strtok(s," ");
		if(!str[0])
			continue;
		//printf("%s\n",str[0]);
		for(i=1; ;++i)
		{
			str[i]=strtok(NULL," ");
			if(str[i]==NULL)
				break;
		}
		str[i]=NULL;
		//   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	Cd command  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//	printf("%d\n",i);
		//	puts(str[0]);

		if(!strcmp(str[0],"cd") )
		{
			if(i==1)
			{
				char zz[200];
				zz[0]='/';
				zz[1]='h';
				zz[2]='o';
				zz[3]='m';
				zz[4]='e';
				zz[5]='/';
				zz[6]='\0';
				strcat(zz,username);

				chdir(zz);
				curr_dir[0]='~';
				curr_dir[1]='\0';
			}
			else
			{

				int ret=chdir(str[1]);
				if(ret==-1)
					printf("The given folder name doesn't exist\n");
				getcwd(buf,100);
				get_path_name(buf);


			}
		}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~		

		else if(!strcmp(str[0],"quit"))
		{
			int j;
			for(j=start;j<end;++j)
			{
				if(queue[j].id)
				{
					kill(queue[j].id,9);
					queue[j].id=0;
				}
			}
			exit(0);
		}
		else if(!strcmp(str[0],"pinfo"))
		{
			paid=fork();
			int status;
			if(paid>0)
				wait(&status);
			else if(paid==0)
			{

				char path[100];
				int k;

				if(i==1)
					k=getpid();
				else
					k=atoi(str[1]);
				if(!k)
					puts("Invalid command ");
				else
					pinfo(k);
			}


		}
		else if(!strcmp(str[0],"jobs"))
		{

			int ct=1;
			for(j=start;j<end;++j)
			{
				if(queue[j].id==0)
					continue;
				printf("[%d] %s [%d]\n",ct++,queue[j].name,queue[j].id);
			}
			//	}

		}
		else if(!strcmp(str[0],"kjob"))
		{
			//paid=fork();
			//int status;
			//if(paid>0)
			//	wait(&status);
			//else if(paid==0)
			//{
			//    
			       if(i<3)
			       {
				       puts("Wrong usage");
				       continue;
			       }
				int x=atoi(str[1]);
				int y=atoi(str[2]);
				int k=kill(queue[start+x-1].id,y);
				if(k==-1)
					puts("Invalid input");
				else 
					puts("Killed");
				queue[start+x-1].id=0;
			//}
		}

		else if(!strcmp(str[0],"fg"))
		{
			char name[100];
			int ans=atoi(str[1]),y=0,t=0;
			//ans=start+ans-1;
			ans;
		
				for(j=0;j<end;++j)
				{
				if(queue[j].id)
					y++;
				if(y==ans)
				{

				pid_t pi=queue[j].id;
				strcpy(name,queue[j].name);
				fg(pi,name);
				t=1;
				break;
				}
				}
				if(!t)
				{
//				puts("Yak");
				queue[ans].id=0;
				}
			

		}
		else if(!strcmp(str[0],"overkill"))
		{
			int j;
			for(j=start;j<end;++j)
			{
				if(queue[j].id)
				{
					kill(queue[j].id,9);
					queue[j].id=0;
				}
			}
		}


		else
		{
			paid=fork();
			//Parent process
			if(paid>0)
			{
				cpid=paid;

				int status;
				if(flag==1)    //i.e. foreground
					waitpid(paid,&status,WUNTRACED);
				if(WIFSTOPPED(status))
				{
					kill(paid,SIGCONT);
					int j;
				}	


				else if(flag==0)              //Push the process into queue
				{
					printf( "Process id %d\n",paid); 
					process a;
					strcpy(a.name,input_str);
					a.id=paid;
					push(a);
				}
			}
			//Child process 
			else if(paid==0)
			{
				execvp(str[0],str);
				_exit(0);

			}
		}
	}





	return 0;
}

