#include<iostream>
#include<mpi.h>
using namespace std;
int main(int argc,char** argv)
{
    int size=0;
    int rank=0;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Group first,second,third;
    MPI_Comm_group(MPI_COMM_WORLD,&first);
    int seq1[]={2,3};
    int seq2[]={0,1};
    MPI_Group_excl( first , 2, seq1 ,&second);
    MPI_Group_incl( first , 2, seq2 ,&third);
    int curRank=-1;
    int tmp[]={1,0};
    int ans[5]={0,0,0,0,0};
    MPI_Group_translate_ranks( third , 2 ,tmp , second , ans);
    MPI_Barrier(MPI_COMM_WORLD);
    if(rank==0)
    {
        for(int i=0;i<5;i++) printf("%d ",ans[i]);
        printf("\n");
        int siz1e=0;
        MPI_Group_size(first,&siz1e);

        printf("size of first is%d ",siz1e);
    }
    
    MPI_Group_free( &first);
    MPI_Group_free( &second);
    MPI_Group_free(&third);
    MPI_Finalize();
    return 0;
}
