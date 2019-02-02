#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "rpc_common.h"
#include "DeSerialization/serialize.h"
#include "DeSerialization/sentinel.h"
#include "sll/sll1.h"


typedef struct person_{

    char name[32];
    int age;
    int weight;
} person_t;


void serialize_struct_Node(struct Node*obj, ser_buff_t *b);
void serialize_dll_t(dll_t * obj, ser_buff_t *b);

struct Node* de_serialize_struct_Node(ser_buff_t *b);
dll_t* de_serialize_dll_t(ser_buff_t *b);

void serialize_void(void* obj, ser_buff_t *b);
person_t* de_serialize_void(ser_buff_t *b);


static void
print_person_details(person_t *person){

    printf("Name = %s\n",   person->name);
    printf("Age = %d\n",    person->age);
    printf("weight = %d\n", person->weight);
    printf("\n");
}


static void print_person_db(dll_t* person_db) 
{
    if(!person_db || !person_db->head) return;
    
    struct Node *node = person_db->head;
    person_t *data = NULL;
  
     while (node != NULL) { 
        data = node->data;
        print_person_details(data);
        node = node->next; 
    } 
}



//void serialize_dll_t(dll_t * obj, ser_buff_t *b)
//{
//  SENTINEL_INSERTION_CODE(obj,b);
//  serialize_struct_Node(obj->head, b);
//}

//void serialize_struct_Node(struct Node*obj, ser_buff_t *b)
//{
//   SENTINEL_INSERTION_CODE(obj,b);
//   serialize_void(obj->data,b);
//   serialize_struct_Node(obj->next,b);
//}


dll_t* list_client_stub_unmarshal(ser_buff_t *b)
{
  SENTINEL_DETECTION_CODE(b);
  dll_t* obj = calloc(1, sizeof(dll_t));
  obj->head = de_serialize_struct_Node(b);
  return obj;
}

struct Node* de_serialize_struct_Node(ser_buff_t *b)
{
  SENTINEL_DETECTION_CODE(b);
  struct Node*obj = calloc(1,sizeof(struct Node));
  obj->data = de_serialize_void(b);
  obj->next = de_serialize_struct_Node(b);
  return obj;
}


//void serialize_void(void* obj1, ser_buff_t *b)
//{
//   int loop_var = 0;
//    unsigned int sentinel = 0xFFFFFFFF;
//    person_t *obj = (person_t*)obj1;
    
//    SENTINEL_INSERTION_CODE(obj,b);
//    serialize_data(b, (char *)obj->name, 32);
//    serialize_data(b, (char *)&obj->age, sizeof(int));
//    serialize_data(b, (char *)&obj->weight, sizeof(int));
//}

person_t* de_serialize_void(ser_buff_t *b)
{
   int loop_var = 0;
    unsigned int sentinel = 0xFFFFFFFF;
    SENTINEL_DETECTION_CODE(b);

    person_t *obj =  calloc(1, sizeof(person_t)); 
    de_serialize_data((char *)obj->name, b, 32);
    de_serialize_data((char *)&obj->age, b, sizeof(int));
    de_serialize_data((char *)&obj->weight, b, sizeof(int));
    return obj;
}


void rpc_send_recv (ser_buff_t *client_send_ser_buffer, 
               ser_buff_t *client_recv_ser_buffer){

    struct sockaddr_in dest;
    int sockfd = 0, rc = 0, recv_size = 0;
    int addr_len;

    dest.sin_family = AF_INET;
    dest.sin_port = htons(SERVER_PORT);
    struct hostent *host = (struct hostent *)gethostbyname(SERVER_IP);
    dest.sin_addr = *((struct in_addr *)host->h_addr);
    addr_len = sizeof(struct sockaddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockfd == -1){
        printf("socket creation failed\n");
        return;
    }

    rc = sendto(sockfd, client_send_ser_buffer->b, 
                get_serialize_buffer_data_size(client_send_ser_buffer), 
                0, (struct sockaddr *)&dest, sizeof(struct sockaddr));

    printf("%s() : %d bytes sent\n", __FUNCTION__, rc);

    recv_size = recvfrom(sockfd, client_recv_ser_buffer->b, 
                get_serialize_buffer_length(client_recv_ser_buffer), 0,
                (struct sockaddr *)&dest, &addr_len);

    printf("%s() : %d bytes recieved\n", __FUNCTION__, recv_size);
}




ser_buff_t* list_client_stub_marshal(int a, char* buffer){

    ser_buff_t *client_send_ser_buffer = NULL;
    init_serialized_buffer_of_defined_size(&client_send_ser_buffer, MAX_RECV_SEND_BUFF_SIZE);

    serialize_data(client_send_ser_buffer, (char *)&a, sizeof(int));
    if(a==2)
    {
      serialize_data(client_send_ser_buffer, (char *)buffer, 128);
    }
    return client_send_ser_buffer;
}



void 
init_rpc_infra(){
}


void list_rpc(int a, char* buffer){

    init_rpc_infra();

    ser_buff_t *client_send_ser_buffer = list_client_stub_marshal(a, buffer);
    ser_buff_t *client_recv_ser_buffer = NULL;

    init_serialized_buffer_of_defined_size(&client_recv_ser_buffer, 
                        MAX_RECV_SEND_BUFF_SIZE);

    rpc_send_recv(client_send_ser_buffer, client_recv_ser_buffer);

    free_serialize_buffer(client_send_ser_buffer);
    client_send_ser_buffer = NULL;
    if(a==1)
        {
           dll_t* student_db = list_client_stub_unmarshal(client_recv_ser_buffer);
           print_person_db(student_db);
        }
    if(a==2)
       {
          person_t *person =   de_serialize_void(client_recv_ser_buffer);
          print_person_details(person);
       }
    free_serialize_buffer(client_recv_ser_buffer);
}


int
main(int argc, char **argv){

    int a;
    char buffer[128]="\0";
  
   while(1)
    {
      printf("Enter 1 for getting all the student information\n");
      printf("Enter 2 for getting studdent information by name\n");
      scanf("%d", &a);
    
     if(a==2)
      {
         printf("\nEnter the name of the student for his information\n");
         scanf("%s",buffer);
      }
    
     list_rpc(a, buffer);
   }   
    return 0; 
}
