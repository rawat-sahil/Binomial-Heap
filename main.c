#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>

struct node{
    long int n;
    int degree;
    char * filename;
    struct node* parent;
    struct node* child;
    struct node* sibling;
};

struct node* link(struct node* parent, struct node* child){
    struct node*temp=child->sibling;

    child->parent=parent;
    child->sibling=parent->child;
    parent->child=child;
    parent->degree=parent->degree+1;

    return temp;
}

struct node* merge(struct node* heap1,struct node* heap2){

    if(heap1==NULL) return heap2;
    if(heap2==NULL) return heap1;

    struct node* heap= (struct node*)malloc(sizeof(struct node) );
    struct node* temp=heap;

    while(heap1!=NULL && heap2!=NULL){
        if(heap1->degree<heap2->degree || ((heap1->degree==heap2->degree)&&(heap1->n>=heap2->n) ) ){
            temp->sibling=heap1;
            heap1=heap1->sibling;
            temp=temp->sibling;
            temp->sibling=NULL;
        }
        else{
            temp->sibling=heap2;
            heap2=heap2->sibling;
            temp=temp->sibling;
            temp->sibling=NULL;
        }

    }

    if(heap1!=NULL){
        temp->sibling=heap1;
    }
    if(heap2!=NULL){
        temp->sibling=heap2;
    }

    temp=heap;
    heap=heap->sibling;
    free(temp);
    return heap;
}


struct node* heap_union(struct node* heap1, struct node* heap2){

    struct node *heap=merge(heap1,heap2);
    if(heap==NULL)return heap;

    struct node* prev=(struct node*)malloc(sizeof(struct node));
    prev->sibling=heap;
    heap=prev;
    struct node* curr=prev->sibling;
    struct node* next=curr->sibling;

    while(next!=NULL){
        if(curr->degree!=next->degree){
            prev=curr;
            curr=next;

        }
        else{

            if(next->sibling==NULL || next->degree!=next->sibling->degree){
                if(curr->n>=next->n){
                    curr->sibling=link(curr,next);
                    next=curr;
                }
                else{
                    link(next,curr);
                    prev->sibling=next;
                    curr=next;
                }
            }
            else {
                prev=curr;
                curr=next;
            }
        }
        next=next->sibling;
    }


    return heap->sibling;

}

struct node* insert(struct node* heap, struct node* val){
    return heap_union(heap,val);
}

struct node* max_val(struct node * heap){
    if(heap==NULL)return heap;

    struct node*temp=NULL;
    long int max=-1;
    while(heap!=NULL){
        if(heap->n>max){
            max=heap->n;
            temp=heap;
        }
        heap=heap->sibling;
    }

    return temp;
}

struct node* reverse(struct node* heap){
    if(heap==NULL)return NULL;

    struct node* prev=NULL;
    struct node* curr=heap;
    struct node* next=curr->sibling;

    while(next!=NULL){
        curr->sibling=prev;
        prev=curr;
        curr=next;
        next=next->sibling;
    }
    curr->sibling=prev;
    return curr;
}

struct node* extract_max(struct node* heap){
    struct node* val=max_val(heap);
    if(val==NULL || heap==NULL)return heap;

    struct node* prev=(struct node*)malloc(sizeof(struct node));
    prev->sibling=heap;
    heap=prev;
    struct node* curr=prev->sibling;
    struct node* next=curr->sibling;

    while(curr!=val){
        prev=curr;
        curr=next;
        next=next->sibling;
    }
    prev->sibling=next;

    curr=curr->child;
    struct node*iter=curr;
    while(iter!=NULL){
    iter->parent=NULL;
    iter=iter->sibling;
    }

    curr=reverse(curr);

    return heap_union(heap->sibling,curr);

}






struct node* createNode(){
        struct node* ret=(struct node*)malloc(sizeof(struct node));
        ret->sibling=NULL;
        ret->parent=NULL;
        ret->child=NULL;
        ret->degree=0;
        return ret;
}


struct priority_queue{
    struct node * heap;
};

struct priority_queue* createPQ(){
    struct priority_queue* pq=(struct priority_queue*)malloc(sizeof(struct priority_queue));
    return pq;
}

void enqueue(struct priority_queue* pq, struct node* val){
    if(pq->heap==NULL){
        pq->heap=val;
        return ;
    }
    pq->heap=insert(pq->heap, val);
}
void dequeue(struct priority_queue* pq){
    if(pq->heap==NULL){
        printf("queue already empty\n" );
        return ;
    }
    pq->heap=extract_max(pq->heap);
}

struct node* frontval(struct priority_queue*pq){
    return max_val(pq->heap);
}

int count_word(char* filename, char* keyword,int  flag){
    int count=0;
    char filen[100]="./files/";
    char buff[100];
    strcat(filen,filename);
    // printf("%s",filen);
    FILE *file=fopen(filen,"r");
    if(file==NULL){
        printf("File can't be open\n");
        exit(-1);
    }

    while(fscanf(file,"%s",buff)==1){
        if(strcmp(keyword,buff)==0)count++;
        if(flag) printf("%s ",buff);
    }

    if(flag)printf("\n\n\n\n");

    fclose(file);

    return count;
}

int main(){
    struct priority_queue *pq=createPQ();
    struct priority_queue *pq1=createPQ();
    // for(int i=0;i<25;++i){
    //     struct node* temp=createNode();
    //     temp->n=rand()%100;
    //     printf("%ld\n",temp->n);
    //     enqueue(pq,temp);
    // }
    // printf("end\v");
    // for(int i=0;i<25;++i){
    //     printf("%ld\n",frontval(pq)->n );
    //     dequeue(pq);
    // }
    struct dirent *de;
    DIR * directory=opendir("./files");
    if(directory==NULL){
        printf("Directory not found");
        exit(-1);
    }

    char keyword[100];
    int relevantDocuments;
    printf("Enter the keyword: ");
    scanf("%s",keyword);
    printf("Enter the no. of relevant documents: ");
    scanf("%d",&relevantDocuments);
    while((de=readdir(directory))!=NULL){
        if(strcmp(".",de->d_name)!=0 && strcmp("..",de->d_name)!=0){
        int cou=count_word(de->d_name,keyword,0);
        // printf("%s  %d\n",de->d_name,cou);

        struct node* temp=createNode();
        struct node* temp1=createNode();

        temp->n=cou;
        temp->filename=(char*)malloc(100*sizeof(char));

        temp1->n=cou;
        temp1->filename=(char*)malloc(100*sizeof(char));

        strcpy(temp->filename,de->d_name);
        strcpy(temp1->filename,de->d_name);

        enqueue(pq,temp);
        enqueue(pq1,temp1);
        }
    }
    printf("\v");
    printf("The relevance order is: ");

    struct priority_queue* temp=createPQ();
    for(int i=1;i<=relevantDocuments;++i){
        if(frontval(pq)->n<=0)break;
        printf("%s(%ld) ",frontval(pq)->filename,frontval(pq)->n);
        // printf("%d. %ld occurences in file %s\n",i,frontval(pq)->n,frontval(pq)->filename);
        // struct node* temp=createNode();
        dequeue(pq);
        // dequeue(pq1);
    }
    printf("\n");

    for(int i=1;i<=relevantDocuments;++i){
        if(frontval(pq)->n<=0)break;

        printf("%s(%ld): ",frontval(pq1)->filename,frontval(pq1)->n );
        count_word(frontval(pq1)->filename,keyword,1);
        dequeue(pq1);
        printf("\n\n\n\n\n\n" );
    }

    closedir(directory);






    return 0;

}
