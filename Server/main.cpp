#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ExpressionManager.h"

int server_port;
const int BUFFER_SIZE = 1024;
const int MAX_EVENTS = 10;

sockaddr_in server_addr;
int server_fd;
int epoll_fd;

void close_fds()
{
    close( server_fd );
    close( epoll_fd );
}

void handle_signal( int signal )
{
    close_fds();

    exit( 0 );
}

int set_nonblocking( int fd )
{
    int flags = fcntl( fd, F_GETFL, 0 );
    if ( flags == -1 )
        return -1;
    return fcntl( fd, F_SETFL, flags | O_NONBLOCK );
}

void set_serv_addr()
{
    memset( &server_addr, 0, sizeof( server_addr ) );
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons( server_port );
}

void run_server()
{
    server_fd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( server_fd == -1 ) {
        std::cerr << strerror( errno ) << std::endl;
    }

    if ( set_nonblocking( server_fd ) == -1 ) {
        std::cerr << strerror( errno ) << std::endl;
    }

    if ( bind( server_fd, (sockaddr *)&server_addr, sizeof( server_addr ) ) == -1 ) {
        std::cerr << strerror( errno ) << std::endl;
    }

    if ( listen( server_fd, SOMAXCONN ) == -1 ) {
        std::cerr << strerror( errno ) << std::endl;
    }
}

void epoll_register()
{
    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_fd;

    if ( epoll_ctl( epoll_fd, EPOLL_CTL_ADD, server_fd, &event ) == -1 ) {
        std::cerr << strerror( errno ) << std::endl;
    }
}

void handle_clients()
{
    epoll_event events[MAX_EVENTS];

    std::map<int, std::string> clients_data;

    std::string big_buffer;

    while ( true ) {

        int number_of_events = epoll_wait( epoll_fd, events, MAX_EVENTS, -1 );

        for ( int i = 0; i < number_of_events; i++ ) {
            int fd = events[i].data.fd;

            if ( fd == server_fd ) {

                socklen_t client_len = sizeof( sockaddr_in );
                int client_fd = accept( server_fd, nullptr, &client_len );
                if ( client_fd == -1 ) {
                    std::cerr << strerror( errno ) << std::endl;
                }

                if ( set_nonblocking( client_fd ) == -1 ) {
                    std::cerr << strerror( errno ) << std::endl;
                }

                epoll_event client_event;
                client_event.events = EPOLLIN;
                client_event.data.fd = client_fd;

                if ( epoll_ctl( epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event ) == -1 ) {
                    std::cerr << strerror( errno ) << std::endl;
                }

                clients_data.emplace( client_fd, "" );
            } else {

                char buffer[BUFFER_SIZE];
                int bytes_received = recv( fd, buffer, BUFFER_SIZE - 1, 0 );
                if ( bytes_received <= 0 ) {

                    clients_data.at( fd ).clear();

                    if ( epoll_ctl( epoll_fd, EPOLL_CTL_DEL, fd, nullptr ) == -1 ) {
                        std::cerr << strerror( errno ) << std::endl;
                    }

                    if ( close( fd ) == -1 ) {
                        std::cerr << strerror( errno ) << std::endl;
                    }

                } else {

                    buffer[bytes_received] = '\0';
                    std::string temp = buffer;
                    clients_data.at( fd ) += temp;
                    std::cout << "Received from client_" << fd << ": " << temp << std::endl;

                    if ( clients_data.at( fd ).find( ' ' ) != std::string::npos ) {
                        ExpressionManager em( clients_data.at( fd ) );
                        std::string answer = std::to_string( em.get_answer() );
                        if ( send( fd, answer.c_str(), answer.size(), 0 ) == -1 ) {
                            std::cerr << "Send error: " << strerror( errno ) << std::endl;
                        }
                    }
                }
            }
        }
    }
}

int main( int argc, char **argv )
{
    if ( argc == 2 ) {
        server_port = std::stoi( argv[1] );
        std::cout << "server_port: '" << server_port << "'\n\n";
    } else {
        return -1;
    }

    std::signal( SIGINT, handle_signal );

    set_serv_addr();

    run_server();

    epoll_fd = epoll_create1( 0 );
    epoll_register();

    // send and receive data
    handle_clients();

    close_fds();

    return 0;
}