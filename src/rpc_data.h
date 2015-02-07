#ifndef RPC_DATA_H
#define RPC_DATA_H

#define RPC_TY_VOID 0x00
#define RPC_TY_INT  0x01
#define RPC_TY_STR  0x02

#define RPC_RET_OK              0x00
#define RPC_RET_UNKNOWN_FUNC    0x01
#define RPC_RET_WRONG_ARG       0x02
#define RPC_RET_NO_ANSWER       0x03

struct rpc_arg {
    unsigned int typ;
    void *data;
};

#endif /* RPC_DATA_H */
