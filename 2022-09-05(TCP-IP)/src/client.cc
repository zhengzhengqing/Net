#include <stdio.h>  
#include <string>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream> 
using namespace std; 

const int FILE_NAME = 200;
const int BUFFER_SIZE = 1024 * 1024;

class Client
{
    public:
        Client(const string & ip, const int port):ip_(ip), port_(port)
        {
            init_socket();
        }

        void init_socket()
        {
            bzero(buffer, BUFFER_SIZE);    
            sock_fd = socket(AF_INET,SOCK_STREAM,0);
            assert(sock_fd);
            cout <<"port = " <<port_ <<endl;
            cout <<"ip = " <<ip_ <<endl;
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(port_);
            server_addr.sin_addr.s_addr = inet_addr(ip_.c_str());
            int ret = connect(sock_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));
            assert(ret >= 0);

            size_t length = 0;
            while((length = recv(sock_fd, buffer, BUFFER_SIZE, 0)) > 0)    
            {      
                // printf("length = %ld\n", length);
                // if(strcmp(buffer, "OK") == 0)
                // {
                //     break;
                // }
                // if(fwrite(buffer, sizeof(char), length, fp) < length)      
                // {        
                //     printf("File:\t%s Write Failed\n", file_name);        
                //     break;      
                // }      
                // bzero(buffer, BUFFER_SIZE);    //在这里注意使用清空函数，否则会发生意想不到的错误   
            }       
            close(sock_fd);
            // 接收成功后，关闭文件，关闭socket    
            //fclose(sock_fd);    
            
            // string str("nihao");
            // send(fd,str.c_str(),str.size(), 0);
            // recvfile(fd);
            // close(fd);
        }

        void recv_file(int fd)
        {
            char file_name[FILE_NAME] = "/home/zzq/unix/recvBigFile/files/unix.pdf";
	       
            // 向服务器发送buffer中的数据    
        
            FILE *fp = fopen(file_name, "w");    
            if(NULL == fp)    
            {      
                cout <<" Can Not Open To Write" <<endl;     
                exit(1);    
            }       
            // 从服务器接收数据到buffer中    
            // 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止    
            
            
            size_t length = 0;  
            cout <<"waiting recv" <<endl;
            while((length = recv(fd, buffer, BUFFER_SIZE, 0)) > 0)    
            {      
                printf("length = %ld\n", length);
                if(strcmp(buffer, "OK") == 0)
                {
                    break;
                }
                if(fwrite(buffer, sizeof(char), length, fp) < length)      
                {        
                    printf("File:\t%s Write Failed\n", file_name);        
                    break;      
                }      
                bzero(buffer, BUFFER_SIZE);    //在这里注意使用清空函数，否则会发生意想不到的错误   
            }       
            // 接收成功后，关闭文件，关闭socket    
            fclose(fp);    
        }

    private:
        string ip_;
        int port_;
        int sock_fd;
        char buffer[BUFFER_SIZE];
        struct sockaddr_in server_addr;
};





int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        printf("Usage: ./client [ip] [port]\n");
        return 1;
    }
    
    Client client(argv[1],atoi(argv[2]));
    return 0;
}