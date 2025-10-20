__kernel void computeNextState(
    __global float *positions,
    __global float *prev_positions,
    __global float *velocities,
    __global int *is,
    __global int *js,
    __global float *rest_lengths
) {

   int id = get_global_id(0);
}
