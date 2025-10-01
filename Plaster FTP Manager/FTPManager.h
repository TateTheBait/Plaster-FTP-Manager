#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <wx/string.h>

#pragma comment(lib, "Ws2_32.lib")  // link Winsock

class FTPManager {
public:
    FTPManager(const wxString& host, int port, const wxString& username, const wxString& password)
        : session(nullptr), sftp_session(nullptr), sftp_handle(nullptr),
        sock(INVALID_SOCKET), connected(false)
    {
        lastError.clear();

        // Initialize Winsock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            lastError = "WSAStartup failed";
            return;
        }

        // Initialize libssh2
        if (libssh2_init(0) != 0) {
            lastError = "libssh2 initialization failed";
            return;
        }

        // Create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) {
            lastError = "Failed to create socket";
            return;
        }

        // Resolve hostname
        sockaddr_in sin{};
        sin.sin_family = AF_INET;
        sin.sin_port = htons(port);
        if (inet_pton(AF_INET, host.utf8_str(), &sin.sin_addr) != 1) {
            // Try DNS resolution
            struct addrinfo* result = nullptr;
            if (getaddrinfo(std::string(host.mb_str()).c_str(), nullptr, nullptr, &result) == 0 && result) {
                sin.sin_addr = ((sockaddr_in*)result->ai_addr)->sin_addr;
                freeaddrinfo(result);
            }
            else {
                lastError = "Invalid host or DNS resolution failed";
                return;
            }
        }

        // Connect
        if (connect(sock, (struct sockaddr*)&sin, sizeof(sin)) != 0) {
            lastError = "Failed to connect to host";
            return;
        }

        // Create libssh2 session
        session = libssh2_session_init();
        if (!session) {
            lastError = "Failed to create libssh2 session";
            return;
        }
        libssh2_session_set_blocking(session, 1);

        // Perform handshake
        rc = libssh2_session_handshake(session, sock);
        if (rc) {
            lastError = "SSH handshake failed";
            return;
        }

        // Authenticate
        wxCharBuffer userBuf = username.utf8_str();
        wxCharBuffer passBuf = password.utf8_str();

        const char* authlist = libssh2_userauth_list(session, userBuf.data(), (unsigned int)strlen(userBuf.data()));
        if (!authlist) {
            lastError = "Failed to get authentication list";
            return;
        }

        if (strstr(authlist, "password")) {
            if (libssh2_userauth_password(session, userBuf.data(), passBuf.data()) != 0) {
                lastError = "Password authentication failed";
                return;
            }
        }

        // Initialize SFTP session
        sftp_session = libssh2_sftp_init(session);
        if (!sftp_session) {
            lastError = "Failed to initialize SFTP session";
            return;
        }

        connected = true;  // success
        lastError = "Succesfully Connected to SFTP Session!";
    }

    ~FTPManager() {
        if (sftp_session) libssh2_sftp_shutdown(sftp_session);
        if (session) libssh2_session_disconnect(session, "Bye");
        if (session) libssh2_session_free(session);
        if (sock != INVALID_SOCKET) closesocket(sock);
        libssh2_exit();
        WSACleanup();
    }

    bool isConnected() const { return connected; }
    const wxString& getLastError() const { return lastError; }

private:
    LIBSSH2_SESSION* session;
    LIBSSH2_SFTP* sftp_session;
    LIBSSH2_SFTP_HANDLE* sftp_handle;
    SOCKET sock;
    int rc;
    bool connected;
    wxString lastError;
};
