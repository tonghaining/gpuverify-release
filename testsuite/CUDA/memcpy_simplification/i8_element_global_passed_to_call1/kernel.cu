//pass
//--blockDim=256 --gridDim=128

struct s {
  char a;
};

s x;

__device__ void bar(s x);

__global__ void foo()
{
  bar(x);
}
