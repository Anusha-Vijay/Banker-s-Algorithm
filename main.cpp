//
//  main.cpp
//  Project
//
//  Created by Anusha Vijay on 4/22/16.
//  Copyright © 2016 Anusha Vijay. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <pthread.h>



using namespace std;

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

pthread_t customer[NUMBER_OF_CUSTOMERS];
pthread_mutex_t Mymutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t update = PTHREAD_MUTEX_INITIALIZER;

class banker{
    int available[NUMBER_OF_RESOURCES];
    int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
    int allocated [NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
    int need [NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
    fstream infile;
public:
    banker();
    void display();
    int resource_request(int,int[]);
    int release(int, int[]);
    bool safe();
    
    void set_request_resource(int);
    void set_release_resource(int);
    int* rel(int);
    
};
banker b;

struct request{
    int customer_id;
    int request[3];
    int rel[3];
};

struct request r[NUMBER_OF_CUSTOMERS];

void* send_resource_request(void*);
void* send_release_request(void*);


banker::banker()
{
    
    available[0]=10;
    available[1]=5;
    available[2]=7;
    string value;
    infile.clear();
    
    //Reading from file max into the max data structure
    infile.open("Max.txt");
    if(!infile.good())
    {
        cerr<<"Check the path of the file\n";
    }
    
    else
    {
        infile.clear();
        infile.seekg(0, ios::beg);
        for(int i=0;i<NUMBER_OF_CUSTOMERS;i++)
            for(int j=0;j<NUMBER_OF_RESOURCES;j++)
            {
                getline(infile,value,',');
                maximum[i][j]=stoi(value);
            }
        
        infile.close();
    }
    
    //Reading allocated from the file allocated.txt
    infile.open("Allocated.txt");
    if(!infile.good())
    {
     cerr<<"Check the path of the file\n";
    }
    
    else
    {
        infile.clear();
        infile.seekg(0, ios::beg);
        for(int i=0;i<NUMBER_OF_CUSTOMERS;i++)
            for(int j=0;j<NUMBER_OF_RESOURCES;j++)
            {
                getline(infile,value,',');
                allocated[i][j]=stoi(value);
            }
        infile.close();

    }

    
    // Getting the Need matrix
    
    for(int i=0;i<NUMBER_OF_CUSTOMERS;i++)
        for(int j=0;j<NUMBER_OF_RESOURCES;j++)
            need[i][j]=maximum[i][j]-allocated[i][j];
    
    //Updating the available list
    for(int i=0;i<NUMBER_OF_RESOURCES;i++)
        for(int j=0;j<NUMBER_OF_CUSTOMERS;j++)
            available[i]=available[i]-allocated[j][i];
    
    
    
}


void banker::display()
{
    cout<<"\n Maxiumum matrix";
    for(int i=0;i<NUMBER_OF_CUSTOMERS;i++)
    {
        cout<<endl;
        for(int j=0;j<NUMBER_OF_RESOURCES;j++)
            cout<<maximum[i][j]<<" ";
    }
    
    cout<<"\n Allocated Matrix";
    for(int i=0;i<NUMBER_OF_CUSTOMERS;i++)
    {
        cout<<endl;
        for(int j=0;j<NUMBER_OF_RESOURCES;j++)
            cout<<allocated[i][j]<<" ";
    }
    
    
    cout<<"\n Need Matrix";
    for(int i=0;i<NUMBER_OF_CUSTOMERS;i++)
    {
        cout<<endl;
        for(int j=0;j<NUMBER_OF_RESOURCES;j++)
            cout<<need[i][j]<<" ";
    }
    cout<<"\n Available ";
    
    for(int i=0;i<NUMBER_OF_RESOURCES;i++)
        cout<<available[i]<<" ";
    
    cout<<"\n------------------------------------------------------"<<endl;
    
}

int  banker::resource_request(int cust_id, int request[])
{
//    long cust_id;
//    cust_id=(long)c_id;
//    
//    long* request;
//    request=(long*)req;
      //pthread_mutex_lock( &update );
    cout<<"\n\n"<<"Processing request of customer "<< cust_id;
    cout<<"\nCustomer "<<cust_id<<" has requested for "<<r[cust_id].request[0]<<" "<<r[cust_id].request[1]<<" "<<r[cust_id].request[2]<<endl;
    bool needi=true; // Reset when need[i] vector isn't greater or equal to request i.e when request > need[i]
    bool ave=true; // Reset when request vector isn't lesser or equal to available vector when i.e request > available
    bool _canallocate=false;
    
    for(int j=0;j<NUMBER_OF_RESOURCES;j++)
    {
        if(request[j]>need[cust_id][j])
        {
            cerr<<"\nIllegal resources requested";
            needi=false;
            break;
        }
    }
    
   if(needi==true)
   {
    for(int j=0;j<NUMBER_OF_RESOURCES;j++)
    {
        if(request[j]>available[j])
        {
            ave=false;
            break;
        }
    }
    
 //Pretending to allocate resources when
    
    
    for(int i=0;i<NUMBER_OF_RESOURCES;i++)
    {
        available[i]-=request[i];
        allocated[cust_id][i]+=request[i];
        need[cust_id][i]-=request[i];
    }
    
    _canallocate=safe();
}
//    cout<<"\n Can this be allocated? ";
//    
//    if(_canallocate)
//        cout<<"Yes";
//    else
//        cout<<"No";
//    
    //Back to the original state
    if(!_canallocate)
    {
        for(int i=0;i<NUMBER_OF_RESOURCES;i++)
        {
            available[i]+=request[i];
            allocated[cust_id][i]-=request[i];
            need[cust_id][i]+=request[i];
        }
    }
    
     //pthread_mutex_unlock( &update );
    
    return _canallocate;
    //pthread_exit(NULL);
    
}


int banker::release(int cust_id, int release[])
{
    //pthread_mutex_lock( &update );
    bool rls=true;
    cout<<"\n\n"<<" Processing release request of customer "<< cust_id;
    cout<<"\n Customer "<<cust_id<<" has requested to release for "<<release[0]<<" "<<release[1]<<" "<<release[2]<<endl;
    for (int i=0;i<NUMBER_OF_RESOURCES;i++)
    {
        if(release[i]>allocated[cust_id][i])
            rls=false;
    }
    
    if(rls==false)
    {
        cerr<<"\nRelease request exceed allocated";
        return rls;
    }
    
    for(int i=0;i<NUMBER_OF_RESOURCES;i++)
    {
        available[i]+=release[i];
        allocated[cust_id][i]-=release[i];
        need[cust_id][i]+=release[i];
    }
    
    cout<<"Resources Released! \n";
     //pthread_mutex_unlock( &update );
    return rls;
}

bool banker::safe()
{
    int work[NUMBER_OF_RESOURCES];
    bool finish[NUMBER_OF_CUSTOMERS];
    string sequence[NUMBER_OF_CUSTOMERS];
    int num=0;
   
    stringstream convert;
    
    //flags for if its safe, if need[i] is lesser than available
    bool safe=true;
    bool needi=true;
    int loopno=0;
    
    //Work=Available
    for(int i=0;i<NUMBER_OF_RESOURCES;i++)
        work[i]=available[i];
    
    // Initialise all the finish flag to false
    for(int i=0;i<NUMBER_OF_CUSTOMERS;i++)
        finish[i]=false;
    
    for(int i=0;i<NUMBER_OF_CUSTOMERS;i++)
        sequence[i]="No Process";
    
    while (loopno<10)
    {
        for(int i=0; i < NUMBER_OF_CUSTOMERS;i++)
        {
            needi=true;
            if(finish[i]==false)
            {
                for(int j=0;j < NUMBER_OF_RESOURCES;j++)
                {
                    if(need[i][j]>work[j])
                    {needi=false;
                    break;
                    }
                }
                while(needi==true&&finish[i]==false)
                {
                    for(int j=0;j<NUMBER_OF_RESOURCES;j++)
                    {
                        work[j]=work[j]+allocated[i][j];
                        
                    }
                    
                    finish[i]=true;
                    convert<<i;
                    sequence[num++]="Process"+ convert.str();
                    convert.str("");
                    
                }
            }
            
        }
        loopno++;
    }
    
    for(int i=0;i<NUMBER_OF_CUSTOMERS;i++)
        if(finish[i]==false)
            safe=false;
    
    
    if(safe)
    {
        cout<<"The request is safe and is granted";
        cout<<"\nSafety Sequence is ";
        for(int i=0;i<NUMBER_OF_CUSTOMERS;i++)
            cout<<sequence[i]<<"->";
    }
    else
        cout<<"The request denied\n";
    
    return safe;
}

void banker::set_request_resource(int cust_id)
{
    
    
    r[cust_id].customer_id=cust_id;
    for (int i=0;i<NUMBER_OF_RESOURCES;i++)
    {
        if(need[cust_id][i]==0)
            r[cust_id].request[i]=0;
        else
        r[cust_id].request[i]= rand()% (need[cust_id][i]+1);
        
    }
    
   
}

void banker::set_release_resource(int cust_id)
{
    
    r[cust_id].customer_id=cust_id;
    for (int i=0;i<NUMBER_OF_RESOURCES;i++)
    {
        if(allocated[cust_id][i]==0)
            r[cust_id].rel[i]=0;
        else
            r[cust_id].rel[i]= rand()% (allocated[cust_id][i]+1);
        
}
   
}




void* send_resource_request(void* threadarg)
{
    pthread_mutex_lock( &Mymutex );
    //cout<<"\n-------------before sending-------------\n";
    //b.display();
    //cout<<"-------------------------------------------------------------------\n";

    struct request *custrequest;
    int cust_id;
    bool ok;
    int request_resources[NUMBER_OF_RESOURCES];
    custrequest=(struct request*) threadarg;
    cust_id= custrequest->customer_id;
    for( int i=0;i<3;i++)
        request_resources[i]=custrequest->request[i];
    
    ok=b.resource_request(cust_id,request_resources);
    //cout<<"\n-------------after sending-------------\n";
    //b.display();
    //cout<<"-------------------------------------------------------------------\n";
    pthread_mutex_unlock( &Mymutex );
    return NULL;
}


void* send_release_request(void* threadarg)
{
    pthread_mutex_lock( &Mymutex );
    //cout<<"\n-------------before release-------------\n";
    //b.display();
    //cout<<"-------------------------------------------------------------------\n";
    struct request *custrequest;
    int cust_id;
    bool ok;
    int release_resources[NUMBER_OF_RESOURCES];
    custrequest=(struct request*) threadarg;
    cust_id= custrequest->customer_id;
    for( int i=0;i<3;i++)
        release_resources[i]=custrequest->rel[i];
    
    ok=b.release(cust_id,release_resources);
     //cout<<"\n-------------after release-------------\n";
    //b.display();
        //cout<<"-------------------------------------------------------------------\n";
    pthread_mutex_unlock( &Mymutex );
    return NULL;
}




int main() {
    //pthread_attr_t attr;
    cout<<"Initial state";
    b.display();
    b.safe();
    cout<<"\n-------------------------------------------------------\n";
    int grant;
    int i;
    int j=10;
    
    while(j>0)
    {
    i=rand()%4;
     {
        b.set_request_resource(i);
        grant=pthread_create(&customer[i],NULL,send_resource_request,(void*) &r[i]);
         
        b.set_release_resource(i);
        grant=pthread_create(&customer[i],NULL, send_release_request,(void*) &r[i]);
         pthread_join(customer[i], NULL);
         j--;
    }
    
    
    }
    pthread_exit(NULL);
}
