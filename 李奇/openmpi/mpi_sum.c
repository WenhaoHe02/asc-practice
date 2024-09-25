#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>

int main(int argc,char** argv){
	MPI_Init(&argc,&argv);
	int rank,size;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	if(argc!=2){
		if(rank==0)
			printf("Usage: %s N \n",argv[0]);	
		MPI_Finalize();
		return 1;
	}
	int N=atoi(argv[1]);
	int local_N=N/size;
	int remainder=N%size;
	int start,end;
	if(rank<remainder){
		start=rank*(local_N+1)+1;
		end=start+local_N;
	}
	else{
		start=rank*local_N+remainder+1;
		end=start+local_N-1;
	}
	int local_sum=0;
	for(int i=start;i<=end;i++){
		local_sum+=i;
	}
	int total_sum=0;
	MPI_Reduce(&local_sum,&total_sum,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
	if(rank==0){
		printf("Total sum from 1 to %d is: %d\n",N,total_sum);
	}
	MPI_Finalize();
	return 0;
}

