#include<stdio.h>
#include<stdlib.h>

typedef struct {
	int x;
	int y;
} point;

bool point_com(point *p1, point *p2){
	if(p1->x<p2->x) return 1;
	if(p1->x>p2->x) return 0;
	if(p1->y<p2->y) return 1;
	return 0;
}
bool point_eq_com(point *p1, point *p2){
	if(p1->x==p2->x && p1->y==p2->y) return 1;
	return 0;
}

typedef struct Node{
	point *p;	
	bool col;
	struct Node *left, *right,*parent;
} Node;

Node *root=NULL;

void leftrotate(Node **x,Node **r){
	Node *y =(*x)->right;
	(*x)->right=y->left;
	if(y->left !=NULL){
		y->left->parent=*x;
	}
	y->parent=(*x)->parent;
	if(x->parent==NULL){
		(*r)=y;
	}
	else if((*x)==(*x)->parent->left){
		(*x)->parent->left=y;
	}
	else {
		(*x)->parent->right=y;
	}
	y->left=x;
	(*x)->parent=y;
}

void rightrotate(Node **x,Node **r){
	Node *y =(*x)->left;
	(*x)->left=y->right;
	if(y->right !=NULL){
		y->right->parent=(*x);
	}
	y->parent=(*x)->parent;
	if((*x)->parent==NULL){
		(*r)=y;
	}
	else if((*x)==(*x)->parent->right){
		(*x)->parent->right=y;
	}
	else {
		(*x)->parent->left=y;
	}
	y->right=(*x);
	(*x)->parent=y;
}

void normal_insert(point *p,Node **n){
	if(*n==NULL){
		Node* nn=((Node *)malloc(sizeof(Node)));
		nn->p=p;
		nn->col=0;
		nn->left=NULL;
		nn->right=NULL;
		nn->parent=*n;
		*n=nn;
		return;
	}
	if(point_com((*n)->p,p)){
		insert(p,&((*n)->left));
	}
	else{
		insert(p,&((*n)->right));
	}
}

void best_insert(Node **k,Node **r){
	while(*k!=*r &&  (*k)->parent->col==1){
		if((*k)->parent==(*k)->parent->parent->left){
			Node** u=&((*k)->parent->parent->right);
			if(u->col==1){
				(*k)->parent->col=0;
				(*u)->col=0;
				(*k)->parent->parent->col=1;
				*k=(*k)->parent->parent;
			}
			else{
				if(*k==(*k)->parent->right){
					*k=(*k)->parent;
					leftrotate(k,r);
				}
				(*k)->parent->col=0;
				(*k)->parent->parent->col=1;
				rightrotate(&((*k)->parent->parent),r);
			}
		}
		else {
			Node** u=&((*k)->parent->parent->left);
			if((*u)->col==1){
				(*k)->parent->col = 0;
				(*u)->col = 0;
				(*k)->parent->parent->col = 1;
				(*k) = (*k)->parent->parent;	
			}
			else{
				if ((*k) == (*k)->parent->left) {
					(*k) = (*k)->parent;
					rightRotate(k,r);
				}
				(*k)->parent->col = 0;
				(*k)->parent->parent->col = 1;
				leftrotate(&((*k)->parent->parent),r); 
			} 
		} 
	}
	(*r)->col=0;
}

void inorder(Node *n){
	if(n==NULL) return;
	printf("x=%d,y=%d\n",n->p->x,n->p->y);
	inorder(n->left);
	inorder(n->right);
}

Node * lookup(point *p,Node *n){
	if(point_eq_com(n->p,p)) return n;
	if(point_com(n->p,p)){
		if(n->left!=NULL)
			lookup(p,n->left);
		else return NULL;
	}
	else{
		if(n->right!=NULL)
			lookup(p,n->right);
		else return NULL;
	}
}
void insert(point * p, )
{
	Node* new_node = new Node(data);
	new_node->left = NIL;
	new_node->right = NIL;

	Node* parent = nullptr;
	Node* current = root;

	// BST insert
	while (current != NIL) {
	    parent = current;
	    if (new_node->data < current->data) {
		current = current->left;
	    }
	    else {
		current = current->right;
	    }
	}

	new_node->parent = parent;

	if (parent == nullptr) {
	    root = new_node;
	}
	else if (new_node->data < parent->data) {
	    parent->left = new_node;
	}
	else {
	    parent->right = new_node;
	}

	if (new_node->parent == nullptr) {
	    new_node->color = "BLACK";
	    return;
	}

	if (new_node->parent->parent == nullptr) {
	    return;
	}

	fixInsert(new_node);
}


int main(){
	point *p=(point *)malloc(sizeof(point));
	p->x=3,p->y=1;
	insert(p,&root);
	printf("insert 1\n");
	p=(point *)malloc(sizeof(point));
	p->x=1,p->y=1;
	insert(p,&root);
	printf("insert 2\n");
	p=(point *)malloc(sizeof(point));
	p->x=2,p->y=1;
	insert(p,&root);
	printf("insert 3\n");
	p=(point *)malloc(sizeof(point));
	p->x=4,p->y=1;
	insert(p,&root);
	printf("insert 4\n");
	p=(point *)malloc(sizeof(point));
	p->x=1,p->y=2;
	insert(p,&root);
	printf("insert 5\n");
	inorder(root);
}
