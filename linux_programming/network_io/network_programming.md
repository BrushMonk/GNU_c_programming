`struct sockaddr_in` is included in <netinet/in.h>. Here are details in `struct sockaddr_in`:
```cpp
struct sockaddr_in {
    sa_family_t    sin_family; // address family: AF_INET
    in_port_t      sin_port;   // port in network byte order
    struct in_addr sin_addr;   // host address
};

struct in_addr {
    uint32_t       s_addr;     // host address in network byte order
};
```

`struct hostent` is included in <netdb.h>. Here are details in `struct hostent`:
```cpp
struct hostent
{   char *h_name;
    char **h_aliases;
    short h_addrtype;
    short h_length;
    // historical reasons make h_addr_list char string array
    char **h_addr_list;
    // historical reasons make h_addr char string
    #define h_addr h_addr_list[0];
};
```