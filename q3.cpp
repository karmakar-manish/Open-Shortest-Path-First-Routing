#include<bits/stdc++.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

const int BUFFER_SIZE = 1024;

// Function to extract hostname from URL
string extractHostname(const string& url) {
    size_t pos1 = url.find("://");
    if (pos1 != string::npos) {
        size_t pos2 = url.find("/", pos1 + 3);
        if (pos2 != string::npos) {
            return url.substr(pos1 + 3, pos2 - pos1 - 3);
        }
        return url.substr(pos1 + 3);
    }
    return url;
}


struct dll{
    string data;
    string url;
    struct dll *next;
    struct dll *prev;
};


int cacheSize = 0;
int maxSize = 3;

void deleteLast(dll** head){
    dll* temp = *head;
    while(temp->next){
        temp = temp->next;
    }
    temp->prev->next = NULL;
    temp->prev = NULL;
    free(temp);
    --cacheSize;
}

void insertFront(dll **head, string data, string url){
    dll *newNode = new dll;
    newNode->data = data;
    newNode->url = url;
    newNode->next = *head;
    newNode->prev = NULL;
    (*head)->prev = newNode;
    *head = newNode;
    ++cacheSize;
}

dll* search(dll** head, string url){
    dll* temp = *head;
    while(temp && temp->url != url){
        temp = temp->next;
    }
    return temp;
}

void removeAndInsertFront(dll** head, string data, string url){
    
    if(*head == NULL){
        dll* node = new dll;
        node->data = data;
        node->url = url;
        node->next = NULL;
        node->prev = NULL;
        *head = node;
        ++cacheSize;
        return;
    }
    dll * temp = search(head, url);
    if(!temp){
        if(cacheSize < maxSize){
            insertFront(head, data, url);
        }else{
            deleteLast(head);
            insertFront(head, data, url);
        }
    } else{

        
        // agar front pe hi hai to kuch mat kro.
        if(!temp->prev){
            return;
        }

        // agar last me hai to remove krke aage lao
        else if(!temp->next){
            temp->prev->next = NULL;
            temp->prev = NULL;
            --cacheSize;
        }

        // agar kahi or hai, to usko remove krke aage rakh do
        else{
            temp->next->prev = temp->prev;
            temp->prev->next = temp->next;
            temp->next = temp->prev = NULL;
            --cacheSize;
            
        }

        free(temp);
        insertFront(head, data, url);
    }
    
}


string getData(string url){
        // Extract hostname from URL
        string hostname = extractHostname(url);

        // Create a TCP socket
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            cerr << "Error creating socket" << endl;
            return "";
        }

        // Get the IP address of the server
        struct addrinfo hints, *res;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(hostname.c_str(), "https", &hints, &res) != 0) {
            cerr << "Error getting address info" << endl;
            return "";
        }

        // Connect to the server
        if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
            cerr << "Error connecting to server" << endl;
            return "";
        }

        // Initialize OpenSSL SSL context
        SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());
        if (!ctx) {
            cerr << "Error creating SSL context" << endl;
            return "";
        }

        // Create SSL connection
        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, sockfd);

        // Perform SSL handshake
        if (SSL_connect(ssl) != 1) {
            cerr << "Error SSL handshake" << endl;
            return "";
        }

        // Send HTTP GET request over SSL
        string httpRequest = "GET / HTTP/1.1\r\n";
        httpRequest += "Host: " + hostname + "\r\n";
        httpRequest += "Connection: close\r\n\r\n";

        if (SSL_write(ssl, httpRequest.c_str(), httpRequest.length()) <= 0) {
            cerr << "Error sending HTTP request" << endl;
            return "";
        }

        // Receive and print content of the response
        char buffer[BUFFER_SIZE];
        stringstream response;
        int bytesReceived;
        while ((bytesReceived = SSL_read(ssl, buffer, BUFFER_SIZE - 1)) > 0) {
            buffer[bytesReceived] = '\0';
            response << buffer;
        }

        string ret = response.str();

        // Close SSL connection and free resources
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);

        // Close the socket
        close(sockfd);
        freeaddrinfo(res);


        return ret;
}


int main(){

    cout<<"Enter the capacity of the Cache: ";
    cin>>maxSize;
    dll* head = NULL;

    // Initialize OpenSSL
    SSL_load_error_strings();
    SSL_library_init();

    // Get the URL from the user
    while(true){
        string url = "";
        cout << "Enter the URL of the website: ";
        cin >> url;
        if(url == "exit") break;
        dll *temp = search(&head, url);

        
        string resp;
        if(temp) resp = temp->data;
        else resp = getData(url);
        if(resp.size() == 0) continue;

        cout<<resp<<" "<<url<<endl;

        removeAndInsertFront(&head, resp, url);



        cout<<"------------------------------------------------PAGE CONTENT------------------------------------------------"<<endl;

        // cout<<resp<<endl;
        
        cout<<endl<<"---------------------------------------------CACHE CONTENT----------------------------------------------"<<endl;

        dll* cur = head;
        int id = 1;
        while(cur){
            cout<<id++<<": "<<cur->url<<" "<<endl;   
            cur = cur->next;
        }
    }
    // Clean up OpenSSL
    ERR_free_strings();
    EVP_cleanup();




    return 0;
}