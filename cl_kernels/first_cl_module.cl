
#pragma OPENCL EXTENSION CL_APPLE_gl_sharing : enable
#pragma OPENCL EXTENSION CL_KHR_gl_sharing : enable


__kernel void test(__global int * message, __global float * gl_buff, __kernel float3 point){
//	float3 point = (float3) (1.0, 2.0, 3.0);
	int gid = get_global_id(0);
	float4 a = (float4)(1.0, 2.0, 3.0, 4.0);
	float4 b = (float4)(2.0, 3.0, 4.0, 5.0);
	float4 c = a*b;
	message[gid] = c.w;
}


// //транспонированная
// 	float4 m1 = (float4)(vbo[mat_ofset], vbo[mat_ofset + 4], vbo[mat_ofset + 8], vbo[mat_ofset + 12]);
// 	float4 m2 = (float4) (vbo[mat_ofset + 1], vbo[mat_ofset + 4 + 1], vbo[mat_ofset + 8 + 1], vbo[mat_ofset + 12 + 1]);
// 	float4 m3 = (float4) (vbo[mat_ofset + 2], vbo[mat_ofset + 4 + 2], vbo[mat_ofset + 8 + 2], vbo[mat_ofset + 12 + 2]);
// 	float4 m4 = (float4) (vbo[mat_ofset + 3], vbo[mat_ofset + 4 + 3], vbo[mat_ofset + 8 + 3], vbo[mat_ofset + 12 + 3]);


// //изначальная
// 	float4 m1 = (float4)(vbo[mat_ofset], vbo[mat_ofset + 1], vbo[mat_ofset + 2], vbo[mat_ofset + 3]);
// 	float4 m2 = (float4) (vbo[mat_ofset + 4], vbo[mat_ofset + 4 + 1], vbo[mat_ofset + 4 + 2], vbo[mat_ofset + 4 + 3]);
// 	float4 m3 = (float4) (vbo[mat_ofset + 8], vbo[mat_ofset + 8 + 1], vbo[mat_ofset + 8 + 2], vbo[mat_ofset + 8 + 3]);
// 	float4 m4 = (float4) (vbo[mat_ofset + 12], vbo[mat_ofset + 12 + 1], vbo[mat_ofset + 12 + 2], vbo[mat_ofset + 12 + 3]);
