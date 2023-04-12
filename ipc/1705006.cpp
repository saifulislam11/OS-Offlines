#include<iostream>
#include<cstdio>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
#include<cstring>
#include <time.h>
#include <vector>
#include<fstream>
#include <random>
#include <chrono>

using namespace std;

FILE *out_file= fopen("output.txt","w");

int number_of_passengers;
int M;
int N;
int P;
int w,x,y,z;
sem_t empty_kiosk;
sem_t* belt_arr;

pthread_mutex_t channelForward,channelBackward;
pthread_mutex_t mutex_security,mutex_kiosk,vip_mutex,channel_mutex;

sem_t boarding,special_kiosk,channel;

int timeCount = 0;
int leftKioskCount=0;
int leftTOright = 1;
int departVIPcount=0;
int enterVIPCount=0;
bool* kiosk_arr;
auto timer_current = std::chrono::steady_clock::now();
auto timer_start = std::chrono::steady_clock::now();
std::chrono::duration<double> elapsed_seconds;
void VIPchannelForward(char* id,const char* vip){
    timer_current = std::chrono::steady_clock::now();
    elapsed_seconds = timer_current-timer_start;
    fprintf(out_file,"Passenger%s %s has arrived at VIP channel(LeftToRight) at time %d\n",id,vip,(int)elapsed_seconds.count());
    printf("Passenger%s %s has arrived at VIP channel(LeftToRight) at time %d\n",id,vip,(int)elapsed_seconds.count());
    fflush(stdout);
    pthread_mutex_lock(&channelForward);
    departVIPcount++;
    if(departVIPcount == 1){
        pthread_mutex_lock(&vip_mutex);
        pthread_mutex_lock(&channel_mutex);
    }
    pthread_mutex_unlock(&channelForward);
    sleep(z);       //sleeep
    timer_current = std::chrono::steady_clock::now();
    elapsed_seconds = timer_current-timer_start;
    fprintf(out_file,"Passenger%s %s has passed VIP channel(LeftToRight) at time %d\n",id,vip,(int)elapsed_seconds.count());
    printf("Passenger%s %s has passed VIP channel(LeftToRight) at time %d\n",id,vip,(int)elapsed_seconds.count());
    fflush(stdout);
    pthread_mutex_lock(&channelForward);
    departVIPcount--;
    if(departVIPcount == 0){
        pthread_mutex_unlock(&vip_mutex);
        pthread_mutex_unlock(&channel_mutex);
    }
    pthread_mutex_unlock(&channelForward);

}
void VIPchannelBackward(char* id,const char* vip){
    timer_current = std::chrono::steady_clock::now();
    elapsed_seconds = timer_current-timer_start;
    fprintf(out_file,"Passenger%s %s has arrived at VIP channel(RightToLeft)  at time %d\n",id,vip,(int)elapsed_seconds.count());
    
    printf("Passenger%s %s has arrived at VIP channel(RightToLeft)  at time %d\n",id,vip,(int)elapsed_seconds.count());
    fflush(stdout);
    pthread_mutex_lock(&vip_mutex);
    pthread_mutex_unlock(&vip_mutex);
    pthread_mutex_lock(&channelBackward);
    enterVIPCount++;
    if(enterVIPCount == 1){
        pthread_mutex_lock(&channel_mutex);
    }
    pthread_mutex_unlock(&channelBackward);
    sleep(z);       //sleeep
    timer_current = std::chrono::steady_clock::now();
    elapsed_seconds = timer_current-timer_start;
    fprintf(out_file,"Passenger%s %s has passed VIP channel(RightToLeft)  at time %d\n",id,vip,(int)elapsed_seconds.count());

    printf("Passenger%s %s has passed VIP channel(RightToLeft)  at time %d\n",id,vip,(int)elapsed_seconds.count());
    fflush(stdout);
     pthread_mutex_lock(&channelBackward);
    enterVIPCount--;
    if(enterVIPCount == 0){
        pthread_mutex_unlock(&channel_mutex);
    }
    pthread_mutex_unlock(&channelBackward);

}
bool forgotBoard(char* id,const char* vip){
    int forgot_board = rand()%2;
    if(forgot_board==1) return true;
    else{
        timer_current = std::chrono::steady_clock::now();
    elapsed_seconds = timer_current-timer_start;
        fprintf(out_file,"Passenger%s %s has started waiting to be boarded at time %d\n",id,vip,(int)elapsed_seconds.count());
        printf("Passenger%s %s has started waiting to be boarded at time %d\n",id,vip,(int)elapsed_seconds.count());
        fflush(stdout);
        sem_wait(&boarding);
        timer_current = std::chrono::steady_clock::now();
    elapsed_seconds = timer_current-timer_start;
        fprintf(out_file,"Passenger%s %s has started boarding the plane at time %d\n",id,vip,(int)elapsed_seconds.count());

        printf("Passenger%s %s has started boarding the plane at time %d\n",id,vip,(int)elapsed_seconds.count());
        fflush(stdout);
        sleep(y);
        timer_current = std::chrono::steady_clock::now();
    elapsed_seconds = timer_current-timer_start;
        fprintf(out_file,"Passenger%s %s has started boarding the plane at time %d\n",id,vip,(int)elapsed_seconds.count());

        printf("Passenger%s %s has boarded the plane at time %d\n",id,vip,(int)elapsed_seconds.count());
        fflush(stdout);
        sem_post(&boarding);
        return false;
    }


}
void specialKiosk(char* id,const char* vip){
    timer_current = std::chrono::steady_clock::now();
    elapsed_seconds = timer_current-timer_start;
    fprintf(out_file,"Passenger%s %s has started waiting in special kiosk at time %d \n",id,vip,(int)elapsed_seconds.count());

    printf("Passenger%s %s has started waiting in special kiosk at time %d \n",id,vip,(int)elapsed_seconds.count());
    fflush(stdout);
    sem_wait(&special_kiosk);
    timer_current = std::chrono::steady_clock::now();
    elapsed_seconds = timer_current-timer_start;
    fprintf(out_file,"Passenger%s %s has started using special kiosk at time %d \n",id,vip,(int)elapsed_seconds.count());

    printf("Passenger%s %s has started using special kiosk at time %d \n",id,vip,(int)elapsed_seconds.count());
    fflush(stdout);
    sleep(w);
    timer_current = std::chrono::steady_clock::now();
    elapsed_seconds = timer_current-timer_start;
    fprintf(out_file,"Passenger%s %s has got the special kiosk at time %d \n",id,vip,(int)elapsed_seconds.count());

    printf("Passenger%s %s has got the special kiosk at time %d \n",id,vip,(int)elapsed_seconds.count());
    fflush(stdout);
    sem_post(&special_kiosk);
}
void* Airport_proccess(void* arg){
    // //-----------splitting vip id-------------//
    // vector<char*> v;
    
    // while(chars_array)
    // {
    //     v.push_back(chars_array);
    //     chars_array = strtok(NULL, " ");
    // }
    // const char* vip="";
    // if(v[1]=="1") vip="(VIP)";
    // printf("%s \n",v.at(0));
    char temp[5],vip_char;
    const char* vip = "";
    bool vip_status=false;
    strcpy(temp,(char*)arg);
    for(int i =0;i<5;i++){
        if(temp[i]==' '){
            vip_char = temp[i+1];
            break;
        }
    }
    if(vip_char=='1'){
        vip = "(VIP)";
        vip_status = true;
    }
    char* chars_array = strtok((char*)arg, " ");
    // //----------end of split--------------//
    
    // clock_t t;
    // t = clock();
    
    sem_wait(&empty_kiosk);
    // //-----getting sem value--------//
 
    
    // t = clock() - t;
    // double time_taken = ((double)t)/CLOCKS_PER_SEC; // in secondsS
   
    
    // pthread_mutex_lock(&mutex_kiosk);
    // leftKioskCount++;           //one passenger departure
    
	// pthread_mutex_unlock(&mutex_kiosk);
    int flag;
    for(flag=0;flag<M;flag++){
        if(!kiosk_arr[flag]){
            kiosk_arr[flag]=true;
            break;
        }
    } 
    timer_current = std::chrono::steady_clock::now();
    elapsed_seconds = timer_current-timer_start;
    fprintf(out_file,"Passenger%s %s has started self-check in at kiosk %d  at time %d \n",(char*)arg,vip,flag+1,(int)elapsed_seconds.count());

    printf("Passenger%s %s has started self-check in at kiosk %d  at time %d \n",(char*)arg,vip,flag+1,(int)elapsed_seconds.count());
    fflush(stdout);
    sleep(w);
    timer_current = std::chrono::steady_clock::now();
    elapsed_seconds = timer_current-timer_start;
    fprintf(out_file,"Passenger%s %s  has finished check in  at time %d\n",(char*)arg,vip,(int)elapsed_seconds.count());

    printf("Passenger%s %s  has finished check in  at time %d\n",(char*)arg,vip,(int)elapsed_seconds.count());
    fflush(stdout);
    sem_post(&empty_kiosk);
    kiosk_arr[flag]=false;
    // -------------security part------------- //
    if(!vip_status){
    
    int i = rand()%N;
    timer_current = std::chrono::steady_clock::now();
    elapsed_seconds = timer_current-timer_start;
    fprintf(out_file,"Passenger%s %s  has started waiting for security check in belt %d  at time %d\n",(char*)arg,vip,i+1,(int)elapsed_seconds.count());

    printf("Passenger%s %s  has started waiting for security check in belt %d  at time %d\n",(char*)arg,vip,i+1,(int)elapsed_seconds.count());
    fflush(stdout);
    sem_wait(&belt_arr[i]);
    timer_current = std::chrono::steady_clock::now();
    elapsed_seconds = timer_current-timer_start;
    fprintf(out_file,"Passenger%s %s  has started the security check  at time %d\n",(char*)arg,vip,(int)elapsed_seconds.count());

    printf("Passenger%s %s  has started the security check  at time %d\n",(char*)arg,vip,(int)elapsed_seconds.count());
    fflush(stdout);
    sleep(x);
    sem_post(&belt_arr[i]);
    timer_current = std::chrono::steady_clock::now();
    elapsed_seconds = timer_current-timer_start;
    fprintf(out_file,"Passenger%s %s has crossed the security check  at time %d\n",(char*)arg,vip,(int)elapsed_seconds.count());

    printf("Passenger%s %s has crossed the security check  at time %d\n",(char*)arg,vip,(int)elapsed_seconds.count());
    fflush(stdout);
    }
    else{
        VIPchannelForward((char*)arg,vip);
    }
    
    //---------------end of security check------------//
    
    //---------------boarding operation----------------//
    while (forgotBoard((char*)arg,vip))
    {
        //repeat until gets the board
        fprintf(out_file,"Passenger%s %s has forgotten the board.Going Backward\n",(char*)arg,vip);

        printf("Passenger%s %s has forgotten the board.Going backward\n",(char*)arg,vip);
        fflush(stdout);
        VIPchannelBackward((char*)arg,vip);
        specialKiosk((char*)arg,vip);
        VIPchannelForward((char*)arg,vip);
    }
    
    //------------END OF BOARDING ----------------//
    
    pthread_exit(arg);

    
}
// void* TIME_COUNT(void* arg){
//     while (1)
//     {
//         sleep(1);
//         timeCount++;

//     }
    
// }


int main(int argc, char* argv[])
{
    int passenger_itr;
    //---------------I/O---------------//
    ifstream fin("input.txt");
    fin >> M >> N >> P;
    fin >> w >> x >> y >> z;
    belt_arr = new sem_t[N];
    kiosk_arr = new bool[M];
    int res;
    for(int i=0;i<N;i++){
        res=sem_init(&belt_arr[i],0,P);
        if(res != 0){
            printf("Failed\n");
         }
    }
    //INIT KIOSK VALUE
    res = sem_init(&empty_kiosk,0,M);
    if(res != 0){
        printf("Failed\n");
    }
    res = sem_init(&boarding,0,1);
    if(res != 0){
        printf("Failed\n");
    }
    res = sem_init(&special_kiosk,0,1);
    if(res != 0){
        printf("Failed\n");
    }
    res = sem_init(&channel,0,1);
    if(res != 0){
        printf("Failed\n");
    }
    res = pthread_mutex_init(&mutex_security,NULL);
    if(res != 0){
        printf("Failed\n");
    }
    res = pthread_mutex_init(&vip_mutex,NULL);
    if(res != 0){
        printf("Failed\n");
    }
    res = pthread_mutex_init(&mutex_kiosk,NULL);
    if(res != 0){
        printf("Failed\n");
    }
    res = pthread_mutex_init(&channelForward,NULL);
    if(res != 0){
        printf("Failed\n");
    }
    res = pthread_mutex_init(&channelBackward,NULL);
    if(res != 0){
        printf("Failed\n");
    }
    
    pthread_t timer;
    random_device rd;
    mt19937 gen(rd());
    poisson_distribution<int> random_arrival(12);
    // pthread_create(&timer,NULL,TIME_COUNT,NULL);
    passenger_itr = rand()%10 + 1 ;
    int count_itr = 0;
    while(count_itr <= passenger_itr){
        count_itr++;
        number_of_passengers = random_arrival(gen);
        timer_current = std::chrono::steady_clock::now();
        elapsed_seconds = timer_current-timer_start;
        fprintf(out_file,"%d number of passengers entered the airport at time %d\n",number_of_passengers,(int)elapsed_seconds.count());

        printf("%d number of passengers entered the airport at time %d\n",number_of_passengers,(int)elapsed_seconds.count());
        fflush(stdout);
        pthread_t producers[number_of_passengers];
        int duration = 10;
        int passenger_time[duration] = {};
        exponential_distribution<double> distribution(3.5);
        default_random_engine generator;
        for(int i = 0; i < number_of_passengers; i++)
        {
            double number = distribution(generator);
            if (number<1.0) ++passenger_time[int(duration*number)];
        }
        int count_psngr = 0;
        for(int i =0;i<duration;i++){
            sleep(1);
            for(int j = 0 ;j<passenger_time[i];j++){
                char *id = new char[5];
                int vip = rand()%2;
                const char* setVIP="";
                if(vip == 1){
                    setVIP="(VIP)";

                }
                string isVip = to_string(count_psngr+1)+" "+to_string(vip);
                strcpy(id,isVip.c_str());        //"ID VIP_status"
                timer_current = std::chrono::steady_clock::now();
                elapsed_seconds = timer_current-timer_start;
                fprintf(out_file,"Passenger%s %d has arrived at the airport at time %d\n",setVIP,count_psngr+1,(int)elapsed_seconds.count());

                printf("Passenger%s %d has arrived at the airport at time %d\n",setVIP,count_psngr+1,(int)elapsed_seconds.count());
                fflush(stdout);
                res = pthread_create(&producers[count_psngr],NULL,Airport_proccess,(void *)id);
                if(res != 0){
                    printf("Thread creation failed\n");
                }

                count_psngr++;
            }

        }

        // for(int i = 0; i < number_of_passengers; i++){
        //     char *id = new char[5];
        //     int vip = rand()%2;
        //     string isVip = to_string(i+1)+" "+to_string(vip);
        //     strcpy(id,isVip.c_str());        //"ID VIP_status"
            

        //     res = pthread_create(&producers[i],NULL,Airport_proccess,(void *)id);

        //     if(res != 0){
        //         printf("Thread creation failed\n");
        //     }
        // }
        for(int i = 0; i < number_of_passengers; i++){
            void *result;
            pthread_join(producers[i],&result);
        // printf("%s",(char*)result);
        }
        sleep(120);
    }

    

    

    
    for(int i=0;i<M;i++){
        res=sem_destroy(&belt_arr[i]);
        if(res != 0){
            printf("Failed\n");
         }
    }
    res = sem_destroy(&empty_kiosk);
    if(res != 0){
        printf("Failed\n");
    }

    res = pthread_mutex_destroy(&mutex_security);
    if(res != 0){
        printf("Failed\n");
    }
    res = pthread_mutex_destroy(&mutex_kiosk);
    if(res != 0){
        printf("Failed\n");
    }
    fclose(out_file);
    
    return 0;
}
