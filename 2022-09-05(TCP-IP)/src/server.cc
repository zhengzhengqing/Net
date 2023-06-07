#include <memory>
#include <vector>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <string.h>
#include <iostream>
#include <dirent.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

const int PATH_SIZE = 200;
const int BUFFER_SIZE = 1024 * 1024;
const int MAX_EVENT_NUMBER = 10000;
const int FILE_NAME = 200;


class Server
{
    public:
        explicit Server(const string & ip, const int port):ip_(ip),port_(port)
        {
            init_socket();
            event_loop();
        }

        ~Server()
        {
            close(epollfd_);
            close(listenfd_);
        }

        void init_socket()
        {
            listenfd_ = socket(PF_INET, SOCK_STREAM, 0);
            assert(listenfd_ >= 0);

            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = inet_addr(ip_.c_str());
            server_addr.sin_port = htons(port_);

            int flag = 1;
            setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
            int ret = bind(listenfd_, (struct sockaddr*)&server_addr, sizeof(server_addr));
            assert(ret >= 0);

            ret = listen(listenfd_, 5);
            assert(ret >= 0);

            epollfd_ = epoll_create(5);
            assert(epollfd_ > 0);

            add_fd(epollfd_ , listenfd_);
        }

        void event_loop()
        {
            while(true)
            {
                int number = epoll_wait(epollfd_, m_events,MAX_EVENT_NUMBER, -1);
                
                if(number < 0)
                {
                    if(number < 0 && errno != EINTR)
                        break;
                }
                for(int i = 0; i < number; i ++)
                {
                    if(m_events[i].data.fd == listenfd_)
                    {
                        cout <<"got a connection" <<endl;
                        new_connection(epollfd_, listenfd_); // 处理新连接
                    }
                    else if(m_events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
                    {
                        cout <<"close a fd" <<endl;
                        delete_fd(epollfd_,  m_events[i].data.fd);
                        close(m_events[i].data.fd);
                    }
                    
                    else if(m_events[i].events & EPOLLIN) // 读事件到来
                    {
                        //char DownFileName[FILE_NAME];
                        //DIR *dp = nullptr;
                        //struct dirent *dirp;
                        cout <<"recv a data" <<endl;
                        //if((dp = opendir("/home/zzq/unix/bigFileTest")) == nullptr)
                        //    cout <<"connot open\n" <<endl;
                        send_file(m_events[i].data.fd);
                    }
                }
            }
        }

        void set_non_blocking(int fd)
        {
            int old_option = fcntl(fd, F_GETFL);
            int new_option = old_option | O_NONBLOCK;
            fcntl(fd, F_SETFL, new_option);
        }

        void add_fd(int epollfd, int fd)
        {
            epoll_event event;
            event.data.fd = fd;
            event.events = EPOLLIN | EPOLLET | EPOLLRDHUP ;
            set_non_blocking(fd);
            int res = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
            assert(res >= 0);
        }

        void delete_fd(int epollfd,int fd)
        {
            int res = epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
            assert(res >= 0);
        }

        void new_connection(int epollfd, int fd)
        {
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);

            int newConnFd = accept4(fd, (struct sockaddr *)&client_addr,
                            &client_addr_len, SOCK_NONBLOCK | SOCK_CLOEXEC);
            
            add_fd(epollfd, newConnFd);
        }

        void send_file(int fd)
        {
            char file_name[FILE_NAME] = "/home/zzq/unix/bigFileTest/unix.pdf";
            char buffer[BUFFER_SIZE];      
            char Path[PATH_SIZE];
            
        
            printf("%s\n", file_name);  
            sprintf(Path, "%s", file_name);       //此路径可以根据自己需要修改
            // 打开文件并读取文件数据      
            FILE *fp = fopen(Path, "r");   

            if(fp == NULL)
            {
                cout <<"can not open the file errno = " <<errno <<endl;
                return ;
            }

            bzero(buffer, BUFFER_SIZE);    
            int length = 0;

            while((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)        
            {          
                printf("length = %d\n", length);
                if(send(fd, buffer, length, 0) < 0)          
                {            
                    printf("Send File:%s Failed.\n", file_name);     
                    
                    if(errno == EAGAIN)
                    {
                        cout <<" errno = " <<errno <<endl;
                        sleep(1);
                        while(send(fd, buffer, length, 0) < 0)
                        {
                            cout <<"sleep" <<endl;
                            sleep(1);
                        }
                    }    
                }          
                bzero(buffer, BUFFER_SIZE);        
            }
            sleep(1);
            cout <<"all the data has been sended " <<endl;
            fclose(fp);
            close(fd);
        }

    private:
    
        string ip_;
        int port_;
        int listenfd_;
        int epollfd_;
        struct sockaddr_in server_addr;
        epoll_event m_events[MAX_EVENT_NUMBER];
};

int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        printf("Usage: ./server [ip] [port]\n");
        return 1;
    }

    Server server(argv[1],atoi(argv[2]));
    return 0;
}