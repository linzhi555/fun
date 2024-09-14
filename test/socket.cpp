#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];
    read(client_sock, buffer, sizeof(buffer) - 1);
    
    // 输出请求内容（调试用）
    printf("Received request:\n%s\n", buffer);
    
    // 简单的 HTTP 响应
    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<html><body><h1>Hello, World!</h1></body></html>";

    write(client_sock, response, strlen(response));
    close(client_sock);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // 创建 socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 设置地址和端口
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 绑定 socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // 监听连接
    listen(server_sock, 5);
    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        // 接受连接
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        printf("receive a sock\n");
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        // 处理客户端请求
        handle_client(client_sock);
        printf("hadle finished\n");
    }

    // 关闭服务器 socket
    close(server_sock);
    return 0;
}
