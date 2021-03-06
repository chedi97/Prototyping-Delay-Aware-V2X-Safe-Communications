#include "control_client.h"

int init_client_frame(int frame_size) {
    u16_client_data_off = (uint16_t) ETH_HLEN;
    pu8a_client_frame   = (uint8_t *) calloc(1, frame_size);

    if (pu8a_client_frame == NULL) {
        perror("Could not get memory for the transmit frame\n");
        return ERROR_CREATE_FRAME;
    }

    pu8a_client_data    = pu8a_client_frame + u16_client_data_off;

    return NO_ERROR;
}

void set_client_frame_header(uint8_t* src_addr, uint8_t* dest_addr) {
    (void)memcpy(pu8a_client_frame, dest_addr, ETH_ALEN);
    (void)memcpy(pu8a_client_frame + ETH_ALEN, src_addr, ETH_ALEN);
}

void free_client_frame() {
    free(pu8a_client_frame);
    pu8a_client_frame = NULL;
    pu8a_client_data  = NULL;
}

void init_client_data(int data_size) {
    (void)memset(&pu8a_client_frame[u16_client_data_off], '\0', data_size);
}

void set_client_data(int data_size, char* string, int index, long timestamp) {
    printf("  Client sent a message %d\n", index);

    (void)snprintf((char *)&pu8a_client_frame[u16_client_data_off],
        data_size,"%s %d %ld", string, index, timestamp);
}

void init_client_socket() {
    s32_client_sock = -1;
}

int set_client_socket() {
    s32_client_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if (s32_client_sock == -1) {
        perror("Could not create the socket");

        return ERROR_CREATE_SOCKET;
    }

    printf("Send Socket created\n");
    return NO_ERROR;
}

void close_client_socket() {
    if (s32_client_sock > 0) {
        close(s32_client_sock);
    }
}

void init_client_sockaddr_ll() {
    (void)memset(&s_dest_addr, 0, sizeof(s_dest_addr));
}

void set_client_sockaddr_ll(int32_t nic_index, uint8_t* dest_addr) {
    s_dest_addr.sll_family = AF_PACKET;
    /*we don't use a protocol above ethernet layer, just use anything here*/
    s_dest_addr.sll_protocol = htons(ETH_P_ALL);
    s_dest_addr.sll_ifindex = nic_index;
    s_dest_addr.sll_hatype = ARPHRD_ETHER;
    s_dest_addr.sll_pkttype = PACKET_OTHERHOST; //PACKET_OUTGOING
    s_dest_addr.sll_halen = ETH_ALEN;
    /*MAC - begin*/
    s_dest_addr.sll_addr[0] = dest_addr[0];
    s_dest_addr.sll_addr[1] = dest_addr[1];
    s_dest_addr.sll_addr[2] = dest_addr[2];
    s_dest_addr.sll_addr[3] = dest_addr[3];
    s_dest_addr.sll_addr[4] = dest_addr[4];
    s_dest_addr.sll_addr[5] = dest_addr[5];
    /*MAC - end*/
    s_dest_addr.sll_addr[6] = 0x00; /*not used*/
    s_dest_addr.sll_addr[7] = 0x00; /*not used*/
}

int send_client_data(int frame_size) {
    int sendResult = sendto(s32_client_sock,
                         pu8a_client_frame,
                         frame_size,
                         0,
                         (struct sockaddr *)&s_dest_addr,
                         sizeof(s_dest_addr));

    if (sendResult == -1) {
        perror("Socket send failed");

        return ERROR_SEND_MESSAGE;
    }

    return NO_ERROR;
}