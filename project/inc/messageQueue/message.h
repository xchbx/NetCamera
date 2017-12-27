#ifndef MESSAGE_H
#define MESSAGE_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef long BaseType_t;

    /**
     * - task and interupt message enum list.
     *  - Message enum struct, all message used by tasks and interupt must be defined here.
     */
    typedef enum
    {
        MSG_TYPE_NULL = 0,
        MSG_TEST_0,
        MSG_TEST_1,
        MSG_TEST_2,
        MSG_TEST_3,
        MSG_TEST_4,
        MSG_TEST_5,
        MSG_TEST_6,
        MSG_TYPE_END /** max message count */
    } MESSAGE_E;


    typedef enum
    {
        QUEUE_TYPE_NULL = 0,
        QUEUE_TYPE_WAKE0,
        QUEUE_TYPE_WAKE1,
        QUEUE_TYPE_WAKE2,
        QUEUE_TYPE_WAKE3,
        QUEUE_TYPE_WAKE4,
        QUEUE_TYPE_WAKE5,
        QUEUE_TYPE_WAKE6,
        QUEUE_TYPE_WAKE7,
        QUEUE_TYPE_WAKE8,
        QUEUE_TYPE_END /** max message count */
    } QUEUE_E;

    /**
     * parameter of message struct
     * - 1. *pointer: point a message parameter which is greater than 4 bytes.
     * - 2. value: if your message parameter less than 4 bytes, assign the parameter to the value directly.
     *
     * @brief
     *      if the parameter is greater than 4 bytes, you should define a struct.  About the struct,
     *      please refer to software architecture files.
     */
    typedef union
    {
        void * pointer; /** transmit pointer */
        uint32_t value; /** transmit value   */

    } PARAMETER_U;

    /**
     * - message_id : identification of message, one of the enum struct: MESSAGE_E
     * - PARAMETER_U: parameter union struct, which can be a uint32_t value, or a pointer that point
     *                a parameter value struct.
     */
    typedef struct
    {
        BaseType_t msg_id;
        uint32_t msg_type;	/*31~16 bit: value or pointer; 15~0bit: msg data length*/
        PARAMETER_U parameter;

    } MSG_T;

#ifdef __cplusplus
}
#endif

#endif
