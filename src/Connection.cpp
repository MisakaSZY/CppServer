#include "Connection.h"

#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <utility>

#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "util.h"

Connection::Connection(EventLoop* _loop, Socket* _sock) : loop(_loop), sock(_sock) {
    if (loop != nullptr) {
        channel = new Channel(loop, sock->get_fd());
        channel->enable_read();
        channel->use_et();
    }
    read_buffer = new Buffer();
    send_buffer = new Buffer();
    state = State::Connected;
}

Connection::~Connection() {
    if (loop != nullptr) {
        delete channel;
    }
    delete sock;
    delete read_buffer;
    delete send_buffer;
}

void Connection::read() {
    assert((state == State::Connected) && ("Connection state is disconnected!"));
    read_buffer->clear();
    if (sock->is_nonblocked()) {
        read_nonblocked();
    } else {
        read_blocked();
    }
}

void Connection::write() {
    assert((state == State::Connected) && ("Connection state is disconnected!"));
    if (sock->is_nonblocked()) {
        write_nonblocked();
    } else {
        write_blocked();
    }
    send_buffer->clear();
}

// 非阻塞读取，服务端使用
void Connection::read_nonblocked() {
    int sockfd = sock->get_fd();
    char buf[1024];
    while (true) {  // 非阻塞循环读取
        memset(buf, 0, sizeof(buf));
        ssize_t read_size = ::read(sockfd, &buf, sizeof(buf));
        if (read_size > 0) {
            read_buffer->append(buf, read_size);
        } else if (read_size == -1 && errno == EINTR) {  // 正常中断，继续读取
            printf("continue reading...");
            continue;
        } else if (read_size == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // 非阻塞IO，这个条件表示数据全部读取完毕
            break;
        } else if (read_size == 0) {  // EOF，客户端断开连接
            printf("read EOF, client fd %d disconnected\n", sockfd);
            state = State::Closed;
            break;
        } else {
            printf("Other error on client fd %d\n", sockfd);
            state = State::Closed;
            break;
        }
    }
}

// 非阻塞写，服务端使用
void Connection::write_nonblocked() {
    int sockfd = sock->get_fd();
    char buf[send_buffer->size()];
    memcpy(buf, send_buffer->c_str(), send_buffer->size());
    int data_size = send_buffer->size();
    int left_size = data_size;
    while (left_size > 0) {
        ssize_t write_size = ::write(sockfd, buf + data_size - left_size, left_size);
        if (write_size == -1 && errno == EINTR) {  // 被中断，继续写入
            printf("continue write...");
            continue;
        }
        if (write_size == -1 && errno == EAGAIN) {
            break;
        }
        if (write_size == -1) {
            printf("Other error on client fd %d\n", sockfd);
            state = State::Closed;
            break;
        }
        left_size -= write_size;
    }
}

// 阻塞读取，客户端使用
void Connection::read_blocked() {
    int sockfd = sock->get_fd();
    unsigned int rcv_size = 0;
    socklen_t len = sizeof(rcv_size);
    // 获取接收缓冲区大小
    getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &len);

    char buf[rcv_size];
    ssize_t read_size = ::read(sockfd, buf, sizeof(buf));
    if (read_size > 0) {
        read_buffer->append(buf);
    } else if (read_size == 0) {
        printf("read EOF, blocking client fd %d disconnected\n", sockfd);
        state = State::Closed;
    } else if (read_size == -1) {
        printf("Other error on blocking client fd %d\n", sockfd);
        state = State::Closed;
    }
}

// 阻塞写入，客户端使用
void Connection::write_blocked() {
    // 没有处理send_buffer_数据大于TCP写缓冲区的情况
    int sockfd = sock->get_fd();
    ssize_t write_size = ::write(sockfd, send_buffer->c_str(), send_buffer->size());
    if (write_size == -1) {
        printf("Other error on blocking client fd %d\n", sockfd);
        state = State::Closed;
    }
}

Connection::State Connection::get_state() { return state; }
void Connection::close() { delete_connection_callback(sock); }
void Connection::set_send_buffer(const char* str) { send_buffer->set_buf(str); }
void Connection::getline_send_buffer() { send_buffer->getline(); }
Buffer* Connection::get_read_buffer() { return read_buffer; }
Buffer* Connection::get_send_buffer() { return send_buffer; }
const char* Connection::read_buffer_str() { return read_buffer->c_str(); }
const char* Connection::send_buffer_str() { return send_buffer->c_str(); }
Socket* Connection::get_socket() { return sock; }

void Connection::set_delete_connection_callback(const std::function<void(Socket*)>& _cb) {
    delete_connection_callback = _cb;
}

void Connection::set_on_connect_callback(const std::function<void(Connection*)>& _cb) {
    on_connect_callback = _cb;
    channel->set_read_callback([this]() {
        on_connect_callback(this);
    });
}
