#include<iostream>
#include<cstdio>
#include<mpi.h>
using namespace std;
int main(int argc,char** argv)
{
    int rank=0;
    int size=0;
    MPI_Init(&argc,&argv);
    MPI_Group firstGroup,secondGroup;
    MPI_Comm_size( MPI_COMM_WORLD , &size);
    MPI_Comm_rank( MPI_COMM_WORLD , &rank);
    MPI_Comm_group(MPI_COMM_WORLD, &firstGroup);
    int members[2]={0,1};
    //get current rank in a particular group
    MPI_Group_incl(firstGroup,2,members,&secondGroup);
    int currentRank=0;
    MPI_Group_rank(secondGroup,&currentRank);
        printf("In process %d, current rank: %d\n",rank,currentRank);
    printf("\n");
    MPI_Group_free(&firstGroup);
    MPI_Group_free(&secondGroup);
    MPI_Finalize();
    return 0;
}
