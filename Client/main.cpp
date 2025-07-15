#include <algorithm>
#include <arpa/inet.h>
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <vector>

#include "ExpressionGenerator.h"
#include "utils.h"

int n;
int connections;
const char *_server_addr;
int server_port;
const int BUFFER_SIZE = 1024;
const int MAX_EVENTS = 10;

int epoll_fd;

void close_fds( int &epoll_fd, std::vector<int> &client_fds )
{
    for ( auto &client_fd : client_fds ) {
        close( client_fd );
    }
    close( epoll_fd );
}

int set_nonblocking( int fd )
{
    int flags = fcntl( fd, F_GETFL, 0 );
    if ( flags == -1 )
        return -1;
    return fcntl( fd, F_SETFL, flags | O_NONBLOCK );
}

void set_serv_addr( sockaddr_in &server_addr )
{
    memset( &server_addr, 0, sizeof( server_addr ) );
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons( server_port );
    inet_pton( AF_INET, _server_addr, &server_addr.sin_addr );
}

void run_clients( std::vector<int> &fds, int size )
{
    for ( int i = 0; i < size; i++ ) {

        int fd = socket( AF_INET, SOCK_STREAM, 0 );
        if ( fd == -1 ) {
            std::cerr << strerror( errno ) << std::endl;
            continue;
        }

        if ( set_nonblocking( fd ) == -1 ) {
            std::cerr << strerror( errno ) << std::endl;
        }

        fds.push_back( fd );
    }
}

void connect_clients( std::vector<int> &client_fds, sockaddr_in &server_addr )
{
    std::vector<int> connected_client_fds;

    for ( auto &client_fd : client_fds ) {
        if ( connect( client_fd, (sockaddr *)&server_addr, sizeof( server_addr ) ) == -1 ) {
            if ( errno != EINPROGRESS ) {
                std::cerr << strerror( errno ) << std::endl;
                close( client_fd );
                return;
            } else {
                connected_client_fds.push_back( client_fd );
            }
        } else {
            connected_client_fds.push_back( client_fd );
        }
    }

    client_fds = connected_client_fds;
}

void epoll_register( std::vector<int> &client_fds )
{
    for ( const auto &client_fd : client_fds ) {
        epoll_event event;
        event.events = EPOLLIN | EPOLLOUT | EPOLLET;
        event.data.fd = client_fd;

        epoll_ctl( epoll_fd, EPOLL_CTL_ADD, client_fd, &event );
    }
}

void handle_clients( int &epoll_fd, std::vector<int> &client_fds )
{
    ExpressionGenerator generator;
    epoll_event events[MAX_EVENTS];

    std::vector<std::string> sent_data( connections );
    std::vector<bool> sent_flags( connections, false );

    while ( ! client_fds.empty() ) {
        int number_of_events = epoll_wait( epoll_fd, events, MAX_EVENTS, -1 );

        for ( int i = 0; i < number_of_events; ++i ) {
            int fd = events[i].data.fd;

            auto it = std::find( client_fds.begin(), client_fds.end(), fd );
            int index = std::distance( client_fds.begin(), it );

            if ( ( events[i].events & EPOLLOUT ) && ! sent_flags[index] ) {
                generator.generate( n );

                sent_data[index] = "";

                std::vector<std::string> parts = generator.get_parts_expr();

                for ( const auto &part : parts ) {
                    if ( send( fd, part.c_str(), part.size(), 0 ) == -1 ) {
                        std::cerr << strerror( errno ) << std::endl;
                    } else {
                        sent_data[index] += part;
                    }
                }
                if ( send( fd, " ", 1, 0 ) == -1 ) {
                    std::cerr << strerror( errno ) << std::endl;
                } else {
                    sent_data[index] += " ";
                }

                sent_flags[index] = true;
            } else if ( events[i].events & EPOLLIN ) {
                char buffer[BUFFER_SIZE];
                int bytes_received = recv( fd, buffer, BUFFER_SIZE - 1, 0 );
                if ( bytes_received > 0 ) {
                    buffer[bytes_received] = '\0';
                    std::cout << "Sent:     " << sent_data[index] << std::endl;
                    std::cout << "Received: " << buffer << std::endl;
                    // processing wrong receivement
                    std::string correct_answer = "";
                    if ( !answer_is_correct( sent_data[index], buffer, correct_answer )){
                        std::cerr << "Invalid answer!\nCorrect:" << correct_answer << std::endl;
                    }
                }

                epoll_ctl( epoll_fd, EPOLL_CTL_DEL, fd, nullptr );
                close( fd );
                client_fds.erase( it );
                sent_data.erase( sent_data.begin() + index );
                sent_flags.erase( sent_flags.begin() + index );
            }
        }
    }
}

int main( int argc, char **argv )
{
    if ( argc == 5 ) {
        n = std::stoi( argv[1] );
        std::cout << "n: '" << n << "'\n";
        connections = std::stoi( argv[2] );
        std::cout << "connections: '" << connections << "'\n";
        _server_addr = argv[3];
        std::cout << "server_addr: '" << _server_addr << "'\n";
        server_port = std::stoi( argv[4] );
        std::cout << "server_port: '" << server_port << "'\n\n";
    } else {
        return -1;
    }

    srand( time( 0 ) );

    sockaddr_in server_addr;
    set_serv_addr( server_addr );

    std::vector<int> client_fds;
    run_clients( client_fds, connections );

    connect_clients( client_fds, server_addr );

    epoll_fd = epoll_create1( 0 );
    epoll_register( client_fds );

    // send and receive data
    handle_clients( epoll_fd, client_fds );

    close_fds( epoll_fd, client_fds );

    return 0;
}