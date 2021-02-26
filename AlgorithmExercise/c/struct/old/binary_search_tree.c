#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef struct node
{
	void* data;
	struct node* left;
	struct node* right;
} node;

//cmp(void*,void*)顺序一致,返回根结点root
node* tree_insert(node* root, void* data, int (*cmp)(void*, void*))
{
	if(!root)
	{
		root = malloc(sizeof(node));
		assert(root);
		root->data = data;
		root->left = root->right = NULL;
	}
	else
	{
		int res = cmp(data, root->data);
		if(res < 0)
			root->left = tree_insert(root->left, data, cmp);
		else if(res > 0)
			root->right = tree_insert(root->right, data, cmp);
	}

	return root;
}

//成功返回查找的结点,失败返回NULL
node* tree_search(node* root, void* data, int (*cmp)(void*, void*))
{
	if(!root)
		return NULL;

	int res = comp(data, root->data);
	if(res < 0)
		tree_search(root->left, data, cmp);
	else if(res > 0)
		tree_search(root->right, data, cmp);
	else
		return root;
}

//不太明白-----成功返回根结点,失败返回NULL
node* tree_delete(node* root, void* data, int (*cmp)(void*, void*))
{
	if(!root)
		return NULL;

	int res = cmp(data, root->data);
	node* temp;
	if(res < 0)
		root->left = tree_delete(root->left, data, cmp);//去掉root->left=就变成root指向当前删除的结点,不去掉就是返回根结点
	else if(res > 0)
		root->right = tree_delete(root->right, data, cmp);//去掉root->right=就变成root指向当前删除的结点,不去掉就是返回根结点
	else
	{
		if(!root->left)
		{
			temp = root->right;
			free(root);
			root = temp;
		}
		else if(!root->right)
		{
			temp = root->left;
			free(root);
			root = temp;
		}
		else
		{
			temp = root->right;
			node* parent = NULL;
			while(temp->left != NULL)
			{
				parent = temp;
				temp = temp->left;
			}
			root->data = temp->data;
			if(parent != NULL)
				parent->left = tree_delete(parent->left, parent->left->data,cmp);
			else
				root->right = tree_delete(root->right, root->right->data, cmp);
		}
		return root;
	}	
}

//安顺序打印
void tree_print(node* root)
{
	if(root)
	{
		tree_print(root->left);
		printf("%s\n", (char*)root->data);
		tree_print(root->right);
	}
}
void tree_free(node* root)
{
	if(root)
	{
		tree_free(root->left);
		tree_free(root->right);
		free(root);
		root = NULL;
	}
}

int main()
{
	node* root=NULL;
	char* arr[]={"now","is","the","for","men","of","time","all","good","party","their","to","aid","come"};
	for(int i=0;i<14;++i)
		root=treeInsert(root,(void*)arr[i],(int (*)(void*,void*))strcmp);
	treePrint(root);
	assert(root);
	char* data="the";
	node* temp=treeSearch(root,(void*)data,(int (*)(void*,void*))strcmp);
	if(temp)
		printf("搜索到的值：%s\n",(char*)temp->data);
	temp=treeDelete(root,(void*)data,(int (*)(void*,void*))strcmp);
	if(temp)
		printf("删除：%s\n",(char*)temp->data);
	treePrint(root);
	treeFree(root);
	return 0;
}
