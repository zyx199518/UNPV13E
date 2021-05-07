#include "unp.h"
#include "errno.h"

// #define TEST
#ifdef TEST
int main(int argc, char **argv) {
	if (argc != 2) {
		err_quit("参数数量错误!, 错误码%d", errno);
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);  
	if (sockfd < 0) { // 返回-1
		err_quit("创建失败客户端套接字失败!, 错误码%d", errno);
	}

	struct sockaddr_in serv_addr;
	bzero((void*)&serv_addr, sizeof(serv_addr));
	// 初始化服务端套接口地址结构
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(13);
	if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) < 0) {  // 返回-1
		err_quit("服务器ip地址格式错误,转换网络字节序失败!, 错误码%d", errno);
	}

	// connect 激发三路握手
	if (connect(sockfd, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		// 返回-1
		err_quit("连接失败 错误码%d", errno);
	}

	int n;
	char buffer[MAXLINE + 1]; // 4096
	while((n = read(sockfd, buffer, MAXLINE)) > 0) {
		buffer[n] = 0;
		if (fputs(buffer, stdout) == EOF) {
			err_quit("输出失败 错误码 %d", errno);
		}
	}

	if (n == 0) {
		err_quit("到达文件末尾 %d", errno);
	} else {
		err_quit("读取出错 %d", errno);
	}

	exit(0); // 程序正常退出
}

#else

int main(int argc, char **argv)
{
	int sockfd, n;
	char recvline[MAXLINE + 1];
	struct sockaddr_in servaddr; // 网际套接口地址结构体

	printf("test %s\n", argv[0]);
	printf("test %s\n", argv[1]);

	union {
		int a;
		char b;
	}tmp;

	tmp.a = 1;

	if (tmp.b == 0) {
		printf("big endian \n");
	} else if (tmp.b == 1) {
		printf("little endian \n");
	}

	if (argc != 2){
		// 分不同情况将出错消息将按指定的级别（LOG_ERR）传递给syslog或者输出到标准错误输出
		// 函数结尾使用exit（1）
		err_quit("usage: a.out <IPaddress>");
	}

	// AF_前缀代表地址族
	// AF_INET = ipv4 AF_INET6 = ipv6
	// SOCK_STREAM（流格式套接字/面向连接的套接字）或者 SOCK_DGRAM（数据报套接字/无连接的套接字）
	// IPPROTO_TCP、IPPTOTO_UDP、IPPROTO_SCTP、IPPROTO_TIPC或者 使用0自动推演
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		err_sys("socket error");
	}

	// 用于结构体清零 void* len
	bzero(&servaddr, sizeof(servaddr));

	// AF_INET(ipv4协议)
	servaddr.sin_family = AF_INET;  

	// sin_port是一个网络字节序的16位值
	// host to network short 大端(IBM)存储转小端网络字节序(power pc)
	servaddr.sin_port = htons(0x000d);

	// 点分文本的IP地址(argv[1])转换为二进制网络字节序”的IP地址(servaddr.sin_addr)
	// 返回值：若成功则为1，若输入不是有效的表达式则为0，若出错则为-1
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
		err_quit("inet_pton error for %s", argv[1]);
	}

/*
	参数：
	第一个参数：int sockdf：
				socket文件描述符
	第二个参数： const struct sockaddr *addr：
				传入参数，指定服务器端地址信息，含IP地址和端口号
	第三个参数：socklen_t addrlen：
				传入参数,传入sizeof(addr)大小
*/

	// connect激发TCP三路握手
	if (connect(sockfd, (const struct sockaddr*/* SA* */) &servaddr, sizeof(servaddr)) < 0) {
		err_sys("connect error");
	}

	while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {  // strlen + 1
		recvline[n] = 0;	/* null terminate */
		if (fputs(recvline, stdout) == EOF) {
			err_sys("fputs error");
		}
	}

	if (n < 0) {
		err_sys("read error");
	}

	exit(0);
}

#endif



/* http://c.biancheng.net/view/2131.html 
https://elixir.bootlin.com/linux/latest/source
*/