#include<stdio.h>
#include<stdlib.h>


typedef struct vector{
	size_t cap;
	size_t size;
	int *arr;
} vector;

vector new_vector(){
	vector a;
	printf("here");
	a.cap=sizeof(int);
	a.size=0;
	a.arr=(int *)malloc(sizeof(int)*(a.cap));
	return a;
}

void insert(vector *a,int val){
	if(a->size==a->cap){
		a->cap=2*a->cap;
		a->arr=(int *)realloc(a->arr,(a->cap)*sizeof(int));
	}
	a->arr[a->size++]=val;
}

int look(vector *a, int i){
	if(i>=a->size){
		perror("out of index");
		exit(EXIT_FAILURE);
	}
	return a->arr[i];
}

void clear(vector *a){
	a->size=0;
}


int main(){
	printf("Hello World!\n");
	vector v=new_vector();
	for(int i=0; i<600000;i++) insert(&v,i);
	for(int i=0; i<600000;i++) printf("%d\n",look(&v,i));
	return 0;
}
