#include "types.h"
#include "stat.h"
#include "user.h"

#define MSGSIZE 8

int main(void)
{
	toggle(); 
	printf(1,"%s\n","IPC Test case");
	// char *msg = (char *)malloc(20);
	//int myid;
	//int from;	
	
int cid = fork();
	if(cid==0){
		// This is child
		char *msg = (char *)malloc(MSGSIZE);
		int stat=-1;
		while(stat==-1){
			stat = recv(msg);
		}
		printf(1,"2 CHILD: msg recv is: %s \n", msg );
		exit();
	}else{
		// This is parent
		char *msg_child = (char *)malloc(MSGSIZE);
		msg_child = "P";
		printf(1,"1 PARENT: msg sent is: %s \n", msg_child );
		send(getpid(),cid,msg_child);	
		
		
		free(msg_child);
	}
	wait();
	
	exit();
	
	/*
	 * uncomment this code to check for multicast
	 * some children remain as zombies, don't know how to deal with that
	 *
	 	
	int childs[3];
	int parentid = getpid();
	int cid = fork();
	
	int cid1=0, cid2=0;
	int i=0;
	printf(1, "%d\n", parentid);
	if(cid != 0){
		cid1 = fork();
		childs[i++]=cid;
	}	
	if(cid != 0 && cid1 != 0){
		cid2 = fork();
		childs[i++]=cid1;
	}
		
	if(cid2 !=0 && cid1 != 0 && cid !=0){
		childs[i++]=cid2;
	}
		
	
	if(cid != 0 && cid1 != 0 && cid2 != 0){
		for(int i=0; i<3; i++)
			printf(1, "%d ", childs[i]);
		char *msg_child = (char *)malloc(MSGSIZE);
		msg_child = "P";
		printf(1,"1 PARENT: msg sent to all childs is is: %s \n", msg_child);
		send_multi(parentid, (void *)childs ,msg_child);
		wait();
		exit();
	}
	else{
		sleep(3);	
		char *msg = (char *)malloc(MSGSIZE);
		recv(msg);
		//int stat=-1;
		//while(stat==-1){
			//stat = recv(msg);
		//}
		printf(1,"%d CHILD: msg recv is: %s \n", getpid(), msg);
		exit();
	}
	wait();
	exit();
	*/
}
