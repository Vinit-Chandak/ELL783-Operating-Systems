#include "types.h"
#include "stat.h"
#include "user.h"

#define MSGSIZE 8
float variance;


void print_variance(float xx)
{
 int beg=(int)(xx);
 int fin=(int)(xx*100)-beg*100;
 printf(1, "Variance of array for the file arr is %d.%d", beg, fin);
}


int main(int argc, char *argv[])
{
	if(argc< 2){
		printf(1,"Need type and input filename\n");
		exit();
	}
	char *filename;
	filename=argv[2];
	int type = atoi(argv[1]);
	printf(1,"Type is %d and filename is %s\n",type, filename);

	int tot_sum = 0;	

	int size=1000;
	short arr[size];
	char c;
	int fd = open(filename, 0);
	for(int i=0; i<size; i++){
		read(fd, &c, 1);
		arr[i]=c-'0';
		read(fd, &c, 1);
	}	
  	close(fd);
  	// this is to supress warning
  	printf(1,"first elem %d\n", arr[0]);
  
  	//----FILL THE CODE HERE for unicast sum
	int noOfProcesses = 8;
	int pidParent = getpid();
	int i, j, cid;
	//int curStart = 0, curEnd=125;
	int elmForEachProc = size/noOfProcesses;
	void *partialSumP = (void *)malloc(MSGSIZE);
	void *pmsg = (void *)malloc(MSGSIZE);
	int partialSum=0;
	
	if(type == 0){
		for(i=0; i<noOfProcesses; i++){
			cid = fork();
			if(cid == 0){
				int curStart = i*elmForEachProc;
		        int curEnd = (i+1)*elmForEachProc;
				partialSum=0;
				for(j=curStart; j<curEnd; j++){
					partialSum += (short)arr[j];
				}
				//printf(1, "partial sum = %d ", partialSum);
				//printf(1, "%d %d ", curStart, curEnd);
				//printf(1, "%d \n",getpid());
				pmsg = (void *)(&partialSum);
				send(getpid(), pidParent, (void *)pmsg);
				//updating local copy?
				//curStart+=elmForEachProc;
				//curEnd+=elmForEachProc;
				exit();
			}
			else{
				wait();
				//printf(1, "%d ",getpid());
				recv((void *)partialSumP);
				//printf(1, "%d \n", *(short *)(partialSumP));
				tot_sum+=*(short *)partialSumP;
			}
			
		}
		/*
		int lmao = fork();
		if(lmao == 0)
			printf(1, "child = %d ", getpid());
		else
			printf(1, "parent = %d", getpid());
		*/
	}
	else{
	
		int noOfProcesses = 8;
		int pidParent = getpid();
		int i; //j;//cid;
		//int curStart = 0, curEnd=125;
		//int elmForEachProc = size/noOfProcesses;
		void *pmsg = (void *)malloc(MSGSIZE);
		int partialSum=0;
		int indexArr[noOfProcesses+1];
		indexArr[0]=0;
		indexArr[noOfProcesses]=size;
		int children[noOfProcesses];
		
		for(int i=1 ;i<noOfProcesses; i++)
			indexArr[i]=indexArr[i-1]+(size/noOfProcesses);
		
		int cid1 = fork();
		if(cid1==0){
			partialSum=0;
			for(j=indexArr[0]; j<indexArr[1]; j++){
				partialSum += (short)arr[j];
			}
			pmsg = (void *)(&partialSum);
			send(getpid(), pidParent, (void *)pmsg);
			sleep(100);
			void *mean = (void *)malloc(MSGSIZE);
			recv((void *)mean);
			//printf(1, "%d ", *(int *)mean);
			float var=0;
			int k;
			for(k=indexArr[0]; k<indexArr[1]; k++){
				var += (arr[k]-*(float *)mean) * (arr[k]-*(float *)mean); 
			}
			sleep(100);
			//printf(1, "%f ", var);
			void *ret_var = (void *)malloc(MSGSIZE);
			ret_var = (void *)(&var);
			send(getpid(), pidParent, (void *)ret_var);
			exit();
		}
		else{
			children[0]=cid1;
			int cid2 = fork();
			if(cid2 == 0){
				partialSum=0;
				for(j=indexArr[1]; j<indexArr[2]; j++){
					partialSum += (short)arr[j];
				}
				pmsg = (void *)(&partialSum);
				sleep(200);
				send(getpid(), pidParent, (void *)pmsg);
				void *mean = (void *)malloc(MSGSIZE);
				recv((void *)mean);
				float var=0;
				int k;
				for(k=indexArr[1]; k<indexArr[2]; k++){
					var += (arr[k]-*(float *)mean) * (arr[k]-*(float *)mean); 
				}
				//printf(1, "%f ", var);
				void *ret_var = (void *)malloc(MSGSIZE);
				ret_var = (void *)(&var);
				sleep(200);
				send(getpid(), pidParent, (void *)ret_var);
				//printf(1,"huh");
				exit();
				}
			else{
				children[1]=cid2;
				int cid3 = fork();
				if(cid3 == 0){
					partialSum=0;
					for(j=indexArr[2]; j<indexArr[3]; j++){
						partialSum += (short)arr[j];
					}
					pmsg = (void *)(&partialSum);
					sleep(300);
					send(getpid(), pidParent, (void *)pmsg);
					void *mean = (void *)malloc(MSGSIZE);
					recv((void *)mean);
					float var=0;
					int k;
					for(k=indexArr[2]; k<indexArr[3]; k++){
						var += (arr[k]-*(float *)mean) * (arr[k]-*(float *)mean); 
					}
					//printf(1, "%d ", var);
					void *ret_var = (void *)malloc(MSGSIZE);
					ret_var = (void *)(&var);
					sleep(300);
					//printf(1,"huh");
					send(getpid(), pidParent, (void *)ret_var);
					//printf(1,"huh");
					exit();
				}
				else{
					children[2]=cid3;
					int cid4 = fork();
					if(cid4 == 0){
						partialSum=0;
						for(j=indexArr[3]; j<indexArr[4]; j++){
							partialSum += (short)arr[j];
						}
						pmsg = (void *)(&partialSum);
						sleep(400);
						send(getpid(), pidParent, (void *)pmsg);
						void *mean = (void *)malloc(MSGSIZE);
						recv((void *)mean);
						float var=0;
						int k;
						for(k=indexArr[3]; k<indexArr[4]; k++){
							var += (arr[k]-*(float *)mean) * (arr[k]-*(float *)mean); 
						}
						//printf(1, "%d ", var);
						void *ret_var = (void *)malloc(MSGSIZE);
						ret_var = (void *)(&var);
						sleep(400);
						//printf(1,"huh");
						send(getpid(), pidParent, (void *)ret_var);
						//printf(1,"huh");
						exit();
					}
					else{
						children[3]=cid4;
						int cid5 = fork();
						if(cid5 == 0){
							partialSum=0;
							for(j=indexArr[4]; j<indexArr[5]; j++){
								partialSum += (short)arr[j];
							}
							pmsg = (void *)(&partialSum);
							sleep(500);
							send(getpid(), pidParent, (void *)pmsg);
							void *mean = (void *)malloc(MSGSIZE);
							recv((void *)mean);
							float var=0;
							int k;
							for(k=indexArr[4]; k<indexArr[5]; k++){
								var += (arr[k]-*(float *)mean) * (arr[k]-*(float *)mean); 
							}
							//printf(1, "%d ", var);
							void *ret_var = (void *)malloc(MSGSIZE);
							ret_var = (void *)(&var);
							sleep(500);
							//printf(1,"huh");
							send(getpid(), pidParent, (void *)ret_var);
							//printf(1,"huh");
							exit();
						}	
						else{
							children[4]=cid5;
							int cid6 = fork();
							if(cid6 == 0){
								partialSum=0;
								for(j=indexArr[5]; j<indexArr[6]; j++){
									partialSum += (short)arr[j];
								}
								pmsg = (void *)(&partialSum);
								sleep(600);
								send(getpid(), pidParent, (void *)pmsg);
								void *mean = (void *)malloc(MSGSIZE);
								recv((void *)mean);
								float var=0;
								int k;
								for(k=indexArr[5]; k<indexArr[6]; k++){
									var += (arr[k]-*(float *)mean) * (arr[k]-*(float *)mean); 
								}
								//printf(1, "%d ", var);
								void *ret_var = (void *)malloc(MSGSIZE);
								ret_var = (void *)(&var);
								sleep(600);
								//printf(1,"huh");
								send(getpid(), pidParent, (void *)ret_var);
								//printf(1,"huh");
								exit();
							}
							else{
								children[5]=cid6;
								int cid7 = fork();
								if(cid7 == 0){
									partialSum=0;
									for(j=indexArr[6]; j<indexArr[7]; j++){
										partialSum += (short)arr[j];
									}
									pmsg = (void *)(&partialSum);
									sleep(700);
									send(getpid(), pidParent, (void *)pmsg);
									void *mean = (void *)malloc(MSGSIZE);
									recv((void *)mean);
									float var=0;
									int k;
									for(k=indexArr[6]; k<indexArr[7]; k++){
										var += (arr[k]-*(float *)mean) * (arr[k]-*(float *)mean); 
									}
									//printf(1, "%d ", var);
									void *ret_var = (void *)malloc(MSGSIZE);
									ret_var = (void *)(&var);
									sleep(700);
									//printf(1,"huh");
									send(getpid(), pidParent, (void *)ret_var);
									//printf(1,"huh");
									exit();
								}
								else{
									children[6]=cid7;
									int cid8 = fork();
									if(cid8 == 0){
										partialSum=0;
										for(j=indexArr[7]; j<indexArr[8]; j++){
											partialSum += (short)arr[j];
										}
										pmsg = (void *)(&partialSum);
										sleep(800);
										send(getpid(), pidParent, (void *)pmsg);
										void *mean = (void *)malloc(MSGSIZE);
										recv((void *)mean);
										float var=0;
										int k;
										for(k=indexArr[7]; k<indexArr[8]; k++){
											var += (arr[k]-*(float *)mean) * (arr[k]-*(float *)mean); 
										}
										//printf(1, "%d ", var);
										void *ret_var = (void *)malloc(MSGSIZE);
										ret_var = (void *)(&var);
										sleep(800);
										//printf(1,"huh");
										send(getpid(), pidParent, (void *)ret_var);
										//printf(1,"huh");
										exit();
									}
									else{
										children[7]=cid8;
										//int ll=0;
										//for(ll=0; ll<8; ll++)
											//printf(1, "%d ", children[ll]);
										sleep(10);
										void *partialSumP1 = (void *)malloc(MSGSIZE);
										void *partialSumP2 = (void *)malloc(MSGSIZE);
										void *partialSumP3 = (void *)malloc(MSGSIZE);
										void *partialSumP4 = (void *)malloc(MSGSIZE);
										void *partialSumP5 = (void *)malloc(MSGSIZE);
										void *partialSumP6 = (void *)malloc(MSGSIZE);
										void *partialSumP7 = (void *)malloc(MSGSIZE);
										void *partialSumP8 = (void *)malloc(MSGSIZE);
										recv((void *)partialSumP1);
										//printf(1, "%d \n", *(short *)(partialSumP1));
										tot_sum+=*(short *)partialSumP1;
										recv((void *)partialSumP2);
										//printf(1, "%d \n", *(short *)(partialSumP2));
										tot_sum+=*(short *)partialSumP2;
										recv((void *)partialSumP3);
										//printf(1, "%d \n", *(short *)(partialSumP3));
										tot_sum+=*(short *)partialSumP3;
										recv((void *)partialSumP4);
										//printf(1, "%d \n", *(short *)(partialSumP4));
										tot_sum+=*(short *)partialSumP4;
										recv((void *)partialSumP5);
										//printf(1, "%d \n", *(short *)(partialSumP5));
										tot_sum+=*(short *)partialSumP5;
										recv((void *)partialSumP6);
										//printf(1, "%d \n", *(short *)(partialSumP6));
										tot_sum+=*(short *)partialSumP6;
										recv((void *)partialSumP7);
										//printf(1, "%d \n", *(short *)(partialSumP7));
										tot_sum+=*(short *)partialSumP7;
										recv((void *)partialSumP8);
										//printf(1, "%d \n", *(short *)(partialSumP8));
										tot_sum+=*(short *)partialSumP8;
										float p_mean = (float)tot_sum/(float)size;
										void *multi_msg = (void *)malloc(MSGSIZE);
										multi_msg=(void *)(&p_mean);
										send_multi(getpid(), children, (void *)multi_msg);
										//sleep(200);
										void *get_var = (void *)malloc(MSGSIZE);
										int v_v=0;
										for(i=0; i<noOfProcesses; i++){
											wait();
											recv((void *)get_var);
											v_v += *(float *)get_var;
										}
										variance = v_v/(float)size;
									}
								}
							}
						}
					}
				}
			}
				
		}
	}
	
  	//------------------

  	if(type==0){ //unicast sum
		printf(1,"Sum of array for file %s is %d\n", filename,tot_sum);
	}
	else
		print_variance(variance);
	exit();
}
