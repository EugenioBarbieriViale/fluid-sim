__kernel void calcSin(__global const float *data, __global float *resu) {
   int id = get_global_id(0);
   resu[id] = sin(data[id]);
}
