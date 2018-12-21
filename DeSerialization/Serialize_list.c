#include "dll1.h"
#include "sentinel.h"
#include "serialize.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>

/*Application specific data structures*/
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
    //printf("\nTraversal in forward direction \n");

    struct Node *node = person_db->head;
    person_t *data = NULL;
  
     while (node != NULL) { 
        data = node->data;
        print_person_details(data);
        //last = node; 
        node = node->next; 
    } 
}


void serialize_dll_t(dll_t * obj, ser_buff_t *b)
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


dll_t* de_serialize_dll_t(ser_buff_t *b)
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


int main(int argc, char **argv){

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

    /*Create a new doubly Linked List*/
 
    dll_t *person_db = get_new_dll();

    At_front(&person_db->head, person1);
    At_front(&person_db->head, person2);
    At_front(&person_db->head, person3);
    At_front(&person_db->head, person4);

    printf("printing the object to be serialized on sending machine before serialiation\n\n");
    print_person_db(person_db);

    ser_buff_t *b;
    init_serialized_buffer(&b);

    /*Serialize the person_t object. It will recirsively serialize all internal sub-structures*/
    //serialize_person_t (&p1, b);
    serialize_dll_t(person_db, b);
    /*Now assume that we have sent the serialized buffer b to recieving machine*/


    /* Recieving machine has recieved the serialized buffer, Lets derialize it and reconstruct the object from it*/
    reset_serialize_buffer(b);
    dll_t *person_db1 = de_serialize_dll_t(b);
    printf("after serialiation\n\n");
    print_person_db(person_db1);
    
    return 0;
}





