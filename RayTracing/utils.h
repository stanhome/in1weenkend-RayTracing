#pragma once
#include <random>
#include <chrono>

#include "vec3.h"

#define M_2PI	   6.28318530717958647692	// 2 * pi
#define M_PI       3.14159265358979323846   // pi
#define M_PI_2     1.57079632679489661923   // pi/2
#define M_PI_4     0.785398163397448309616  // pi/4
#define M_1_PI     0.318309886183790671538  // 1/pi
#define M_2_PI     0.636619772367581343076  // 2/pi

#define DEG_2_RAD  0.017453292519943295770	// pi / 180

std::default_random_engine s_randGenerator;

void initUtils() {
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	s_randGenerator = std::default_random_engine(seed);
}

// return Floating point value in range [0, 1)
float randCanonical() {
	return std::generate_canonical<float, std::numeric_limits<float>::digits>(s_randGenerator);
}


//////////////////////////////////////////////////////////////////////////
// geometry math

// return 在单位球内的 向量
vec3 randomInUnitSphere() {
	vec3 p;
	do
	{
		p = 2.0 * vec3(randCanonical(), randCanonical(), randCanonical()) - vec3(1, 1, 1);
	} while (vec3::dot(p, p) >= 1.0);

	return p;
}

// return 在单位球表面上的 向量
vec3 randomOnUnitSphere() {
	vec3 p;
	do 
	{
		p = 2.0 * vec3(randCanonical(), randCanonical(), randCanonical()) - vec3(1, 1, 1);
	} while (vec3::dot(p, p) >= 1.0);

	return p.normalized();
}

inline vec3 randomToSphere(float radius, float distanceSquared) {
	float r1 = randCanonical();
	float r2 = randCanonical();
	float z = 1 + r2 * (sqrt(1 - radius * radius / distanceSquared) - 1);
	float phi = M_2PI * r1;
	float x = cos(phi) * sqrt(1 - z * z);
	float y = sin(phi) * sqrt(1 - z * z);

	return vec3(x, y, z);
}

vec3 randomCosineDirection() {
	float r1 = randCanonical();
	float r2 = randCanonical();

	float z = sqrt(1 - r2); // cos(theta)
	float phi = M_2PI * r1;
	float x = cos(phi) * sqrt(r2);
	float y = sin(phi) * sqrt(r2);

	return vec3(x, y, z);
}

vec3 randomInUnitDisk() {
	vec3 p;
	do 
	{
		p = 2.0 * vec3(randCanonical(), randCanonical(), 0) - vec3(1, 1, 0);
	} while (vec3::dot(p, p) >= 1.0);

	return p;
}

// 反射
vec3 reflect(const vec3 &v, const vec3 &n) {
	return v - 2 * vec3::dot(v, n) * n;
}

//折射(斯涅耳定律)
bool refract(const vec3 &v, const vec3 &n, float niOverNt, vec3 &refracted) {
	vec3 vNormalized = v.normalized();
	float dt = vec3::dot(vNormalized, n);

	// 根据斯涅耳定律，n sin(theta) = n’sin(theta’) 推得：
	float discriminant = 1.0 - niOverNt * niOverNt * (1 - dt* dt);
	if (discriminant > 0)
	{
		// 发生折射
		refracted = niOverNt * (v + abs(dt) * n) - n * sqrt(discriminant);
		return true;
	}
	else
	{
		// 发生全反射
		return false;
	}
}

// 反射和折射
float schlick(float cosine, float refIdx) {
	float r0 = (1 - refIdx) / (1 + refIdx);
	r0 = r0 * r0;
	
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}

void getSphereUV(const vec3 &pos, float &u, float &v) {
	float phi = atan2(pos.z, pos.x); // -pi to pi
	float theta = acos(pos.y); // return [0,pi], asin return [-pi/2,+pi/2]
	u = 1 - (phi + M_PI) / (2 * M_PI);
	v = 1 - theta / M_PI;
}

