#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct node
{
	int data;
	struct node* left;
	struct node* right;
	int height;
} node;
static int height(node* n)
{
	if(n==NULL)
		return -1;
	else
		return n->height;
}
static int max(int l,int r)
{
	return l>r?l:r;
}
static node* singleRotateWithLeft(node* k)
{
	node* t=k->left;
	k->left=t->right;
	t->right=k;
	k->height=max(height(k->left),height(k->right))+1;
	t->height=max(height(t->left),k->height)+1;
}
void AVLFree(node* root)
{
	if(root!=NULL)
	{
		AVLFree(root->left);
		AVLFree(root->right);
		free(root);
	}
}
node* AVLFind(node* root,int e)
{
	if(root==NULL)
		return NULL;
	if(e<root->data)
		return AVLFind(root->left,e);
	else if(e>root->data)
		return AVLFind(root->right,e);
	return root;
}
static node* singleRotateWithRight(node* k1)
{
	node* k2=k1->right;
	k1->right=k2->left;
	k2->left=k1;
	k1->height=max(height(k1->left),height(k1->right))+1;
	k2->height=max(height(k1->right),k1->height)+1;
}
static node* doubleRotateWithLeft(node* k3)
{
	k3->left=singleRotateWithRight(k3->left);
	return singleRotateWithLeft(k3);
}
static node* doubleRotateWithRight(node* k1)
{
	k1->right=singleRotateWithLeft(k1->right);
	return singleRotateWithRight(k1);
}
node* insert(node* t,int e)
{
	if(t==NULL)
	{
		t=(node*)malloc(sizeof(node));
		if(t==NULL)
			return NULL;
		t->data=e;
		t->height=0;
		t->left=t->right=NULL;
	}
	else if(e<t->data)
	{
		t->left=insert(t->left,e);
		if(height(t->left)-height(t->right)==2)
		{
			if(e<t->left->data)
				t=singleRotateWithLeft(t);
			else
				t=doubleRotateWithLeft(t);
		}
	}
	else if(e>t->data)
	{
		t->right=insert(t->right,e);
		if(height(t->right)-height(t->left)==2)
		{
			if(e>t->right->data)
				t=singleRotateWithRight(t);
			else 
				t=doubleRotateWithRight(t);
		}
	}
	t->height=max(height(t->left),height(t->right))+1;
	return t;
}
//删除没有实现！！！
void AVLDelete(node* root,int data)
{
	if(root==NULL)
		return;
	if(data<root->data)
	{
		//delete(root->left,data);
		if(height(root->right)-height(root->left)==2)
		{
			node* r=root->right;
			//if(height(r->left)>height(r->right))
				//root=
		}
	}
	//else if(x>root->data)
		//delete(root-.right,data);

}
void AVLPrint(node* root)
{
	if(!root)
	{
		AVLPrint(root->left);
		printf("%d\n",root->data);
		AVLPrint(root->right);
	}
}
//-------------------有问题？？？
int main()
{
	int arr[]={6,3,7,8,3,0,22,43};
	node* root=NULL;
	for(int i=0;i!=8;++i)
		root=insert(root,arr[i]);
	AVLPrint(root);
	AVLFree(root);

	return 0;
}
