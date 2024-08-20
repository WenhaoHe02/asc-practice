#include <stdio.h>
#include <sys/time.h>
#include <array>

using namespace std;

double get_walltime() // s
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return (double)(tp.tv_sec * 1e3 + tp.tv_usec * 1e-3);
}

const int BLOCK_DIM = 1024;
constexpr int BLOCK_DIM_WARP = 32;

__global__ void softmax_one_warp_one_row(float *input, float *output, int M, int N)
{
    int row = blockIdx.x;
    float val = -__FLT_MAX__;

    for (int i = threadIdx.x; i < N; i += BLOCK_DIM_WARP)
    {
        val = max(val, input[row * N + i]);
    }
    __syncthreads();
    for (int offset = BLOCK_DIM_WARP / 2; offset > 0; offset /= 2)
    {
        val = max(val, __shfl_down_sync(0xffffffff, val, offset));
    }
    float globalMax = __shfl_sync(0xffffffff, val, 0);

    val = 0.0f;
    for (int i = threadIdx.x; i < N; i += BLOCK_DIM_WARP)
    {
        val += __expf(input[row * N + i] - globalMax);
    }
    __syncthreads();
    for (int offset = BLOCK_DIM_WARP / 2; offset > 0; offset /= 2)
    {
        val += __shfl_down_sync(0xffffffff, val, offset);
    }
    float globalSum = __shfl_sync(0xffffffff, val, 0);

    for (int i = threadIdx.x; i < N; i += BLOCK_DIM_WARP)
    {
        output[row * N + i] = __expf(input[row * N + i] - globalMax) / globalSum;
    }
}
__global__ void softmax_one_warp_one_row_float4(float *input, float *output, int M, int N)
{
    int row_id = blockIdx.x;
    int id_in_warp = threadIdx.x;
    int lane_id = id_in_warp % 32;

    float4 val4 = make_float4(-__FLT_MAX__, -__FLT_MAX__, -__FLT_MAX__, -__FLT_MAX__);
    for (int i = lane_id * 4; i < N; i += 32 * 4)
    {
        float4 data = *reinterpret_cast<float4*>(&input[row_id * N + i]);
        val4.x = max(val4.x, data.x);
        val4.y = max(val4.y, data.y);
        val4.z = max(val4.z, data.z);
        val4.w = max(val4.w, data.w);
    }

    float max_val = max(max(val4.x, val4.y), max(val4.z, val4.w));
    for (int offset = 16; offset > 0; offset /= 2)
    {
        max_val = max(max_val, __shfl_down_sync(0xffffffff, max_val, offset));
    }

    __shared__ float warpMax;
    if (lane_id == 0)
    {
        warpMax = max_val;
    }
    __syncthreads();

    float4 sum4 = make_float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = lane_id * 4; i < N; i += 32 * 4)
    {
        float4 data = *reinterpret_cast<float4*>(&input[row_id * N + i]);
        sum4.x += __expf(data.x - warpMax);
        sum4.y += __expf(data.y - warpMax);
        sum4.z += __expf(data.z - warpMax);
        sum4.w += __expf(data.w - warpMax);
    }

    float sum_val = sum4.x + sum4.y + sum4.z + sum4.w;
    for (int offset = 16; offset > 0; offset /= 2)
    {
        sum_val += __shfl_down_sync(0xffffffff, sum_val, offset);
    }

    __shared__ float warpSum;
    if (lane_id == 0)
    {
        warpSum = sum_val;
    }
    __syncthreads();

    for (int i = lane_id * 4; i < N; i += 32 * 4)
    {
        float4 data = *reinterpret_cast<float4*>(&input[row_id * N + i]);
        data.x = __expf(data.x - warpMax) / warpSum;
        data.y = __expf(data.y - warpMax) / warpSum;
        data.z = __expf(data.z - warpMax) / warpSum;
        data.w = __expf(data.w - warpMax) / warpSum;
        *reinterpret_cast<float4*>(&output[row_id * N + i]) = data;
    }
}

__global__ void softmax_one_warp_four_row(float *input, float *output, int M, int N)
{
    constexpr int size = 4;
    constexpr int group_size = BLOCK_DIM_WARP / size;
    int row_group_id = blockIdx.x;
    int warp_group_id = threadIdx.x / group_size;
    int id_in_warp = threadIdx.x % group_size;
    int gap = group_size;

    __shared__ float globalMax[size];
    __shared__ float globalSum[size];

    float val = -__FLT_MAX__;
    for (int i = id_in_warp; i < N; i += gap)
    {
        val = max(val, input[row_group_id * size * N + warp_group_id * N + i]);
    }
    for (int offset = gap / 2; offset > 0; offset /= 2)
    {
        val = max(val, __shfl_down_sync(0xff, val, offset, 8));
    }
    if (id_in_warp == 0)
    {
        globalMax[warp_group_id] = val;
    }
    __syncthreads();

    val = 0.0f;
    for (int i = id_in_warp; i < N; i += gap)
    {
        val += __expf(input[row_group_id * size * N + warp_group_id * N + i] - globalMax[warp_group_id]);
    }
    for (int offset = gap / 2; offset > 0; offset /= 2)
    {
        val += __shfl_down_sync(0xff, val, offset, 8);
    }
    if (id_in_warp == 0)
    {
        globalSum[warp_group_id] = val;
    }
    __syncthreads();

    for (int i = id_in_warp; i < N; i += gap)
    {
        output[row_group_id * 4 * N + warp_group_id * N + i] = __expf(input[row_group_id * 4 * N + warp_group_id * N + i] - globalMax[warp_group_id]) / globalSum[warp_group_id];
    }
}
__global__ void softmax_one_warp_four_row_float4(float *input, float *output, int M, int N)
{
    constexpr int size = 4;
    constexpr int group_size = BLOCK_DIM_WARP / size;
    int row_group_id = blockIdx.x;
    int warp_group_id = threadIdx.x / group_size;
    int id_in_warp = threadIdx.x % group_size;
    int gap = group_size;

    __shared__ float globalMax[size];
    __shared__ float globalSum[size];

    float4 val4 = make_float4(-__FLT_MAX__, -__FLT_MAX__, -__FLT_MAX__, -__FLT_MAX__);
    for (int i = id_in_warp * 4; i < N; i += gap * 4)
    {
        float4 data = *reinterpret_cast<float4 *>(&input[row_group_id * size * N + warp_group_id * N + i]);
        val4.x = max(val4.x, data.x);
        val4.y = max(val4.y, data.y);
        val4.z = max(val4.z, data.z);
        val4.w = max(val4.w, data.w);
    }
    float val = max(max(val4.x, val4.y), max(val4.z, val4.w));
    for (int offset = gap / 2; offset > 0; offset /= 2)
    {
        val = max(val, __shfl_down_sync(0xffffffff, val, offset, group_size));
    }
    if (id_in_warp == 0)
    {
        globalMax[warp_group_id] = val;
    }
    __syncthreads();

    float4 sum4 = make_float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = id_in_warp * 4; i < N; i += gap * 4)
    {
        float4 data = *reinterpret_cast<float4 *>(&input[row_group_id * size * N + warp_group_id * N + i]);
        sum4.x += __expf(data.x - globalMax[warp_group_id]);
        sum4.y += __expf(data.y - globalMax[warp_group_id]);
        sum4.z += __expf(data.z - globalMax[warp_group_id]);
        sum4.w += __expf(data.w - globalMax[warp_group_id]);
    }
    float sum = sum4.x + sum4.y + sum4.z + sum4.w;
    for (int offset = gap / 2; offset > 0; offset /= 2)
    {
        sum += __shfl_down_sync(0xffffffff, sum, offset, group_size);
    }
    if (id_in_warp == 0)
    {
        globalSum[warp_group_id] = sum;
    }
    __syncthreads();

    for (int i = id_in_warp * 4; i < N; i += gap * 4)
    {
        float4 data = *reinterpret_cast<float4 *>(&input[row_group_id * size * N + warp_group_id * N + i]);
        data.x = __expf(data.x - globalMax[warp_group_id]) / globalSum[warp_group_id];
        data.y = __expf(data.y - globalMax[warp_group_id]) / globalSum[warp_group_id];
        data.z = __expf(data.z - globalMax[warp_group_id]) / globalSum[warp_group_id];
        data.w = __expf(data.w - globalMax[warp_group_id]) / globalSum[warp_group_id];
        *reinterpret_cast<float4 *>(&output[row_group_id * size * N + warp_group_id * N + i]) = data;
    }
}

__global__ void softmax_one_warp_two_row_float4(float *input, float *output, int M, int N)
{
    constexpr int size = 2;
    constexpr int group_size = BLOCK_DIM_WARP / size;
    int row_group_id = blockIdx.x;
    int warp_group_id = threadIdx.x / group_size;
    int id_in_warp = threadIdx.x % group_size;
    int gap = group_size;

    __shared__ float globalMax[size];
    __shared__ float globalSum[size];

    float4 val4 = make_float4(-__FLT_MAX__, -__FLT_MAX__, -__FLT_MAX__, -__FLT_MAX__);
    for (int i = id_in_warp * 4; i < N; i += gap * 4)
    {
        float4 data = *reinterpret_cast<float4 *>(&input[row_group_id * size * N + warp_group_id * N + i]);
        val4.x = max(val4.x, data.x);
        val4.y = max(val4.y, data.y);
        val4.z = max(val4.z, data.z);
        val4.w = max(val4.w, data.w);
    }
    float val = max(max(val4.x, val4.y), max(val4.z, val4.w));
    for (int offset = gap / 2; offset > 0; offset /= 2)
    {
        val = max(val, __shfl_down_sync(0xffffffff, val, offset, group_size));
    }
    if (id_in_warp == 0)
    {
        globalMax[warp_group_id] = val;
    }
    __syncthreads();

    float4 sum4 = make_float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = id_in_warp * 4; i < N; i += gap * 4)
    {
        float4 data = *reinterpret_cast<float4 *>(&input[row_group_id * size * N + warp_group_id * N + i]);
        sum4.x += __expf(data.x - globalMax[warp_group_id]);
        sum4.y += __expf(data.y - globalMax[warp_group_id]);
        sum4.z += __expf(data.z - globalMax[warp_group_id]);
        sum4.w += __expf(data.w - globalMax[warp_group_id]);
    }
    float sum = sum4.x + sum4.y + sum4.z + sum4.w;
    for (int offset = gap / 2; offset > 0; offset /= 2)
    {
        sum += __shfl_down_sync(0xffffffff, sum, offset, group_size);
    }
    if (id_in_warp == 0)
    {
        globalSum[warp_group_id] = sum;
    }
    __syncthreads();

    for (int i = id_in_warp * 4; i < N; i += gap * 4)
    {
        float4 data = *reinterpret_cast<float4 *>(&input[row_group_id * size * N + warp_group_id * N + i]);
        data.x = __expf(data.x - globalMax[warp_group_id]) / globalSum[warp_group_id];
        data.y = __expf(data.y - globalMax[warp_group_id]) / globalSum[warp_group_id];
        data.z = __expf(data.z - globalMax[warp_group_id]) / globalSum[warp_group_id];
        data.w = __expf(data.w - globalMax[warp_group_id]) / globalSum[warp_group_id];
        *reinterpret_cast<float4 *>(&output[row_group_id * size * N + warp_group_id * N + i]) = data;
    }
}
__global__ void softmax_one_warp_two_row(float *input, float *output, int M, int N)
{
    constexpr int size = 2; // 每个warp处理的行数
    constexpr int group_size = BLOCK_DIM_WARP / size; // 每个warp内的线程数
    int row_group_id = blockIdx.x; // 获取当前处理的行组索引
    int warp_group_id = threadIdx.x / group_size; // 获取线程在warp组内的索引
    int id_in_warp = threadIdx.x % group_size; // 获取线程在组内的位置
    int gap = group_size; // 每个线程处理的步长

    __shared__ float globalMax[size];
    __shared__ float globalSum[size];

    float val = -__FLT_MAX__; 

    for (int i = id_in_warp; i < N; i += gap)
    {
        val = max(val, input[row_group_id * size * N + warp_group_id * N + i]);
    }

    for (int offset = gap / 2; offset > 0; offset /= 2)
    {
        val = max(val, __shfl_down_sync(0xffff, val, offset, group_size));
    }

    if (id_in_warp == 0)
    {
        globalMax[warp_group_id] = val;
    }
    __syncthreads(); 

    val = 0.0f; 

    for (int i = id_in_warp; i < N; i += gap)
    {
        val += __expf(input[row_group_id * size * N + warp_group_id * N + i] - globalMax[warp_group_id]);
    }

    for (int offset = gap / 2; offset > 0; offset /= 2)
    {
        val += __shfl_down_sync(0xffff, val, offset, group_size);
    }

    if (id_in_warp == 0)
    {
        globalSum[warp_group_id] = val;
    }
    __syncthreads();

    for (int i = id_in_warp; i < N; i += gap)
    {
        output[row_group_id * size * N + warp_group_id * N + i] = __expf(input[row_group_id * size * N + warp_group_id * N + i] - globalMax[warp_group_id]) / globalSum[warp_group_id];
    }
}

__global__ void softmax(float *input, float *output, int M, int N)
{
    int row = blockIdx.x;
    __shared__ float tmp[BLOCK_DIM];
    __shared__ float globalMax;
    __shared__ float globalSum;

    float val = -__FLT_MAX__;
    for (int i = threadIdx.x; i < N; i += BLOCK_DIM)
    {
        val = max(val, input[row * N + i]);
    }
    tmp[threadIdx.x] = val;
    __syncthreads();

    for (int step = BLOCK_DIM / 2; step > 0; step /= 2)
    {
        if (threadIdx.x < step)
        {
            tmp[threadIdx.x] = max(tmp[threadIdx.x], tmp[threadIdx.x + step]);
        }
        __syncthreads();
    }

    if (threadIdx.x == 0)
    {
        globalMax = tmp[0];
    }
    __syncthreads();

    val = 0.0f;
    for (int i = threadIdx.x; i < N; i += BLOCK_DIM)
    {
        val += __expf(input[row * N + i] - globalMax);
    }
    tmp[threadIdx.x] = val;
    __syncthreads();

    for (int step = BLOCK_DIM / 2; step > 0; step /= 2)
    {
        if (threadIdx.x < step)
        {
            tmp[threadIdx.x] += tmp[threadIdx.x + step];
        }
        __syncthreads();
    }

    if (threadIdx.x == 0)
    {
        globalSum = tmp[0];
    }
    __syncthreads();

    for (int i = threadIdx.x; i < N; i += BLOCK_DIM)
    {
        output[row * N + i] = __expf(input[row * N + i] - globalMax) * __fdividef(1.0F, globalSum);
    }
}

void cpu_softmax(float *cpu_input, float *cpu_output, int M, int N, float *input, float *output)
{
    double tot_ker_time = 0.0;
    double tot_time = 0.0;
    constexpr int times = 1001;

    int num_block = M;
    dim3 block_dim(BLOCK_DIM, 1, 1);
    dim3 grid_dim(num_block, 1, 1);
    for (int i = 0; i < times; i++)
    {
        cudaEvent_t start, stop;
        cudaEventCreate(&start);
        cudaEventCreate(&stop);

        cudaDeviceSynchronize();
        double stt_tot_time = get_walltime();

        cudaEventRecord(start, 0);

        softmax<<<grid_dim, block_dim>>>(input, output, M, N);

        cudaEventRecord(stop, 0);
        cudaEventSynchronize(stop);

        float ker_time = 0.0f;
        cudaEventElapsedTime(&ker_time, start, stop);
        double end_tot_time = get_walltime();

        if (i > 0)
        {
            tot_ker_time += ker_time;
            tot_time += (end_tot_time - stt_tot_time);
        }

        cudaEventDestroy(start);
        cudaEventDestroy(stop);
    }

    cudaMemcpy(cpu_output, output, M * N * sizeof(float), cudaMemcpyDeviceToHost);
    double avg_ker_time = tot_ker_time / (times - 1);
    double avg_tot_time = tot_time / (times - 1);
    printf("baseline: \n");
    printf("M = %d, N = %d \n", M, N);
    printf("average kernel time: %.4f ms, average total time: %.4f ms\n", avg_ker_time, avg_tot_time);
}

void cpu_softmax_one_warp_one_row(float *cpu_input, float *cpu_output, int M, int N, float *input, float *output)
{
    double tot_ker_time = 0.0;
    double tot_time = 0.0;
    constexpr int times = 1001;

    int num_block = M;
    dim3 block_dim(BLOCK_DIM_WARP, 1, 1);
    dim3 grid_dim(num_block, 1, 1);
    for (int i = 0; i < times; i++)
    {
        cudaEvent_t start, stop;
        cudaEventCreate(&start);
        cudaEventCreate(&stop);

        cudaDeviceSynchronize();
        double stt_tot_time = get_walltime();

        cudaEventRecord(start, 0);

        softmax_one_warp_one_row<<<grid_dim, block_dim>>>(input, output, M, N);

        cudaEventRecord(stop, 0);
        cudaEventSynchronize(stop);

        float ker_time = 0.0f;
        cudaEventElapsedTime(&ker_time, start, stop);
        double end_tot_time = get_walltime();

        if (i > 0)
        {
            tot_ker_time += ker_time;
            tot_time += (end_tot_time - stt_tot_time);
        }

        cudaEventDestroy(start);
        cudaEventDestroy(stop);
    }

    cudaMemcpy(cpu_output, output, M * N * sizeof(float), cudaMemcpyDeviceToHost);
    double avg_ker_time = tot_ker_time / (times - 1);
    double avg_tot_time = tot_time / (times - 1);
    printf("one warp one row: \n");
    printf("M = %d, N = %d \n", M, N);
    printf("average kernel time: %.4f ms, average total time: %.4f ms\n", avg_ker_time, avg_tot_time);
}
void cpu_softmax_one_warp_one_row_float4(float *cpu_input, float *cpu_output, int M, int N, float *input, float *output)
{
    double tot_ker_time = 0.0;
    double tot_time = 0.0;
    constexpr int times = 1001;

    int num_block = M;
    dim3 block_dim(BLOCK_DIM_WARP, 1, 1);
    dim3 grid_dim(num_block, 1, 1);
    for (int i = 0; i < times; i++)
    {
        cudaEvent_t start, stop;
        cudaEventCreate(&start);
        cudaEventCreate(&stop);

        cudaDeviceSynchronize();
        double stt_tot_time = get_walltime();

        cudaEventRecord(start, 0);

        softmax_one_warp_one_row_float4<<<grid_dim, block_dim>>>(input, output, M, N);

        cudaEventRecord(stop, 0);
        cudaEventSynchronize(stop);

        float ker_time = 0.0f;
        cudaEventElapsedTime(&ker_time, start, stop);
        double end_tot_time = get_walltime();

        if (i > 0)
        {
            tot_ker_time += ker_time;
            tot_time += (end_tot_time - stt_tot_time);
        }

        cudaEventDestroy(start);
        cudaEventDestroy(stop);
    }

    cudaMemcpy(cpu_output, output, M * N * sizeof(float), cudaMemcpyDeviceToHost);
    double avg_ker_time = tot_ker_time / (times - 1);
    double avg_tot_time = tot_time / (times - 1);
    printf("one warp one row float4 version: \n");
    printf("M = %d, N = %d \n", M, N);
    printf("average kernel time: %.4f ms, average total time: %.4f ms\n", avg_ker_time, avg_tot_time);
}
void cpu_softmax_one_warp_two_row(float *cpu_input, float *cpu_output, int M, int N, float *input, float *output)
{
    double tot_ker_time = 0.0;
    double total_time = 0.0;
    constexpr int times = 1001;

    int size = 2;
    int num_block = M / size;
    dim3 block_dim(BLOCK_DIM_WARP, 1, 1);
    dim3 grid_dim(num_block, 1, 1);
    for (int i = 0; i < times; i++)
    {
        cudaEvent_t start, stop;
        cudaEventCreate(&start);
        cudaEventCreate(&stop);

        cudaDeviceSynchronize();
        double start_total = get_walltime();

        cudaEventRecord(start, 0);

        softmax_one_warp_two_row<<<grid_dim, block_dim>>>(input, output, M, N);

        cudaEventRecord(stop, 0);
        cudaEventSynchronize(stop);

        float ker_time = 0.0f;
        cudaEventElapsedTime(&ker_time, start, stop);
        double end_total = get_walltime();

        if (i > 0)
        {
            tot_ker_time += ker_time;
            total_time += (end_total - start_total);
        }

        cudaEventDestroy(start);
        cudaEventDestroy(stop);
    }

    cudaMemcpy(cpu_output, output, M * N * sizeof(float), cudaMemcpyDeviceToHost);
    double avg_ker_time = tot_ker_time / (times - 1);
    double avg_tot_time = total_time / (times - 1);
    printf("one warp two rows: \n");
    printf("M = %d, N = %d \n", M, N);
    printf("average kernel time: %.4f ms, average total time: %.4f s\n", avg_ker_time, avg_tot_time);
}
void cpu_softmax_one_warp_two_row_float4(float *cpu_input, float *cpu_output, int M, int N, float *input, float *output)
{
    double tot_ker_time = 0.0;
    double total_time = 0.0;
    constexpr int times = 1001;

    int size = 2;
    int num_block = M / size;
    dim3 block_dim(BLOCK_DIM_WARP, 1, 1);
    dim3 grid_dim(num_block, 1, 1);
    for (int i = 0; i < times; i++)
    {
        cudaEvent_t start, stop;
        cudaEventCreate(&start);
        cudaEventCreate(&stop);

        cudaDeviceSynchronize();
        double start_total = get_walltime();

        cudaEventRecord(start, 0);

        softmax_one_warp_two_row_float4<<<grid_dim, block_dim>>>(input, output, M, N);

        cudaEventRecord(stop, 0);
        cudaEventSynchronize(stop);

        float ker_time = 0.0f;
        cudaEventElapsedTime(&ker_time, start, stop);
        double end_total = get_walltime();

        if (i > 0)
        {
            tot_ker_time += ker_time;
            total_time += (end_total - start_total);
        }

        cudaEventDestroy(start);
        cudaEventDestroy(stop);
    }

    cudaMemcpy(cpu_output, output, M * N * sizeof(float), cudaMemcpyDeviceToHost);
    double avg_ker_time = tot_ker_time / (times - 1);
    double avg_tot_time = total_time / (times - 1);
    printf("one warp two rows float4 version: \n");
    printf("M = %d, N = %d \n", M, N);
    printf("average kernel time: %.4f ms, average total time: %.4f s\n", avg_ker_time, avg_tot_time);
}

void cpu_softmax_one_warp_four_row(float *cpu_input, float *cpu_output, int M, int N, float *input, float *output)
{
    double tot_ker_time = 0.0;
    double tot_time = 0.0;
    constexpr int times = 1001;

    int size = 4;
    int num_block = M / size;
    dim3 block_dim(BLOCK_DIM_WARP, 1, 1);
    dim3 grid_dim(num_block, 1, 1);
    for (int i = 0; i < times; i++)
    {
        cudaEvent_t start, stop;
        cudaEventCreate(&start);
        cudaEventCreate(&stop);

        cudaDeviceSynchronize();
        double stt_tot_time = get_walltime();

        cudaEventRecord(start, 0);

        softmax_one_warp_four_row<<<grid_dim, block_dim>>>(input, output, M, N);

        cudaEventRecord(stop, 0);
        cudaEventSynchronize(stop);

        float ker_time = 0.0f;
        cudaEventElapsedTime(&ker_time, start, stop);
        double end_tot_time = get_walltime();

        if (i > 0)
        {
            tot_ker_time += ker_time;
            tot_time += (end_tot_time - stt_tot_time);
        }

        cudaEventDestroy(start);
        cudaEventDestroy(stop);
    }

    cudaMemcpy(cpu_output, output, M * N * sizeof(float), cudaMemcpyDeviceToHost);
    double avg_ker_time = tot_ker_time / (times - 1);
    double avg_tot_time = tot_time / (times - 1);
    printf("one warp four row: \n");
    printf("M = %d, N = %d \n", M, N);
    printf("average kernel time: %.4f ms, average total time: %.4f ms\n", avg_ker_time, avg_tot_time);
}
void cpu_softmax_one_warp_four_row_float4(float *cpu_input, float *cpu_output, int M, int N, float *input, float *output)
{
    double tot_ker_time = 0.0;
    double tot_time = 0.0;
    constexpr int times = 1001;

    int size = 4;
    int num_block = M / size;
    dim3 block_dim(BLOCK_DIM_WARP, 1, 1);
    dim3 grid_dim(num_block, 1, 1);
    for (int i = 0; i < times; i++)
    {
        cudaEvent_t start, stop;
        cudaEventCreate(&start);
        cudaEventCreate(&stop);

        cudaDeviceSynchronize();
        double stt_tot_time = get_walltime();

        cudaEventRecord(start, 0);

        softmax_one_warp_four_row_float4<<<grid_dim, block_dim>>>(input, output, M, N);

        cudaEventRecord(stop, 0);
        cudaEventSynchronize(stop);

        float ker_time = 0.0f;
        cudaEventElapsedTime(&ker_time, start, stop);
        double end_tot_time = get_walltime();

        if (i > 0)
        {
            tot_ker_time += ker_time;
            tot_time += (end_tot_time - stt_tot_time);
        }

        cudaEventDestroy(start);
        cudaEventDestroy(stop);
    }

    cudaMemcpy(cpu_output, output, M * N * sizeof(float), cudaMemcpyDeviceToHost);
    double avg_ker_time = tot_ker_time / (times - 1);
    double avg_tot_time = tot_time / (times - 1);
    printf("one warp four row float4 version: \n");
    printf("M = %d, N = %d \n", M, N);
    printf("average kernel time: %.4f ms, average total time: %.4f ms\n", avg_ker_time, avg_tot_time);
}

int main()
{
    double stt_tot_time = get_walltime();
    float *cpu_input, *cpu_output;
    float *input, *output;
    int M = 1024;
    int N = 1024;
    constexpr array<int, 3> Ms = {32, 1024, 2048};
    constexpr array<int, 3> Ns = {32, 1024, 2048};
    for (auto m : Ms)
    {
        for (auto n : Ns)
        {
            M = m;
            N = n;
            cpu_input = (float *)malloc(M * N * sizeof(float));
            cpu_output = (float *)malloc(M * N * sizeof(float));
            cudaMalloc((void **)&input, M * N * sizeof(float));
            cudaMalloc((void **)&output, M * N * sizeof(float));
            for (int i = 0; i < M * N; i++)
            {
                cpu_input[i] = i % 10;
            }
            cudaMemcpy(input, cpu_input, M * N * sizeof(float), cudaMemcpyHostToDevice);
            cpu_softmax(cpu_input, cpu_output, M, N, input, output);
            for (int i = 0; i < 10; i++)
            {
                printf("%.4e ", cpu_output[i]);
            }
            printf("\n\n");
            cpu_softmax_one_warp_one_row(cpu_input, cpu_output, M, N, input, output);
            for (int i = 0; i < 10; i++)
            {
                printf("%.4e ", cpu_output[i]);
            }
            printf("\n\n");
            cpu_softmax_one_warp_one_row_float4(cpu_input, cpu_output, M, N, input, output);
            for (int i = 0; i < 10; i++)
            {
                printf("%.4e ", cpu_output[i]);
            }
            printf("\n\n");
            cpu_softmax_one_warp_two_row(cpu_input, cpu_output, M, N, input, output);
            for (int i = 0; i < 10; i++)
            {
                printf("%.4e ", cpu_output[i]);
            }
            printf("\n\n");
            cpu_softmax_one_warp_two_row_float4(cpu_input, cpu_output, M, N, input, output);
            for (int i = 0; i < 10; i++)
            {
                printf("%.4e ", cpu_output[i]);
            }
            printf("\n\n");
            cpu_softmax_one_warp_four_row(cpu_input, cpu_output, M, N, input, output);
            for (int i = 0; i < 10; i++)
            {
                printf("%.4e ", cpu_output[i]);
            }
            printf("\n\n");
            cpu_softmax_one_warp_four_row_float4(cpu_input, cpu_output, M, N, input, output);
            for (int i = 0; i < 10; i++)
            {
                printf("%.4e ", cpu_output[i]);
            }
            printf("\n\n\n\n");
            cudaFree(input);
            cudaFree(output);
            free(cpu_input);
            free(cpu_output);
        }
    }
    double end_tot_time = get_walltime();
    double tot_time = end_tot_time - stt_tot_time;
    printf("\n\nthe total time of the whole program is: %.4f ms", tot_time);
    return 0;
}
