
#pragma OPENCL EXTENSION CL_APPLE_gl_sharing : enable
#pragma OPENCL EXTENSION CL_KHR_gl_sharing : enable

__kernel void test( __global float * vbo, 
					__global uint * ebo, 
					__global float * res, 
					__private float3 origin, 
					__private float3 dir,
					__private uint _mat_ofset){


//	float3 point = (float3) (1.0, 2.0, 3.0);

	// float3 origin = (float3) (1.0, 0.0, 0.0); 
	// float3 dir = (float3) (1.0, 1.0, 0.0);

	int instance = get_global_id(0);
	int face = get_global_id(1);
	int faces = get_global_size(1);

	uint mat_ofset = _mat_ofset + instance * 16 ; // отступаем на 16 float * номер матрицы

/* получает вершины треугольника */
	float4 a1 = (float4)(vbo[ebo[face*3]*3], vbo[ebo[face*3]*3+1], vbo[ebo[face*3]*3+2], 1.0);
	float4 a2 = (float4)(vbo[ebo[face*3+1]*3], vbo[ebo[face*3+1]*3+1], vbo[ebo[face*3+1]*3+2], 1.0);
	float4 a3 = (float4)(vbo[ebo[face*3+2]*3], vbo[ebo[face*3+2]*3+1], vbo[ebo[face*3+2]*3+2], 1.0);


/* получаем матрицу для инстанса */
	float4 m1 = (float4) (vbo[mat_ofset], vbo[mat_ofset + 4], vbo[mat_ofset + 8], vbo[mat_ofset + 12]);
	float4 m2 = (float4) (vbo[mat_ofset + 1], vbo[mat_ofset + 4 + 1], vbo[mat_ofset + 8 + 1], vbo[mat_ofset + 12 + 1]);
	float4 m3 = (float4) (vbo[mat_ofset + 2], vbo[mat_ofset + 4 + 2], vbo[mat_ofset + 8 + 2], vbo[mat_ofset + 12 + 2]);
	float4 m4 = (float4) (vbo[mat_ofset + 3], vbo[mat_ofset + 4 + 3], vbo[mat_ofset + 8 + 3], vbo[mat_ofset + 12 + 3]);
	
	float16 mat = (float16)(m1, m2, m3, m4);

	float4 b1 = (float4)(dot(m1, a1), dot(m2, a1), dot(m3, a1), dot(m4, a1));
	float4 b2 = (float4)(dot(m1, a2), dot(m2, a2), dot(m3, a2), dot(m4, a2));
	float4 b3 = (float4)(dot(m1, a3), dot(m2, a3), dot(m3, a3), dot(m4, a3));

	float3 v0 = b1.xyz;
	float3 v1 = b2.xyz;
	float3 v2 = b3.xyz;

	float3 D = dir;
	float3 T = origin - v0;
	float3 E1 = v1 - v0;
	float3 E2 = v2 - v0;
	float3 P = cross(D, E2);
	float3 Q = cross(T, E1);

	float denom = dot(P, E1);


	float t = dot(Q, E2)/denom;
	float u = dot(P, T)/denom;
	float v = dot(Q, D)/denom;
	float t1 = 1 - u - v;

//	res[face] = (float4)(t1, u, v, t);
	
	res[face + faces*instance] = -1;

	if(	t1 >= 0 && t1 <= 1 &&
		u >= 0 && u <= 1 &&
		v >= 0 && v <= 1 ){
	
		res[face + faces*instance] = t;
	}

}

__kernel void minimaze(){

}


