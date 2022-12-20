#ifndef _KKP2P_SDK_H_
#define _KKP2P_SDK_H_

#include <stdint.h>

#ifdef KKP2P_WINDLL   
    #ifdef KKP2P_EXPORTS
        #define KKP2P_API __declspec(dllexport)
    #else
        #define KKP2P_API __declspec(dllimport)
    #endif
#else
    #define KKP2P_API
#endif
    

// error code
#define KKP2P_SUCCESS                             0
#define KKP2P_ERROR                              -1
#define KKP2P_ERROR_DOMAIN_PARAM_ERROR         -101
#define KKP2P_ERROR_ANALYSIS_DOMAIN_TIMEOUT    -102
#define KKP2P_ERROR_ANALYSIS_DOMAIN_FAILURE    -103
#define KKP2P_ERROR_DOMAIN_NO_IP               -104
#define KKP2P_ERROR_SYSTEM_INTERNAL        -105
#define KKP2P_ERROR_PARAMETER              -106
#define KKP2P_ERROR_CONN_DISCONNECT        -107
#define KKP2P_ERROR_OTHER_DISCONNECT       -108
#define KKP2P_ERROR_CONNECT_TIMEOUT        -109
#define KKP2P_ERROR_INVALID_PEER_ID        -110
#define KKP2P_ERROR_INVALID_PEER_KEY       -111
#define KKP2P_ERROR_LANMODE_MUSTUSE_P2P    -112
#define KKP2P_ERROR_BIND_IP                -113

typedef struct kkp2p_engine_s kkp2p_engine_t;
typedef struct kkp2p_channel_s kkp2p_channel_t;
typedef void(*kkp2p_connect_cb)(int result, kkp2p_channel_t* channel, void* param);

typedef struct kkp2p_engine_conf_s {
    // NULL means only LAN search mode
    // maybe domain, such as login.p2p.com
    // maybe ip, such as 125.70.218.16
    // maybe iplist, such as 125.70.218.16|125.70.218.17|125.70.218.18
	char* login_domain;
    
	uint16_t login_port;

    // 0 means not use LAN serach mode
    // used to connect or listen
    uint16_t  lan_search_port;

    // 1:disable upnp,0 enable upnp
    char disable_upnp;

    // when log_path is null, log path is program running path
    char* log_path;

    // maximum size(Byte) of log file
    int max_log_size;
} kkp2p_engine_conf_t;

typedef struct kkp2p_connect_ctx_s {
    char peer_id[33];    // peer_id with a maximum length of 32 and ended with '\0'
    int connect_mode;    // 0:auto; 1:only p2p; 2:only relay
    int encrypt_data;    // encrypt data or not. 0:not encrypt data; 1:encrypt
    int timeout;         // milliseconds, timeout for channel creation
    int connect_desc;    // user defined parameters, transmitted to the dest peer
    kkp2p_connect_cb func;    // connect async or not. NULL means synchronous, otherwise asynchronous
    void* func_param;         // the param for func when func is not NULL
} kkp2p_connect_ctx_t;

typedef struct kkp2p_channel_s {
    char peer_id[33];         // peer_id with a maximum length of 32 and ended with '\0'
    int  transmit_mode;       // 1:p2p; 2:relay
    int  encrypt_data;        // encrypt data or not. 0:not encrypt data; 1:encrypt
    uint32_t channel_id;      // the channel id
    int connect_desc;         // user defined parameters
    int fd;                   // the channel's fd for app read and write data 
} kkp2p_channel_t;

#ifdef __cplusplus
	extern "C" {
#endif

// get sdk version
// Parameter: 
//   version: version buffer
// Return:
//   sdk version

KKP2P_API void kkp2p_sdk_version(char* version, int buffLen);

// Parameter: 
//   timeout: second when login_domain is domain needed to be analysised
// Return:
//   valid engine pointer when success and NULL when error

KKP2P_API kkp2p_engine_t*  kkp2p_engine_init(const kkp2p_engine_conf_t* conf, int timeout);

KKP2P_API void kkp2p_engine_destroy(kkp2p_engine_t* engine);

// 0:no log, 1:error log, 2:normal log, 3:info log, 4:debug log
// default log level is 2, log name is kkp2p_engine.log
KKP2P_API void kkp2p_switch_log_level(kkp2p_engine_t* engine, int level);

// Parameter: 
//  szHosts:  char szHosts[ipCount][ipLen], get the domain's ip
// Return:
//  actually returned ip count
KKP2P_API int kkp2p_get_domainip(kkp2p_engine_t* engine, char* szHosts, int ipLen, int ipCount);

// return value: 0 is success, <0 is error
KKP2P_API int kkp2p_join_net(kkp2p_engine_t* engine, char* peerId, char* secret);

// return value: 0 is success, <0 is error
KKP2P_API int kkp2p_join_lan(kkp2p_engine_t* engine, char* peerId);

KKP2P_API int kkp2p_listen_fd(kkp2p_engine_t* engine);

// return value: >0 is success, 0 is timeout, <0 is error
KKP2P_API int kkp2p_accept(kkp2p_engine_t* engine, int timeout, kkp2p_channel_t* channel);

// when ctx->func is not null, it's asynchronous connect and channel can be NULL
// when ctx->func is null, it's synchronous  connect and channel can't be NULL
// return value: <0 is error, >=0 is success
KKP2P_API int kkp2p_connect(kkp2p_engine_t* engine, kkp2p_connect_ctx_t* ctx, kkp2p_channel_t* channel);
KKP2P_API int kkp2p_lan_search(kkp2p_engine_t* engine, kkp2p_connect_ctx_t* ctx, kkp2p_channel_t* channel);

// start a proxy to communicate with peer
// The data sent to the IP and port will be forwarded to ctx.peer_id
// ctx.func and ctx.func_param must be NULL
// return value:<0 is error, >=0 is success, proxyId is an output parameter
KKP2P_API int kkp2p_start_proxy(kkp2p_engine_t* engine, char* ip, uint16_t port, kkp2p_connect_ctx_t* ctx, uint32_t* proxyId);
KKP2P_API void kkp2p_stop_proxy(kkp2p_engine_t* engine, uint32_t proxyId);

// return value: <0 is error, 0 is timeout, >0 is success read byte count
KKP2P_API int kkp2p_read(int fd, char* buff, int len, int timeout);

// return value: <0 is error, 0 is timeout, >0 is success write byte count
KKP2P_API int kkp2p_write(int fd, char* buff,int len, int timeout);

KKP2P_API void kkp2p_close_channel(kkp2p_engine_t* engine, uint32_t channel);

KKP2P_API void kkp2p_close_fd(int fd);

#ifdef __cplusplus
}
#endif

#endif


