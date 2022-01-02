#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <time.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include<fcntl.h>
#include <sqlite3.h>

typedef struct sockaddr *(SA);
typedef enum {TYPE_HTML,TYPE_JPG,TYPE_GIF,TYPE_PNG} FILE_TYPE;
unsigned char FromHex(unsigned char x)
{
	if(0 == x ) 
	{
		return -1;
	}
	unsigned char y;
	if(x>='A' &&x<='Z') 
	{
		y = x-'A'+10;
	}
	else if(x>='a' &&x <='z') 
	{
		y = x-'a'+10;
	}
	else if(x>='0' && x<='9') 
	{
		y = x-'0';
	}

	return y;
}
int  urlDecode(  char* dest, const char* src)
{
	if(NULL ==src || NULL == dest)
	{
		return -1;

	}
	int len = strlen(src);
	int i =0 ;
	for(i = 0 ;i<len;i++)
	{

		if('+' == src[i]) strcat(dest,"");
		else if('%'==src[i])
		{

			unsigned char high = FromHex((unsigned char)src[++i]);
			unsigned char low = FromHex((unsigned char)src[++i]);
			unsigned char temp = high*16 +low;
			char temp2[5]={0};
			sprintf(temp2,"%c",temp);
			strcat(dest , temp2);
		}
	}
	return 0;
}

int show(void *arg, int col, char **result, char **title)
{	
	*(int *)arg = 1;
	return 0;
}


int show_one_goods (void * arg, int col,char ** result, char ** title)
{
	FILE * fp = (FILE *)arg;
	fprintf (fp,"<img src=\"%s\" align = 'center' width=\"25%%\" height=\"2%%\"><br>\n",result[5]);
	fprintf (fp,"<mark><h3>%s<br><br><h3></mark>\n",result[1]);
	fprintf (fp,"<del align = 'center'>原价：%s</del><br>\n",result[6]);
	fprintf (fp,"现价：%s<br><br>\n",result[2]);
	fprintf (fp,"%s<br><br>\n",result[3]);
	fprintf (fp,"<h3 align = 'right'>%s</h3><br>\n",result[4]);
	fflush(fp);
	return 0;
}

int sql_one_goods(char *goodsname, FILE * fp)
{
	sqlite3 * db;
	int ret = sqlite3_open("/home/linux/c_Language/web-shopping/123.db",&db);
	if (SQLITE_OK != ret)
	{
		fprintf (stderr, "open error:%s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}
	char sql_cmd[512] = {0};
	sprintf (sql_cmd,"select goods_id,goods_name,shop_price,keywords,goods_desc,goods_img,market_price from goods where goods_id = %s;",goodsname);

	char *errmsg;
	ret = sqlite3_exec(db, sql_cmd,show_one_goods,fp,&errmsg);
	if (SQLITE_OK != ret)
	{
		fprintf(stderr,"exec error:%s\n",errmsg);
		sqlite3_free(errmsg);
		sqlite3_close(db);
		return 1;
	}
	return 0;
}

int one_goods_interface(char * arg)
{
	FILE * src = fopen("9.html","r");
	FILE * fp = fopen("8.html","w");
	if (NULL == fp || NULL == src)
	{
		perror("fopen");
		exit(1);
	}
	int i = 0;
	for (i = 0; i<=8; ++i)
	{
		char buf[256] = {0};
		fgets(buf,256,src);
		fputs(buf,fp);
		fflush(fp);
	}

	char * goodsname = NULL;
	goodsname = index(arg, '_');
	goodsname += 1;

	sql_one_goods(goodsname,fp);

	
	fprintf(fp," \n");
	fprintf(fp,"</body>\n");
	fprintf(fp,"</html>");
	fclose(fp);
	return 0;
}

int show_goods (void * arg, int col,char ** result, char ** title)
{
	FILE * fp = (FILE *)arg;
	fprintf (fp,"<h3 align='center'><a href = 'detail_%s'><img src=\"./%s\" width=\"15%%\" height=\"2%%\"></a></h3>&ensp;",result[0],result[2]);
	fflush(fp);
	return 0;
}

int sql_search(char *goodsname, FILE * fp)
{
	sqlite3 * db;
	int ret = sqlite3_open("/home/linux/c_Language/web-shopping/123.db",&db);
	if (SQLITE_OK != ret)
	{
		fprintf (stderr, "open error:%s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}
	char sql_cmd2[512] = {0};
	sprintf (sql_cmd2,"select goods_id,goods_name,goods_img from goods where goods_name like \"%%%s%%\";",goodsname);

	char *errmsg;
	ret = sqlite3_exec(db, sql_cmd2,show_goods,fp,&errmsg);
	if (SQLITE_OK != ret)
	{
		fprintf(stderr,"exec error:%s\n",errmsg);
		sqlite3_free(errmsg);
		sqlite3_close(db);
		return 1;
	}
	return 0;
}

int goods_interface(char * arg)
{
	FILE * src = fopen("./6.html","r");
	FILE * fp = fopen("./7.html","w");
	if (NULL == fp|| NULL == src)
	{
		perror("fopen");
		exit(1);
	}
	int i = 0;
	for (i = 0; i<=8; ++i)
	{
		char buf[256] = {0};
		fgets(buf,256,src);
		fputs(buf,fp);
		fflush(fp);
	}
	fprintf(fp,"<h1 align='center'>手机商城</h1>\n");
	fprintf(fp,"<br>\n");
	fprintf(fp,"<br>\n");
	fprintf(fp,"<br>\n");
	fprintf(fp,"<br>\n");
	fprintf(fp,"<br>\n");
	fprintf(fp,"<br>\n");
	fprintf(fp,"<br>\n");
	char * goodsname = NULL;
	goodsname = index(arg, '=');
	goodsname += 1;
	char new_goodsname[512] = {0};
	urlDecode(new_goodsname, goodsname);

	sql_search(new_goodsname,fp);
	
	fprintf(fp," \n");
	fprintf(fp,"</body>\n");
	fprintf(fp,"</html>");
	fclose(fp);
	return 0;
}



int send_head(int conn,char *filename,FILE_TYPE type)
{
	struct stat st;
	int ret = stat(filename,&st);
	if(-1 == ret)
	{
		perror("stat");
		return 1;
	}
	char *arg[6] = {0};
	arg[0] = "HTTP/1.1 200 OK\r\n";
	arg[1] = "Server: MYWEBSERVER\r\n";
	arg[2] = "Date: Fri, 27 Aug 2021 05:56:39 GMT\r\n";
	switch(type)
	{
		case TYPE_HTML:
			arg[3] = "Content-Type: text/html; charset=UTF-8\r\n";
			break;
		case TYPE_JPG:
			arg[3] = "content-type: image/jpeg\r\n";
			break;
		default:
			arg[3] = "Content-Type: text/html; charset=UTF-8\r\n";


	}
	//arg[3] = "Content-Type: text/html; charset=UTF-8\r\n";
	char buf[512] = {0};
	arg[4] = buf;
	sprintf(buf,"content-length: %lu\r\n",st.st_size);
//	arg[4] = "content-length: 1076\r\n";
	arg[5] = "Connection: closed\r\n\r\n";
	int i = 0;
	for(i = 0;i < 6;i++)
	{
		send(conn,arg[i],strlen(arg[i]),0);
	}
}

int send_file(int conn,char *filename,FILE_TYPE type)
{
	send_head(conn,filename,type);
	int fd = open(filename,O_RDONLY);
	while(1)
	{	
		char buf[4096] = {0};
		bzero(buf,sizeof(buf));
		int rd_ret = read(fd,buf,sizeof(buf));
		if(0 == rd_ret)
		{
			break;
		}
		send(conn,buf,rd_ret,0);
	}
	close(fd);
	return 0;
}
int main(int argc, const char *argv[])
{
	int listfd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == listfd)
	{
		perror("socket error\n");
		exit(1);
	}
	struct sockaddr_in ser,cli;
	bzero(&ser,sizeof(ser));
	bzero(&cli,sizeof(cli));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(80);
	ser.sin_addr.s_addr = INADDR_ANY;
	int ret = bind(listfd,(SA)&ser,sizeof(ser));
	if(-1 == ret)
	{
		perror("bind error\n");
		exit(1);
	}
	listen(listfd,3);
	int len = sizeof(cli);

	sqlite3* db=NULL;
	ret = sqlite3_open("/home/linux/c_Language/web-shopping/bbb.db",&db);
	if(SQLITE_OK != ret)
	{
		fprintf(stderr,"open error,%s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}
	fd_set rd_set,temp_set;
	FD_ZERO(&rd_set);
	FD_ZERO(&temp_set);

	FD_SET(listfd,&temp_set);
	int maxfd = listfd;
	int i = 0;
	while(1)
	{
		rd_set = temp_set;
		select(maxfd+1,&rd_set,NULL,NULL,NULL);
		for(i=listfd;i<maxfd+1;i++)
		{
			if(FD_ISSET(i,&rd_set) && i == listfd)//listfd
			{
				int conn = accept(listfd,(SA)&cli,&len);
				if(-1 == conn)
				{
					perror("accept");
					exit(1);
				}
				FD_SET(conn,&temp_set);
				if(conn>maxfd)
				{
					maxfd = conn;
				}

			}
			if(FD_ISSET(i,&rd_set)&& i!=listfd)//  !listfd
			{
				char buf[256] = {0};
				ret = recv(i,buf,sizeof(buf),0);
				if(ret == 0)
				{
					close(i);
					continue;
				}
				printf("%s\n",buf);
				fflush(stdout);
				printf("---------------------\n");
				//GET / HTTP/1.1
				char *arg[3] = {NULL};
				arg[0] = strtok(buf," ");
				arg[1] = strtok(NULL," ");
				arg[2] = strtok(NULL,"\r");

				if(0 == strcmp(arg[1],"/"))
				{
					send_file(i,"1.html",TYPE_HTML);
				}
				//GET /login?um=123&pw=123\0
				if(0 == strncmp(arg[1],"/login",6))
				{
					char *name=NULL;
					char *pw=NULL;
					char *end=NULL;
					name = index(arg[1],'=');
					name+=1;
					end = index(name,'&');
					*end = '\0';

					pw = index(end+1,'=');
					pw+=1;
					
					int data = 0;
					char* errmsg1=NULL;
					char sql_cmd1[512]={0};
					sprintf(sql_cmd1,"select * from user where username like '%s';",name);
					ret = sqlite3_exec(db,sql_cmd1,show,&data,&errmsg1);//not select
					if(SQLITE_OK != ret)
					{
						fprintf(stderr,"exec %s,%s\n",sql_cmd1 ,errmsg1);
						sqlite3_free(errmsg1);
						sqlite3_close(db);
						return 1;
					}

					if(1 == data)
					{
						send_file(i,"2.html",TYPE_HTML);	
					}
					else
					{
						send_file(i,"4.html",TYPE_HTML);
					}


					/*if(0 == strcmp(name,"zhangsan") 
							&& 0 == strcmp(pw,"123"))
					{
						send_file(i,"2.html",TYPE_HTML);	
					}
					else
					{
						send_file(i,"4.html",TYPE_HTML);
					}
					*/

				}
				//GET /5.html HTTP/1.1
				if(0 == strncmp(arg[1],"/5.html",7))
				{
					send_file(i,"5.html",TYPE_HTML);	
				}

				//GET /register?um=shishaocong250&pw=1234&pw=1234 HTTP/1.1
				if(0 == strncmp(arg[1],"/register",9))
				{
					char *um=NULL;
					char *pw1=NULL;
					char *end1=NULL;
					char *end2=NULL;
					//printf("1111111111111\n");
					um = index(arg[1],'=');
					//printf("1111111111111\n");
					um+=1;
					//printf("1111111111111\n");
					end1 = index(um,'&');
					//printf("1111111111111\n");
					*end1 = '\0';
					//printf("1111111111111\n");
					//printf("1111111111111\n");
					pw1 = index(end1+1,'=');
					pw1+=1;
					end2 = index(pw1,'&');
					*end2 = '\0';
					//printf("1111111111111\n");
			
					char* errmsg=NULL;
					char sql_cmd[512]={0};
					sprintf(sql_cmd,"insert into user values (NULL,'%s',%s);",um,pw1);
					ret = sqlite3_exec(db,sql_cmd,NULL,NULL,&errmsg);//not select
					if(SQLITE_OK != ret)
					{
						fprintf(stderr,"exec %s,%s\n",sql_cmd ,errmsg);
						sqlite3_free(errmsg);
						sqlite3_close(db);
						return 1;
					}
					sleep(2);
					send_file(i,"1.html",TYPE_HTML);
					
					continue;
					sqlite3_close(db);
					
				}

				//GET /1.html HTTP/1.1
				if(0 == strncmp(arg[1],"/1.html",7))
				{
					send_file(i,"1.html",TYPE_HTML);	
				}
				//GET /search?name=%E4%B8%89%E6%98%9F HTTP/1.1
				if(0 == strncmp(arg[1],"/search",7))
				{
					goods_interface(arg[1]);		
					send_file(i,"7.html",TYPE_HTML);
				}

				//GET /detail_19 HTTP/1.1
				if(0 == strncmp(arg[1],"/detail",7))
				{
					one_goods_interface(arg[1]);
					send_file(i,"8.html",TYPE_HTML);
				}


				// GET /1.jpg HTTP/1.1
				if(strstr(arg[1],".jpg"))
				{
					send_file(i,arg[1]+1,TYPE_JPG);
				}

				close(i);
				}
			}
		}


	//close(conn);
	close(listfd);
	return 0;
}
