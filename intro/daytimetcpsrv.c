#include	"unp.h"
#include <time.h>
int main(int argc, char **argv)
{
	printf("test servaddr.sin_addr.s_addr 123464681");
	int listenfd, connfd;
	// 网际套接口地址结构体
	// sockaddr_in 结构体解决了sockaddr的缺陷，把port和addr 分开储存在两个变量中
	struct sockaddr_in	servaddr;
	char buff[MAXLINE];
	time_t ticks;

	// 创建套接口
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;

	// host to network long
	// sin_addr 32位ip地址 s_addr 32位ipv4地址
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // INADDR_ANY一般为内核指定的,大多数系统取0,表示任意的IP地址.
	servaddr.sin_port        = htons(13);	/* daytime server */	
	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	// 把当前套接口变成监听套接口
	// LISTENQ 可排队的最大客户连接数
	Listen(listenfd, LISTENQ); 

	for ( ; ; ) {
		// 1.等待用户连接,进入睡眠状态
		// 2.三路握手完毕则返回一个新的描述符
		connfd = Accept(listenfd, (SA *) NULL, NULL);

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        Write(connfd, buff, strlen(buff));

		Close(connfd);
	}
}
