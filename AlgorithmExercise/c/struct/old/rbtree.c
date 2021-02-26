#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct node
{
	int red;//color 1=red 0=black;
	void* data;
	struct node* link[2];//left (0) right(1)
} node;

static int isRed(node* root)
{
	return root!=NULL&&root->red==1;
}
static node* single(node* root,int dir)
{
	node* save=root->link[!dir];

	root->link[!dir]=save->link[dir];
	save->link[dir]=root;

	root->red=1;
	save->red=0;

	return save;
}
static node* Double(node* root,int dir)
{
	root->link[!dir]=single(root->link[!dir],!dir);

	return single(root,dir);
}
static node* newNode(node* root,void* data)
{
	node* rn=(node*)malloc(sizeof(*rn));

	if(rn==NULL)
		return NULL;

	rn->red=1;
	rn->data=data;
	rn->link[0]=rn->link[1]=NULL;

	return rn;
}
void rbDelete(node* root)
{
	node* it=root;
	node* save;

	while(it!=NULL)
	{
		if(it->link[0]==NULL)
		{
			save=it->link[1];
			//tree->rel(it->data);
			free(it);
		}
		else
		{
			save=it->link[0];
			it->link[0]=save->link[1];
			save->link[1]=it;
		}
		it=save;
	}
	//free(tree);
}
void* rbFind(node* root,void* data)
{
	node* it=root;
	while(it!=NULL)
	{
		int cmp=strcmp(it->data,data);
		if(cmp==0)
			break;
		/*如果树支持重复，它们应该是
		 在右子树这工作*/
		it=it->link[cmp<0];
	}
	return it==NULL?NULL:it->data;
}
node* rbInsert(node* root,void* data)
{
	if(root==NULL)
	{
		root=newNode(root,data);
		if(root==NULL)
			return 0;
	}
	else
	{
		node head={0};
		node* g,*t;
		node* p,*q;
		int dir=0,last=0;

		t=&head;
		g=p=NULL;
		q=t->link[1]=root;

		for(;;)
		{
			if(q==NULL)
			{
				p->link[dir]=q=newNode(root,data);
				if(q==NULL)
					return 0;
			}
			else if(isRed(q->link[0])&&isRed(q->link[1]))
			{
				q->red=1;
				q->link[0]->red=0;
				q->link[1]->red=0;
			}
			if(isRed(q)&&isRed(p))
			{
				int dir2=t->link[1]==g;
				if(q==p->link[last])
					t->link[dir2]=single(g,!last);
				else
					t->link[dir2]=Double(g,!last);
			}
			if(strcmp(q->data,data)==0)
				break;
			last=dir;
			dir=strcmp(q->data,data)<0;
			if(g!=NULL)
				t=g;

			g=p,p=q;
			q=q->link[dir];
		}
		root=head.link[1];
	}
	root->red=0;
	//++tree->size;

	return root;
}
node* rbErase(node* root,void* data)
{
	if(root!=NULL)
	{
		node head={0};
		node* q,*p,*g;
		node* f=NULL;
		int dir=1;

		q=&head;
		g=p=NULL;
		q->link[1]=root;

		while(q->link[dir]!=NULL)
		{
			int last=dir;

			g=p,p=q;
			q=q->link[dir];
			dir=strcmp(q->data,data)<0;

			if(strcmp(q->data,data)==0)
				f=q;

			if(!isRed(q)&&!isRed(q->link[dir]))
			{
				if(isRed(q->link[!dir]))
					p=p->link[last]=single(q,dir);
				else if(!isRed(q->link[!dir]))
				{
					node* s=p->link[!last];

					if(s!=NULL)
					{
						if(!isRed(s->link[!last])&&!isRed(s->link[last]))
						{
							p->red=0;
							s->red=1;
							q->red=1;
						}
						else
						{
							int dir2=g->link[1]==p;

							if(isRed(s->link[last]))
								g->link[dir2]=Double(p,last);
							else if(isRed(s->link[!last]))
								g->link[dir2]=single(p,last);

							g->red=g->link[dir2]->red=1;
							g->link[dir2]->link[0]->red=0;
							g->link[dir2]->link[1]->red=0;
						}
					}
				}
			}
		}
		if(f!=NULL)
		{
			//tree->rel(f->data);
			f->data=q->data;
			p->link[p->link[1]==q]=q->link[q->link[0]==NULL];
			free(q);
		}
		root=head.link[1];
		if(root!=NULL)
			root->red=0;
		//--tree->size;
	}
	return root;
}
void rbPrint(node* root)
{
	int dir=0;
	if(root!=NULL)
	{
		rbPrint(root->link[dir]);
		printf("%s\n",(char*)root->data);
		rbPrint(root->link[!dir]);
	}
}
//---------------------------------------
int main()
{
	node* root=NULL;
	root=rbInsert(root,"hello");
	root=rbInsert(root,"word");
	root=rbInsert(root,"I");
	root=rbInsert(root,"am");
	root=rbInsert(root,"a");
	root=rbInsert(root,"techer");
	root=rbInsert(root,"com");
	root=rbInsert(root,"here");
	root=rbInsert(root,"string");
	root=rbInsert(root,"link");
	root=rbInsert(root,"title");
	root=rbInsert(root,"you");
	root=rbInsert(root,"www");
	root=rbInsert(root,"ming");
	root=rbInsert(root,"Ming");
	rbPrint(root);

	char* str=rbFind(root,"link");
	printf("找到：%s\n",str);
	root=rbErase(root,"link");
	root=rbErase(root,"string");
	root=rbErase(root,"com");
	rbPrint(root);
	
	rbDelete(root);
}
