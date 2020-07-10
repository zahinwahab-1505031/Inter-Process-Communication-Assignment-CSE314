#include<unistd.h>
#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<queue>
#include<string.h>
using namespace std;
#define Q1SIZE 5
#define Q2SIZE 5
#define Q3SIZE 5
//#define VANILLA 0
//#define CHOCOLATE 1
class Cake{
public:
	int isDecorated;
	char flavor[100];//0 for vanilla 1 for chocolate
	int id;
	Cake(int isDecorated1,char *flavor1,int id1){
		isDecorated = isDecorated1;
		strcpy(flavor,flavor1);
		id = id1;
	}

};

int globalID = 1;
//semaphore to control sleep and wake up
sem_t empty1;
sem_t full1;
sem_t empty2;
sem_t full2;
sem_t empty3;
sem_t full3;
//3 separate queues
queue<Cake> q1;
queue<Cake> q2;
queue<Cake> q3;
//3 separate mutex locks
pthread_mutex_t lock1;
pthread_mutex_t lock2;
pthread_mutex_t lock3;

//lock for console
pthread_mutex_t lockC;
//lock for global ID
pthread_mutex_t lockG;
void init_semaphore()
{
	sem_init(&empty1,0,Q1SIZE);
	sem_init(&full1,0,0);
	pthread_mutex_init(&lock1,0);

	sem_init(&empty2,0,Q2SIZE);
	sem_init(&full2,0,0);
	pthread_mutex_init(&lock2,0);

	sem_init(&empty3,0,Q3SIZE);
	sem_init(&full3,0,0);
	pthread_mutex_init(&lock3,0);

	pthread_mutex_init(&lockC,0);
	pthread_mutex_init(&lockG,0);
}

void * ChefX(void * arg)
{	
	printf("%s\n",(char*)arg);
	int i;
	for(i=1;;i++)
	{
		pthread_mutex_lock(&lockC);
		printf("ChefX wants to acquire the queue 1\n");
		pthread_mutex_unlock(&lockC);

		sem_wait(&empty1);
		pthread_mutex_lock(&lock1);	

		pthread_mutex_lock(&lockC);
		printf("ChefX got access to queue 1\n");
		pthread_mutex_unlock(&lockC);	
		
		sleep(1);	
		pthread_mutex_lock(&lockG);	
		Cake cake(0,"CHOCOLATE",globalID);
		globalID++;
		pthread_mutex_unlock(&lockG);
		q1.push(cake);
		
		pthread_mutex_lock(&lockC);
		printf("ChefX produced item %d -> flavor: %s \n",cake.id,cake.flavor);
		printf("queue 1 --> size: %d\n",q1.size());
		pthread_mutex_unlock(&lockC);
		
		pthread_mutex_lock(&lockC);
		printf("ChefX wants to release queue 1\n");
		pthread_mutex_unlock(&lockC);

		pthread_mutex_unlock(&lock1);
		sem_post(&full1);

		pthread_mutex_lock(&lockC);
		printf("ChefX releases queue 1\n");
		pthread_mutex_unlock(&lockC);
	}
}

void * ChefY(void * arg)
{	
	printf("%s\n",(char*)arg);
	int i;
	for(i=1;;i++)
	{
		pthread_mutex_lock(&lockC);
		printf("ChefY wants to acquire the queue 1\n");
		pthread_mutex_unlock(&lockC);

		sem_wait(&empty1);
		pthread_mutex_lock(&lock1);		
		
		pthread_mutex_lock(&lockC);
		printf("ChefY got access to queue 1\n");
		pthread_mutex_unlock(&lockC);

		sleep(1);
		pthread_mutex_lock(&lockG);	
		Cake cake(0,"VANILLA",globalID);
		globalID++;
		pthread_mutex_unlock(&lockG);
		q1.push(cake);
		
		pthread_mutex_lock(&lockC);
		printf("ChefY produced item %d -> flavor: %s \n",cake.id,cake.flavor);
		printf("queue 1 --> size: %d\n",q1.size());
		pthread_mutex_unlock(&lockC);
		
		
		pthread_mutex_lock(&lockC);
		printf("ChefY wants to release queue 1\n");
		pthread_mutex_unlock(&lockC);

		pthread_mutex_unlock(&lock1);
		sem_post(&full1);

		pthread_mutex_lock(&lockC);
		printf("ChefY releases queue 1\n");
		pthread_mutex_unlock(&lockC);
	}
}
void * ChefZ(void * arg)
{
	printf("%s\n",(char*)arg);
	int i;
	for(i=1;;i++)
	{	
		pthread_mutex_lock(&lockC);
		printf("ChefZ wants to acquire the queue 1\n");
		pthread_mutex_unlock(&lockC);

		sem_wait(&full1);
 		pthread_mutex_lock(&lock1);

		pthread_mutex_lock(&lockC);
		printf("ChefZ got access to queue 1\n");
		pthread_mutex_unlock(&lockC);
	
			
		sleep(1);
		Cake cake= q1.front();
		
		pthread_mutex_lock(&lockC);
		printf("<ChefZ saw item %d -> flavor %s>\n",cake.id,cake.flavor);	
		printf("queue 1 --> size: %d\n",q1.size());
		pthread_mutex_unlock(&lockC);


		pthread_mutex_unlock(&lock1);
	

		if(strcmp(cake.flavor,"VANILLA")==0){// vanilla cake so push it to queue 2
		pthread_mutex_lock(&lockC);
		printf("ChefZ wants to acquire the queue 2\n");
		pthread_mutex_unlock(&lockC);


		sem_wait(&empty2);
		

		pthread_mutex_lock(&lockC);
		printf("ChefZ got access to queue 2\n");
		pthread_mutex_unlock(&lockC);

		pthread_mutex_lock(&lockC);
		printf("ChefZ wants to acquire the queue 1\n");
		pthread_mutex_unlock(&lockC);

		
 		pthread_mutex_lock(&lock1);

		pthread_mutex_lock(&lockC);
		printf("ChefZ got access to queue 1\n");
		pthread_mutex_unlock(&lockC);
	

		//from here
		q1.pop();
		
		pthread_mutex_lock(&lockC);
		printf("<ChefZ popped item %d -> flavor %s>\n",cake.id,cake.flavor);	
		printf("queue 1 --> size: %d\n",q1.size());
		pthread_mutex_unlock(&lockC);

		pthread_mutex_lock(&lockC);
		printf("ChefZ wants to release queue 1\n");
		pthread_mutex_unlock(&lockC);

		pthread_mutex_unlock(&lock1);
		sem_post(&empty1);

		pthread_mutex_lock(&lockC);
		printf("ChefZ releases queue 1\n");
		pthread_mutex_unlock(&lockC);
		//to here
		
		cake.isDecorated=1;
		pthread_mutex_lock(&lock2);
		q2.push(cake);

		pthread_mutex_lock(&lockC);
		printf("<ChefZ decorate item %d -> flavor: %s> \n",cake.id,cake.flavor);
		printf("queue 2 --> size: %d\n",q2.size());
		pthread_mutex_unlock(&lockC);

		pthread_mutex_lock(&lockC);
		printf("ChefZ wants to release queue 2\n");
		pthread_mutex_unlock(&lockC);

		pthread_mutex_unlock(&lock2);
		sem_post(&full2);

		pthread_mutex_lock(&lockC);
		printf("ChefZ releases queue 2\n");
		pthread_mutex_unlock(&lockC);

		}
		else if(strcmp(cake.flavor,"CHOCOLATE")==0){ // chocolate cake so push it to queue 3

		pthread_mutex_lock(&lockC);
		printf("ChefZ wants to acquire the queue 3\n");
		pthread_mutex_unlock(&lockC);

		sem_wait(&empty3);
		

		pthread_mutex_lock(&lockC);
		printf("ChefZ got access to queue 3\n");
		pthread_mutex_unlock(&lockC);

		pthread_mutex_lock(&lockC);
		printf("ChefZ wants to acquire the queue 1\n");
		pthread_mutex_unlock(&lockC);

		
 		pthread_mutex_lock(&lock1);

		pthread_mutex_lock(&lockC);
		printf("ChefZ got access to queue 1\n");
		pthread_mutex_unlock(&lockC);

		//from here
		q1.pop();
		
		pthread_mutex_lock(&lockC);
		printf("<ChefZ popped item %d -> flavor %s>\n",cake.id,cake.flavor);	
		printf("queue 1 --> size: %d\n",q1.size());
		pthread_mutex_unlock(&lockC);

		pthread_mutex_lock(&lockC);
		printf("ChefZ wants to release queue 1\n");
		pthread_mutex_unlock(&lockC);

		pthread_mutex_unlock(&lock1);
		sem_post(&empty1);

		pthread_mutex_lock(&lockC);
		printf("ChefZ releases queue 1\n");
		pthread_mutex_unlock(&lockC);
		//to here
		sleep(1);


		cake.isDecorated=1;
		pthread_mutex_lock(&lock3);	
		q3.push(cake);
	

		pthread_mutex_lock(&lockC);
		printf("<ChefZ decorate item %d -> flavor: %s> \n",cake.id,cake.flavor);
		printf("queue 3 --> size: %d\n",q3.size());
		pthread_mutex_unlock(&lockC);

		pthread_mutex_lock(&lockC);
		printf("ChefZ wants to release queue 3\n");
		pthread_mutex_unlock(&lockC);

		pthread_mutex_unlock(&lock3);
		sem_post(&full3);

		pthread_mutex_lock(&lockC);
		printf("ChefZ releases queue 3\n");
		pthread_mutex_unlock(&lockC);
		}
			
		
	}
}



void * Waiter1(void * arg)
{
	printf("%s\n",(char*)arg);
	int i;
	for(i=1;;i++)
	{
		pthread_mutex_lock(&lockC);
		printf("Waiter 1 wants to acquire the queue 3\n");
		pthread_mutex_unlock(&lockC);	

		sem_wait(&full3);
 		pthread_mutex_lock(&lock3);

 		pthread_mutex_lock(&lockC);
		printf("Waiter 1 got access to queue 3\n");
		pthread_mutex_unlock(&lockC);
			
		sleep(1);
		Cake cake = q3.front();
		q3.pop();
		
		pthread_mutex_lock(&lockC);
		printf("<<Waiter 1 delivered item %d -> flavor %s and decoration %d>>\n",cake.id,cake.flavor,cake.isDecorated);
		printf("queue 3 --> size: %d\n",q3.size());
		pthread_mutex_unlock(&lockC);


		pthread_mutex_lock(&lockC);
		printf("Waiter1 wants to release queue 3\n");
		pthread_mutex_unlock(&lockC);
	
		pthread_mutex_unlock(&lock3);
		sem_post(&empty3);

		pthread_mutex_lock(&lockC);
		printf("Waiter1 releases queue 3\n");
		pthread_mutex_unlock(&lockC);
	
	}
}

void * Waiter2(void * arg)
{
	printf("%s\n",(char*)arg);
	int i;
	for(i=1;;i++)
	{	pthread_mutex_lock(&lockC);
		printf("Waiter 2 wants to acquire the queue 2\n");
		pthread_mutex_unlock(&lockC);

		sem_wait(&full2);
 		pthread_mutex_lock(&lock2);

 		pthread_mutex_lock(&lockC);
		printf("Waiter 2 got access to queue 2\n");
		pthread_mutex_unlock(&lockC);
			
		sleep(1);
		Cake cake = q2.front();
		q2.pop();
		
		pthread_mutex_lock(&lockC);
		printf("<<Waiter 2 delivered item %d -> flavor %s and decoration %d>>\n",cake.id,cake.flavor,cake.isDecorated);
		printf("queue 2 --> size: %d\n",q2.size());
		pthread_mutex_unlock(&lockC);


		pthread_mutex_lock(&lockC);
		printf("Waiter2 wants to release queue 2\n");
		pthread_mutex_unlock(&lockC);
	

		pthread_mutex_unlock(&lock2);
		sem_post(&empty2);

		pthread_mutex_lock(&lockC);
		printf("Waiter2 releases queue 2\n");
		pthread_mutex_unlock(&lockC);
	
	}
}


int main(void)
{	pthread_t thread1; //chef x
	pthread_t thread2; //chef y
	pthread_t thread3; //chef z
	pthread_t thread4; //waiter 1
	pthread_t thread5; //waiter 2
	init_semaphore();
	
	char * message1 = "i am Chef-X , I produce chocolate cakes";
	char * message2 = "i am Chef-Y , I produce vanilla cakes";
	char * message3 = "i am Chef-Z , I decorate cakes";
	char * message4 = "i am Waiter-1 , I deliver chocolate cakes";
	char * message5 = "i am Waiter-2 , I deliver vanilla cakes";
	
	
	pthread_create(&thread1,NULL,ChefX,(void*)message1 );
	pthread_create(&thread2,NULL,ChefY,(void*)message2 );
	pthread_create(&thread3,NULL,ChefZ,(void*)message3 );
	pthread_create(&thread4,NULL,Waiter1,(void*)message4 );
	pthread_create(&thread5,NULL,Waiter2,(void*)message5 );
	
	//while(1);
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	pthread_join(thread3,NULL);
	pthread_join(thread4,NULL);
	pthread_join(thread5,NULL);
	freopen("output.txt","w",stdout);
	
	return 0;
}
