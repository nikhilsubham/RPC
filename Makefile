TARGET: server client
serialize.o:DeSerialization/serialize.c
	gcc  -c -I DeSerialization DeSerialization/serialize.c -o DeSerialization/serialize.o
sll.o:sll/sll1.c
	gcc  -c -I sll sll/sll1.c -o sll/sll.o
client.o:client.c
	gcc -c -g client.c -o client.o
server.o:server.c
	gcc -c -g server.c -o server.o 
client:client.o DeSerialization/serialize.o sll/sll.o 
	gcc -g -I DeSerialization -I sll  DeSerialization/serialize.o sll/sll.o client.o -o client 
server:server.o DeSerialization/serialize.o sll/sll.o
	gcc -g -I DeSerialization -I sll DeSerialization/serialize.o sll/sll.o server.o -o server 
clean:
	rm client
	rm server
	rm *.o
	rm sll/*.o
	rm Deserialization/*.o
