#include <stdio.h>
#include <stdlib.h>
//未完成！！！！！！！！！！！！！
typedef struct node
{
	struct node* right;
	struct node* left;
	struct node* parent;
	uintptr_t value;
} Node;

int tree32Insert(Node* root,uint32_t key,uint32_t mask,uintptr_t value)
{
	uint32_t bit=0x80000000;
	if(root==NULL)
	{
		root=(Node*)malloc(sizeof(Node));
		root->right=root->left=root->parent=NULL;
		root->value=//??????????
	}
	Node** node=&root;
	while(bit&mask)
	{
		if(key&bit)
			node=(&root)->right;//1向右
		else
			node=(&root)->left;	//0向左
		if(*node==NULL)//创建新结点
		{
			*node=(Node*)malloc(sizeof(Node));
			if(*node==NULL)
				return NULL;
			(*node)->right=*node->left=NULL;
			(*node)->value=//??怎样表示无值NO_VALUE;
			if(key&bit)
			{
				root->right=*node;
				(*node)->parent=root;
			}
			else
			{
				root->left=*node;
				(*node)->parent=root;
			}
		}
		bit>>=1;
		root=*node;
	}
	if(root->value!=NO_VALUE)
		return NGX_BUSY;
	root->value=value;
	return OK;
}
uintptr_t tree32Find(Node* root,uint32_t key)
{
	uint32_t bit=0x80000000;
	uintptr_t value=NO_VALUE;
	while(root)
	{
		if(root->value!=NO_VALUE)
			value=root->value;
		if(key&bit)
			root=root->right;
		else
			root=root->left;
		bit>>=1;
	}
	return value;
}
Node* tree32Delete(Node* root,uint32_t key,uint32_t mask)
{
	uint32_t bit=0x80000000;
	Node** node;
	while(root&&(bit&mask))//定位到要删除的结点
	{
		if(key&bit)
			node=(&root)->right;
		else
			node=(&root)->left;
		bit>>=1;
	}
	if(*node==NULL)//如果要删除的结点不存在
		return ERROR;
	if((*node)->right||(*node)->left)//如果有字节的存在，设置该结点为NO_VALUE
	{
		if((*node)->value!=NO_VALUE)
		{
			(*node)->value=NO_VALUE;
			return OK;//?
		}
		return ERROR;
	}
	for(;;)//如果没有字节点，则递归删除空的父结点，所以需要parent
	{
		if((*node)->parent->right=*node)
		{
			free((*node)->parent->right);
			(*node)->parent->right=NULL;
		}
		else
		{
			free((*node)->paret->left);
			(*node)->parent->left=NULL;
		}
		node=(*node)->parent;//指向父结点，返回上一层
		if((*node)->right||(*node)->left)
			break;
		if((*node)->value!=NO_VALUE)//??
			break;
		if((*node)->parent==NULL)
			break;
	}
	return OK;
}
void tree32Free(Node* root)
{
	if(root)
	{

	}
}
