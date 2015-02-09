#ifndef RPC_DATA_H
#define RPC_DATA_H

#define RPC_TY_VOID 0x00
#define RPC_TY_INT  0x01
#define RPC_TY_STR  0x02

#define RPC_RET_OK              0x00
#define RPC_RET_UNKNOWN_FUNC    0x01
#define RPC_RET_WRONG_ARG       0x02
#define RPC_RET_NO_ANSWER       0x03

/**
 * @brief Represent a data argument.
 * Data is the data of the argument.
 */
struct rpc_arg {

    /**
     * @brief The argument type.
     * It could be RPC_TY_VOID, RPC_TY_INT or RPC_TY_STR.
     */
    unsigned int typ;

    /**
     * @brief The argument's data.
     */
    void *data;

};

#endif /* RPC_DATA_H */
