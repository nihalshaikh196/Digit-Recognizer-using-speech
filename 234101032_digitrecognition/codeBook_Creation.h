// digitRecognition_224101059.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<conio.h>
#include "string.h"
#include "stdio.h"
#include<stdlib.h>
#include<cstdlib>
#include<math.h>
#include<windows.h>
#include<iostream>


#define framesize 320
#define sample_count 20
#define p 12
#define pii 22.0/7.0
#define size 30000
double tokhura_weight[]={0.0,1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0};
double arr[size][p+1];
double codebook[33][p+1];
int universe_size=0;
#define epsilon 0.03
#define deltaa 0.00001


void Update_codebook(int k)
{
	double temp[p+1];
	int temp_count=0;
	for(int i=1;i<=p;i++)
		temp[i]=0;
	for(int i=1;i<=k;i++)
	{
		temp_count=0;
		for(int j=1;j<=universe_size;j++)
		{
			
			if(arr[j][0]==i)
			{
				for(int k1=1;k1<=p;k1++)
				{
					
					temp[k1]+=arr[j][k1];
				}
				temp_count++;
			}
		}
		for(int j=1;j<=p;j++)
		{
			
			codebook[i][j]=temp[j]/temp_count;
			temp[j]=0;
		}
	}
}
double Tokhura_dis(int i,int j)
{
	double d=0;
	for(int k1=1;k1<=p;k1++)
	{
		d+=tokhura_weight[k1]*pow((arr[i][k1]-codebook[j][k1]),2);
	}
	return d;
}
//this function is used to calculate distortion as well as map the input_arr vectors to corresponding codebook vector and stores at 0'th position of arr
double Distortion(int k)
{
	double dis=0,min_dis=INT_MAX,total_dis=0;
	int index=0;
	for(int i=1;i<=universe_size;i++)
	{
		min_dis=INT_MAX;
		for(int j=1;j<=k;j++)
		{
			dis=Tokhura_dis(i,j);
			if(dis<min_dis)
			{
				min_dis=dis;
				index=j;
			}
		}
		arr[i][0]=index; 
		total_dis+=min_dis;
	}
	return total_dis/universe_size;
}
void populate_input_arr(FILE *fp)
{
	int i,j,result;
	double num;
	for(i=1;i<size;i++)
	{
		for(j=1;j<=p;j++)
		{
			result=fscanf(fp,"%Lf[^,]",&num);
			if(result==-1)
				return;
			else if(result==0)
			{
				result=fscanf(fp,"%*c");
				fscanf(fp,"%Lf[,]",&num);
				arr[i][j]=num;
			}	
			else
				arr[i][j]=num;
		}
		universe_size++;
	}
}
void initialize_codebook(int k)//create arbitary codebook initially
{
	int i,j;
	for(i=1;i<=k;i++)
	{
		for(j=1;j<=p;j++)
			codebook[i][j]=0;
	}
}

void k_means(int k)
{
	double distor1,distor2,diff;
	int step_count=0;//to count no of times k-means runs before it converges
	distor1=Distortion(k);//initial distortion
	diff=1;//assume initial difference is 1
	while(diff>deltaa)
	{
		printf("-");
		distor2=distor1;
		Update_codebook(k);
		distor1=Distortion(k);
		diff=abs(distor1-distor2);
		step_count++;
	}
	/*printf("Previous codebook distortion=%Lf\n",distor1);
	printf("Current codebook distortion=%Lf\n",distor2);
	printf("Differnce between distortions=%Lf\n",diff);
	printf("No of steps=%d\n",step_count);
	print_codebook(k);
	printf("\n");*/
}
void LBG()
{
	int d;
	double temp;
	for(int k=1;k<=32;k*=2)
	{

		k_means(k);//apply k-means on corrsponding k size
		if(k<32)//to avoid doubling codebook size to 32
		{
			for(int i=1;i<=p;i++)
			{
				d=k+1;//for doubling the codebook size e.g. if k=2 then d=3 starts i.e 1->3 2->4 
				for(int j=1;j<=k;j++)
				{
					temp=codebook[j][i];
					codebook[j][i]=temp*(1+epsilon);
					codebook[d][i]=temp*(1-epsilon);
					d++;
				}
			}
		}
	}
}
void print_infile_codebook()//to store final codebook in an ouput file named codebook
{
	FILE *fptr;
	fptr=fopen("codebook.txt","w");
	for(int i=1;i<=32;i++)
	{
		for(int j=1;j<=p;j++)
		{
			fprintf(fptr,"%Lf  ",codebook[i][j]);
			//printf("%Lf ",codebook[i][j]);
		}
		fprintf(fptr,"\n");
		//printf("\n");
	}
	fclose(fptr);
}

void create_codebook()
{
	FILE *fp;
	fp=fopen("universe.csv","r");
	if(fp==NULL)
		printf("Can't open file\n");
	int k=32;
	printf("\nCreating Codebook");
	populate_input_arr(fp);//stores universe vectors in 2-d array
	fclose(fp);
	initialize_codebook(k);
	LBG();
	print_infile_codebook();
	printf("\nCodebook created!\n");
}
