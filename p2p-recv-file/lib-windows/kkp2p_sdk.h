#ifndef _KKP2P_SDK_H_
#define _KKP2P_SDK_H_

#include <stdint.h>

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
#define KKP2P_ERROR_CHANNEL_IS_NULL        -113

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

    // when log_path is null, log path is program running path
    char* log_path;

    // maximum size(Byte) of log file
    int max_log_size;
} kkp2p_engine_conf_t;

typedef struct kkp2p_connect_ctx_s {
    char peer_id[33];    // peer_id with a maximum length of 32 and ended with '\0'
    int connect_mode;    // 0:auto; 1:only p2p; 2:only relay
    int encrypt_data;    // encrypt data or not. 0:not encrypt data; 1:encrypt
    int timeout;        // milliseconds, timeout for channel creation
    kkp2p_connect_cb func;    // connect async or not. NULL means synchronous, otherwise asynchronous
    void* func_param;         // the param for func when func is not NULL
} kkp2p_connect_ctx_t;

typedef struct kkp2p_channel_s {
    char peer_id[33];         // peer_id with a maximum length of 32 and ended with '\0'
    int  transmit_mode;       // 1:p2p; 2:relay
    int  encrypt_data;        // encrypt data or not. 0:not encrypt data; 1:encrypt
    uint32_t channel_id;         // the channel id
    int fd;                   // the channel's fd for app read and write data 
} kkp2p_channel_t;

#ifdef __cplusplus
	extern "C" {
#endif

// Parameter: 
//   timeout: second when login_domain is domain needed to be analysised
// Return:
//   valid engine pointer when success and NULL when error

kkp2p_engine_t*  kkp2p_engine_init(const kkp2p_engine_conf_t* conf, int timeout);

void kkp2p_engine_destroy(kkp2p_engine_t* engine);

// 0:no log, 1:error log, 2:normal log, 3:info log, 4:debug log
// default log level is 2, log name is kkp2p_engine.log
void kkp2p_switch_log_level(kkp2p_engine_t* engine, int level);

// Parameter: 
//  szHosts:  char szHosts[ipCount][ipLen], get the domain's ip
// Return:
//  actually returned ip count
int kkp2p_get_domainip(kkp2p_engine_t* engine, char* szHosts, int ipLen, int ipCount);

// return value: 0 is success, <0 is error
int kkp2p_join_net(kkp2p_engine_t* engine, char* peerId, char* secret);

// return value: 0 is success, <0 is error
int kkp2p_join_lan(kkp2p_engine_t* engine, char* peerId);

int kkp2p_listen_fd(kkp2p_engine_t* engine);

// return value: >0 is success, 0 is timeout, <0 is error
int kkp2p_accept(kkp2p_engine_t* engine, int timeout, kkp2p_channel_t* channel);

// when ctx->func is not null, it's asynchronous connect and channel can be NULL
// when ctx->func is null, it's synchronous  connect and channel can't be NULL
// return value: <0 is error, >=0 is success
int kkp2p_connect(kkp2p_engine_t* engine, kkp2p_connect_ctx_t* ctx, kkp2p_channel_t* channel);
int kkp2p_lan_search(kkp2p_engine_t* engine, kkp2p_connect_ctx_t* ctx, kkp2p_channel_t* channel);

// return value: <0 is error, 0 is timeout, >0 is success read byte count
int kkp2p_read(int fd, char* buff, int len, int timeout);

// return value: <0 is error, 0 is timeout, >0 is success write byte count
int kkp2p_write(int fd, char* buff,int len, int timeout);

void kkp2p_close_channel(kkp2p_engine_t* engine, uint32_t channel);

void kkp2p_close_fd(int fd);

#ifdef __cplusplus
}
#endif

#endif


