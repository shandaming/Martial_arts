#include <stdo.h>

typedef struct result
{
	int left;
	int right;
	int sum;
} res;

res* findMaxCrossingSubarray(int* A,int left,int mid,int right)
{
	res* R=malloc(sizeof(struct result));
	int leftSum=*(A+mid);
	int sum=0;
	int maxLeftIndex=mid;
	for(int i=mid;i>=left;--i)
	{
		sum+=*(A+i);
		if(sum>leftSum)
		{
			leftSum=sum;
			maxLeftIndex=i;
		}
	}
	int rightSum=*(A+mid+1);
	sum=0;
	int maxRightIndex=mid+1;
	for(int j=mid+1;j<=right;++j)
	{
		sum+=*(A+j);
		if(sum>rightSum)
		{
			rightSum=sum;
			maxRightIndex=j;
		}
	}
	R->left=maxLeftIndex;
	R->right=maxRightIndex;
	R->sum=(leftSum+rightSum);

	return R;
}

res* findMaxSubarray(int* A,int low,int high)
{
	if(high==low)
	{
		res* re=malloc(sizeof(struct result));
		re->left=low;
		re->right=high;
		re->sum=*(A+low);
		return re;
	}
	else
	{
		int mid=(low+mid)/2;
		res* resLeft=findMaxSubarray(A,low,mid);
		res* resRight=findMaxSubarray(A,mid+1,high);
		res* resCross=findMaxCrossingSubarray(A,low,mid,high);
		if((resLeft->sum)>=(resRight->sum)&&(resLeft->sum)>=(resCross->sum))
			return resLeft;
		else if(resRight->sum>=resLeft->sum && resRight->sum>=resCross->sum)
			return resRight;
		else
			return resCross;
	}
}
