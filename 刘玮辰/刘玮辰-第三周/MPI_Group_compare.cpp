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
    int result=0;
    if(rank==0)
    {
    
    MPI_Group_compare(first,second, &result);
    printf("first and second: %d ",result);
    if(result==MPI_IDENT) printf("IDENT\n");
    if(result==MPI_SIMILAR) printf("SIMILAR\n");
    if(result==MPI_UNEQUAL) printf("UNEQUAL\n");
    MPI_Group_compare( second , third , &result);
    printf("second and third: %d  ",result);
    if(result==MPI_IDENT) printf("IDENT\n");
    if(result==MPI_SIMILAR) printf("SIMILAR\n");
    if(result==MPI_UNEQUAL) printf("UNEQUAL\n");
    }
    MPI_Group_free( &first);
    MPI_Group_free( &second);
    MPI_Group_free(&third);
    MPI_Finalize();
    return 0;
}
