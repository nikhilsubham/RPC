#include "sll/sll1.h"
#include "DeSerialization/sentinel.h"
#include "DeSerialization/serialize.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "rpc_common.h"

/*Application specific data structures*/
typedef struct person_{
    char name[32];
    int age;
    int weight;
} person_t;

int a;
char buffer[128];

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


struct Node* find_person_db(dll_t* person_db, char*name) 
{
    if(!person_db || !person_db->head) return;
    
    struct Node *node = person_db->head;
    person_t *data = NULL;
   
     while (node != NULL) { 
        data = node->data;
        if(!strncmp(data->name, name,sizeof(name)))
            return node;
        node = node->next; 
    } 
    return NULL;
}


void list_server_stub_marshal(dll_t * obj, ser_buff_t *b)
{
  SENTINEL_INSERTION_CODE(obj,b);
  serialize_struct_Node(obj->head, b);
}

void serialize_struct_Node(struct Node*obj, ser_buff_t *b)
{
   SENTINEL_INSERTION_CODE(obj,b);
   serialize_void(obj->data,b);
   serialize_struct_Node(obj->next,b);
}


//dll_t* de_serialize_dll_t(ser_buff_t *b)
//{
//  SENTINEL_DETECTION_CODE(b);
//  dll_t* obj = calloc(1, sizeof(dll_t));
//  obj->head = de_serialize_struct_Node(b);
//  return obj;
//}

//struct Node* de_serialize_struct_Node(ser_buff_t *b)
//{
//  SENTINEL_DETECTION_CODE(b);
//  struct Node*obj = calloc(1,sizeof(struct Node));
//  obj->data = de_serialize_void(b);
//  obj->next = de_serialize_struct_Node(b);
//  return obj;
//}


void serialize_void(void* obj1, ser_buff_t *b)
{
   int loop_var = 0;
    unsigned int sentinel = 0xFFFFFFFF;
    person_t *obj = (person_t*)obj1;
    
    SENTINEL_INSERTION_CODE(obj,b);
    serialize_data(b, (char *)obj->name, 32);
    serialize_data(b, (char *)&obj->age, sizeof(int));
    serialize_data(b, (char *)&obj->weight, sizeof(int));
}


//person_t* de_serialize_void(ser_buff_t *b)
//{
//   int loop_var = 0;
//    unsigned int sentinel = 0xFFFFFFFF;
//    SENTINEL_DETECTION_CODE(b);

//    person_t *obj =  calloc(1, sizeof(person_t)); 
//    de_serialize_data((char *)obj->name, b, 32);
//    de_serialize_data((char *)&obj->age, b, sizeof(int));
//    de_serialize_data((char *)&obj->weight, b, sizeof(int));
//    return obj;
//}

void list_server_stub_unmarshal(ser_buff_t *server_recv_ser_buffer){

    de_serialize_data((char *)&a, server_recv_ser_buffer, sizeof(int)); 
   
    if(a == 2)
       de_serialize_data((char *)buffer, server_recv_ser_buffer, 128);
}



void rpc_server_process_msg(ser_buff_t *server_recv_ser_buffer, 
                       ser_buff_t *server_send_ser_buffer, dll_t *person_db){

   list_server_stub_unmarshal(server_recv_ser_buffer); 
   
   if(a==1)
     list_server_stub_marshal(person_db, server_send_ser_buffer);
   
   if(a==2)
   {
         struct Node* new_node = find_person_db(person_db, buffer); 
         serialize_void(new_node->data, server_send_ser_buffer);
   }    
}



int main(int argc, char **argv){
    
    int sock_udp_fd = 0, len, addr_len, opt = 1,
        reply_msg_size = 0;
         
	struct sockaddr_in server_addr,
                       client_addr;

    if ((sock_udp_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP )) == -1)
    {
        printf("socket creation failed\n");
        exit(1);
    }

     server_addr.sin_family = AF_INET;
     server_addr.sin_port = htons(SERVER_PORT);
     server_addr.sin_addr.s_addr = INADDR_ANY;
     addr_len = sizeof(struct sockaddr);

	if (setsockopt(sock_udp_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))<0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(sock_udp_fd, SOL_SOCKET, SO_REUSEPORT, (char *)&opt, sizeof(opt))<0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

    if (bind(sock_udp_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        printf("socket bind failed\n");
        exit(1);
    }


    person_t *person1 = calloc(1, sizeof(person_t));
    strncpy(person1->name, "Abhishek", strlen("Abhishek"));
    person1->age = 31;
    person1->weight = 75;

    person_t *person2 = calloc(1, sizeof(person_t));
    strncpy(person2->name, "Joseph", strlen("Joseph"));
    person2->age = 41;
    person2->weight = 70;

    person_t *person3 = calloc(1, sizeof(person_t));
    strncpy(person3->name, "Jack", strlen("Jack"));
    person3->age = 29;
    person3->weight = 55;

    person_t *person4 = calloc(1, sizeof(person_t));
    strncpy(person4->name, "Shubham", strlen("Shubham"));
    person4->age = 25;
    person4->weight = 70;

  
    dll_t *person_db = get_new_dll();

    At_front(&person_db->head, person1);
    At_front(&person_db->head, person2);
    At_front(&person_db->head, person3);
    At_front(&person_db->head, person4);

    printf("printing the object to be serialized on sending machine before serialiation\n\n");
    print_person_db(person_db);

    ser_buff_t *server_recv_ser_buffer = NULL,
               *server_send_ser_buffer = NULL;

    init_serialized_buffer_of_defined_size(&server_recv_ser_buffer, 
                                MAX_RECV_SEND_BUFF_SIZE);

    init_serialized_buffer_of_defined_size(&server_send_ser_buffer, 
                                MAX_RECV_SEND_BUFF_SIZE);
    
    printf("Server ready to service rpc calls\n");

READ:
    reset_serialize_buffer(server_recv_ser_buffer);

    len = recvfrom(sock_udp_fd, server_recv_ser_buffer->b, 
                   get_serialize_buffer_length(server_recv_ser_buffer),
                   0,(struct sockaddr *)&client_addr, &addr_len);

    printf("No of bytes recvd from client = %d\n", len); 

    reset_serialize_buffer(server_send_ser_buffer); 

    rpc_server_process_msg(server_recv_ser_buffer, server_send_ser_buffer, person_db); 

    len = sendto(sock_udp_fd, server_send_ser_buffer->b, 
                 get_serialize_buffer_data_size(server_send_ser_buffer),
                 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
    
    printf("rpc server replied with %d bytes msg\n", len);
    goto READ;
    return 0; 

}

   



    //ser_buff_t *b;
    //init_serialized_buffer(&b);

    /*Serialize the person_t object. It will recirsively serialize all internal sub-structures*/
    //serialize_person_t (&p1, b);
    ///serialize_dll_t(person_db, b);
    /*Now assume that we have sent the serialized buffer b to recieving machine*/


    /* Recieving machine has recieved the serialized buffer, Lets derialize it and reconstruct the object from it*/
    //reset_serialize_buffer(b);
    //dll_t *person_db1 = de_serialize_dll_t(b);
    //printf("after serialiation\n\n");
    //print_person_db(person_db1);
    
    //return 0;






