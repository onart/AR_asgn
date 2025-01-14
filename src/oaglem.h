/********************************************************************************
* 2D/3D OpenGL Game Engine
* Copyright 2022 onart@github
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*********************************************************************************/
#ifndef __OAGLEM_H__
#define __OAGLEM_H__

/*
* 수학 함수
* 구성:
* nvec, mat2, mat3, mat4, Quaternion
* 다른 모듈로 대체하는 법:
*	namespace onart{
*		using mat4 =(다른모듈 4x4행렬);
*	}
* 참고: mat4의 경우 이 모듈에서는 행 우선 순서입니다. 즉 uniform 전달 시 전치를 전달하도록 설정하는데, 위와 같이 다른 mat4를 사용하는 경우
* shader 코드에서 mat4를 보낼 때 전치를 전달하지 않도록 해야 합니다.
*/

#include <cmath>
#include <cfloat>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <numeric>

#include "oagle_simd.h"

#pragma warning(disable: 6294 6201 26495)

constexpr float PI = 3.14159265358979323846f;
constexpr float INF = INFINITY;
constexpr float _NAN = NAN;

/// <summary>
/// 라디안을 입력하면 도 단위로 변경합니다.
/// </summary>
constexpr float deg(float rad) { return rad * 180 / PI; }
/// <summary>
/// 도 단위를 입력하면 라디안으로 변경합니다.
/// </summary>
constexpr float rad(float deg) { return deg / 180 * PI; }

namespace onart {
	using byte = unsigned char;
	struct Quaternion;
	/// <summary>
	/// N차원 벡터입니다. 길이에 관계없이 상호 변환이 가능합니다.
	/// </summary>
	/// <typeparam name="T">벡터 성분의 타입입니다. 사칙 연산 및 부호 반전이 가능하여야 합니다.</typeparam>
	template <unsigned D, class T = float> union nvec {
		T entry[D];
		struct { T x, y, z, w; };
		struct { T r, g, b, a; };
		struct { T s, t, p, q; };
		struct { T left, down, width, height; };
		struct { T xy[2]; T zw[2]; };
		struct { T xyz[3]; };
		struct { T _x; T yz[2]; };
		struct { T __x; T yzw[3]; };
		/// <summary>
		/// 영벡터를 생성합니다.
		/// </summary>
		inline nvec() { static_assert(D > 1, "Vectors must be at least two dimensions."); memset(entry, 0, sizeof(entry)); }

		/// <summary>
		/// 벡터의 모든 값을 하나의 값으로 초기화합니다.
		/// </summary>
		inline nvec(T a) { static_assert(D > 1, "Vectors must be at least two dimensions."); setAll(entry, a, D > 4 ? D : 4); }

		/// <summary>
		/// 벡터의 값 중 앞 2~4개를 초기화합니다.
		/// </summary>
		inline nvec(T x, T y, T z = 0, T w = 0) : x(x), y(y), z(z), w(w) { static_assert(D > 1, "Vectors must be at least two dimensions."); }

		/// <summary>
		/// 복사 생성자입니다.
		/// </summary>
		inline nvec(const nvec& v) { memcpy(entry, v.entry, sizeof(entry)); }

		/// <summary>
		/// 배열을 이용하여 벡터를 생성합니다.
		/// </summary>
		inline nvec(const T* v) { static_assert(D > 1, "Vectors must be at least two dimensions."); memcpy(entry, v, sizeof(entry)); }

		/// <summary>
		/// 한 차원 낮은 벡터를 이용하여 생성합니다.
		/// </summary>
		/// <param name="v">한 차원 낮은 벡터</param>
		/// <param name="a">나머지 자리에 들어가는 값</param>
		inline nvec(const nvec<D - 1, T>& v, T a) { memcpy(entry, v.entry, sizeof(entry) - sizeof(T)); entry[D - 1] = a; }

		/// <summary>
		/// 다른 차원의 벡터를 사용하는 복사 생성자입니다. 가급적 차원 축소에만 사용하는 것이 좋습니다.
		/// </summary>
		template <unsigned E> inline nvec(const nvec<E, T>& v) { static_assert(D > 1, "Vectors must be at least two dimensions."); constexpr unsigned min = D > E ? E : D; memcpy(entry, v.entry, min * sizeof(T)); }

		/// <summary>
		/// 벡터의 모든 성분을 하나의 값으로 초기화합니다. operator=과 동일합니다.
		/// </summary>
		inline void set(T a) { setAll(entry, a, D > 4 ? D : 4); }

		/// <summary>
		/// 다른 벡터의 값을 복사해 옵니다. operator=과 동일합니다.
		/// </summary>
		/// <param name="v"></param>
		inline void set(const nvec& v) { memcpy(entry, v.entry, sizeof(entry));; }

		/// <summary>
		/// 다른 벡터의 값을 복사해 옵니다. operator=과 동일합니다.
		/// </summary>
		template <unsigned E> inline void set(const nvec<E, T>& v) { constexpr unsigned min = D > E ? E : D; memcpy(entry, v.entry, min * sizeof(T)); }

		/// <summary>
		/// 벡터의 모든 성분을 하나의 값으로 초기화합니다. set()과 동일합니다.
		/// </summary>
		inline nvec& operator=(T a) { set(a); return *this; }

		/// <summary>
		/// 다른 벡터의 값을 복사해 옵니다. set()과 동일합니다.
		/// </summary>
		inline nvec& operator=(const nvec& v) { set(v); return *this; }

		/// <summary>
		/// 다른 벡터의 값을 복사해 옵니다. set()과 동일합니다.
		/// </summary>
		template <unsigned E> inline nvec& operator=(const nvec<E, T>& v) { set(v); return *this; }

		/// <summary>
		/// 다른 벡터와 성분별 연산을 차원수가 낮은 쪽을 기준으로 합니다.
		/// </summary>
		template <unsigned E> inline nvec& operator+=(const nvec<E, T>& v) { constexpr unsigned min = D > E ? E : D; addAll(entry, v.entry, min); return *this; }
		template <unsigned E> inline nvec& operator-=(const nvec<E, T>& v) { constexpr unsigned min = D > E ? E : D; subAll(entry, v.entry, min); return *this; }
		template <unsigned E> inline nvec& operator*=(const nvec<E, T>& v) { constexpr unsigned min = D > E ? E : D; mulAll(entry, v.entry, min); return *this; }
		template <unsigned E> inline nvec& operator/=(const nvec<E, T>& v) { constexpr unsigned min = D > E ? E : D; divAll(entry, v.entry, min); return *this; }

		/// <summary>
		/// 같은 차원의 다른 벡터와 성분별 연산을 합니다.
		/// </summary>
		inline nvec& operator+=(const nvec& v) { if constexpr (D <= 4) add4(entry, v.entry); else addAll(entry, v.entry, D); return *this; }
		inline nvec& operator-=(const nvec& v) { if constexpr (D <= 4) sub4(entry, v.entry); else subAll(entry, v.entry, D); return *this; }
		inline nvec& operator*=(const nvec& v) { if constexpr (D <= 4) mul4(entry, v.entry); else mulAll(entry, v.entry, D); return *this; }
		inline nvec& operator/=(const nvec& v) { if constexpr (D <= 4) div4(entry, v.entry); else divAll(entry, v.entry, D); return *this; }

		/// <summary>
		/// 벡터의 모든 성분에 대하여 주어진 값과 연산합니다.
		/// </summary>
		inline nvec& operator+=(T a) { add4<T>(entry, a); for (unsigned i = 4; i < D; i++) entry[i] += a; return *this; }
		inline nvec& operator-=(T a) { sub4<T>(entry, a); for (unsigned i = 4; i < D; i++) entry[i] -= a; return *this; }
		inline nvec& operator*=(T a) { mul4<T>(entry, a); for (unsigned i = 4; i < D; i++) entry[i] *= a; return *this; }
		inline nvec& operator/=(T a) { div4<T>(entry, a); for (unsigned i = 4; i < D; i++) entry[i] /= a; return *this; }
		inline nvec operator+(T a) const { auto r(*this); r += a; return r; }
		inline nvec operator-(T a) const { auto r(*this); r -= a; return r; }
		inline nvec operator*(T a) const { auto r(*this); r *= a; return r; }
		inline nvec operator/(T a) const { auto r(*this); r /= a; return r; }

		/// <summary>
		/// 벡터의 모든 성분이 동일한 경우 참을 리턴합니다. 다른 크기의 벡터와의 비교를 지원하지 않습니다.
		/// </summary>
		inline bool operator==(const nvec& v) const { return memcmp(entry, v.entry, sizeof(nvec)) == 0; }
		inline bool operator!=(const nvec& v) const { return !operator==(v); }

		/// <summary>
		/// 다른 벡터와 성분별 연산을 차원수가 낮은 쪽을 기준으로 합니다. 리턴 차원수는 항상 좌변값과 동일합니다.
		/// </summary>
		template <unsigned E> inline nvec operator+(const nvec<E, T>& v) const { auto r(*this); r += v; return r; }
		template <unsigned E> inline nvec operator-(const nvec<E, T>& v) const { auto r(*this); r -= v; return r; }
		template <unsigned E> inline nvec operator*(const nvec<E, T>& v) const { auto r(*this); r *= v; return r; }
		template <unsigned E> inline nvec operator/(const nvec<E, T>& v) const { auto r(*this); r /= v; return r; }

		/// <summary>
		/// T형 배열로 사용할 수 있도록 포인터를 리턴합니다.
		/// </summary>
		inline operator T* () { return &x; }
		inline operator const T* () const { return &x; }

		/// <summary>
		/// 명시적 캐스트가 가능한 타입이라면 벡터도 명시적으로 캐스트가 가능합니다.
		/// 성분별로 캐스트해서 새로 만드는 것과 비교하여 특별히 성능적 면에서 나을 부분은 없으며
		/// 유연한 코드만을 위해 추가하였습니다.
		/// </summary>
		template <class T2> inline operator nvec<D, T2>() const { nvec<D, T2> n; for (unsigned i = 0; i < D; i++) { n[i] = (T2)entry[i]; } return n; }

		/// <summary>
		/// 부호를 반전시켜 리턴합니다.
		/// </summary>
		inline nvec operator-() const { return (*this) * -1; }

		/// <summary>
		/// 인덱스 연산자
		/// </summary>
		inline T& operator[](ptrdiff_t i) { assert(i < D); return entry[i]; }
		inline const T& operator[](ptrdiff_t i) const { assert(i < D); return entry[i]; }

		/// <summary>
		/// 벡터의 방향을 유지하고 길이를 1로 맞춘 것을 리턴합니다. 정수 벡터에서는 사용할 수 없습니다.
		/// </summary>
		inline nvec normal() const { return (*this) / length(); }

		/// <summary>
		/// 벡터의 길이를 1로 만듭니다.
		/// </summary>
		inline void normalize() { operator/=(length()); }

		/// <summary>
		/// 다른 벡터와의 내적을 리턴합니다. 다른 차원과의 연산을 지원하지 않습니다.
		/// dot 함수에 비해 행렬 간 곱 및 행렬 x 벡터에서 사용하기에 빠릅니다. (원인은 파악 중입니다)
		/// </summary>
		inline T dot2(const nvec& v) const {
			auto nv = (*this) * v; T s = 0;
			if constexpr (D == 2) return nv[0] + nv[1];
			else if constexpr (D == 3) return nv[0] + nv[1] + nv[2];
			else if constexpr (D == 4) return nv[0] + nv[1] + nv[2] + nv[3];
			else for (unsigned i = 0; i < D; i++)s += nv[i]; return s;
		}

		/// <summary>
		/// 다른 벡터와의 내적을 리턴합니다. 다른 차원과의 연산을 지원하지 않습니다.
		/// dot2 함수에 비해 단순히 벡터에서 사용하기에 빠릅니다. (원인은 파악 중입니다)
		/// </summary>
		inline T dot(const nvec& v) const { return std::transform_reduce(entry, entry + D, v.entry, (T)0); }

		/// <summary>
		/// 벡터 길이의 제곱을 리턴합니다.
		/// </summary>
		inline float length2() const { return dot(*this); }

		/// <summary>
		/// 벡터 길이를 리턴합니다.
		/// </summary>
		inline float length() const { return sqrtf(length2()); }

		/// <summary>
		/// 다른 벡터와의 거리 제곱을 리턴합니다.
		/// </summary>
		inline float distance2(const nvec& v) const { return (*this - v).length2(); }

		/// <summary>
		/// 다른 벡터와의 거리를 리턴합니다.
		/// </summary>
		inline float distance(const nvec& v) const { return sqrtf(distance(v)); }
	};

	using vec2 = nvec<2>;					using vec3 = nvec<3>;					using vec4 = nvec<4>;
	using ivec2 = nvec<2, int>;				using ivec3 = nvec<3, int>;				using ivec4 = nvec<4, int>;
	using uvec2 = nvec<2, unsigned>;		using uvec3 = nvec<3, unsigned>;		using uvec4 = nvec<4, unsigned>;
	using dvec2 = nvec<2, double>;			using dvec3 = nvec<3, double>;			using dvec4 = nvec<4, double>;

	/// <summary>
	/// 2개 2차원 실수 벡터의 외적의 z축 성분을 계산합니다.
	/// </summary>
	inline float cross2(const vec2& a, const vec2& b) { return a.x * b.y - a.y * b.x; }

	/// <summary>
	/// 2개 3차원 실수 벡터의 외적을 계산합니다.
	/// </summary>
	inline vec3 cross(const vec3& a, const vec3& b) { return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }

	/// <summary>
	/// 2개의 벡터를 선형 보간합니다.
	/// </summary>
	/// <param name="a">선형 보간 대상 1(t=0에 가까울수록 이 벡터에 가깝습니다.)</param>
	/// <param name="b">선형 보간 대상 2(t=1에 가까울수록 이 벡터에 가깝습니다.)</param>
	/// <param name="t">선형 보간 값</param>	
	template <unsigned D, class T> inline nvec<D, T> lerp(const nvec<D, T>& a, const nvec<D, T>& b, const nvec<D>& t) { return a * (1 - t) + b * t; }

	/// <summary>
	/// 2개의 벡터를 선형 보간합니다.
	/// </summary>
	/// <param name="a">선형 보간 대상 1(t=0에 가까울수록 이 벡터에 가깝습니다.)</param>
	/// <param name="b">선형 보간 대상 2(t=1에 가까울수록 이 벡터에 가깝습니다.)</param>
	/// <param name="t">선형 보간 값</param>
	template <unsigned D, class T> inline nvec<D, T> lerp(const nvec<D, T>& a, const nvec<D, T>& b, float t) { return a * (1 - t) + b * t; }

	/// <summary>
	/// 2개 단위 벡터를 구면 선형 보간합니다.
	/// </summary>
	/// <param name="a">구면 선형 보간 대상 1(t=0에 가까울수록 이 벡터에 가깝습니다.)</param>
	/// <param name="b">구면 선형 보간 대상 2(t=1에 가까울수록 이 벡터에 가깝습니다.)</param>
	/// <param name="t">구면 선형 보간 값</param>
	inline vec3 slerp(const vec3 a, const vec3& b, float t) {
		float sinx = cross(a, b).length();
		float theta = asinf(sinx);
		if (theta <= FLT_EPSILON)return a;
		return a * sinf(theta * (1 - t)) + b * sinf(theta * t);
	}

	/// <summary>
	/// 2차원 이미지의 회전연산을 위한 2x2 행렬입니다. 단, 3차원 연산의 z축을 0으로 고정하는 것이 더 일반적인 방법입니다.
	/// </summary>
	struct mat2 {
		union { float a[4]; struct { float _11, _12, _21, _22; }; };

		/// <summary>
		/// 단위행렬을 생성합니다.
		/// </summary>
		inline mat2() { _11 = _22 = 1; _12 = _21 = 0; }

		/// <summary>
		/// 행 우선 순서로 매개변수를 주어 행렬을 생성합니다.
		/// </summary>
		inline mat2(float _11, float _12, float _21, float _22) :_11(_11), _12(_12), _21(_21), _22(_22) { }

		/// <summary>
		/// 인덱스 연산자
		/// </summary>
		inline float& operator[](ptrdiff_t i) { return a[i]; }
		inline const float& operator[](ptrdiff_t i) const { return a[i]; }

		/// <summary>
		/// 복사 생성
		/// </summary>
		inline mat2(const mat2& m) { for (int i = 0; i < 4; i++) { a[i] = m[i]; } }

		/// <summary>
		/// 행렬을 단위행렬로 바꿉니다.
		/// </summary>
		inline void toI() { _11 = _22 = 1; _12 = _21 = 0; }

		/// <summary>
		/// 다른 행렬과 성분별로 더하거나 뺍니다.
		/// </summary>
		inline mat2& operator+=(const mat2& m) { for (int i = 0; i < 4; i++)a[i] += m[i]; return *this; }
		inline mat2& operator-=(const mat2& m) { for (int i = 0; i < 4; i++)a[i] -= m[i]; return *this; }
		inline mat2 operator+(const mat2& m) const { auto r = mat2(*this); r += m; return r; }
		inline mat2 operator-(const mat2& m) const { auto r = mat2(*this); r -= m; return r; }

		/// <summary>
		/// 행렬곱을 수행합니다.
		/// </summary>
		inline mat2 operator*(const mat2& m) const {
			return mat2(
				_11 * m._11 + _12 * m._21,
				_11 * m._12 + _12 * m._22,

				_21 * m._11 + _22 * m._21,
				_21 * m._12 + _22 * m._22
			);
		}
		inline mat2& operator*=(const mat2& m) { return *this = operator*(m); }

		/// <summary>
		/// 벡터에 선형변환을 적용하여 리턴합니다.
		/// </summary>
		inline vec2 operator*(const vec2& v) const { return vec2(_11 * v.x + _12 * v.y, _21 * v.x + _22 * v.y); }

		/// <summary>
		/// 행렬에 실수배를 합니다.
		/// </summary>
		inline mat2& operator*=(float f) { for (int i = 0; i < 4; i++) a[i] *= f; return *this; }
		inline mat2 operator*(float f) const { mat2 r(*this); r *= f; return r; }
		inline mat2& operator/=(float f) { for (int i = 0; i < 4; i++) a[i] /= f; return *this; }
		inline mat2 operator/(float f) const { mat2 r(*this); r /= f; return r; }

		/// <summary>
		/// 행렬식을 반환합니다.
		/// </summary>
		inline float det() const { return _11 * _22 - _12 * _21; }

		/// <summary>
		/// 역행렬을 반환합니다.
		/// </summary>
		inline mat2 inverse() const {
			float d = det();
#ifdef DEBUG
			if (d == 0) printf("%p: 이 행렬은 역행렬이 없거나 매우 큰 성분을 가집니다. NaN에 의해 예기치 못한 동작이 발생할 수 있습니다.\n", this);
#endif // DEBUG
			return mat2(_22, -_12, -_21, _11) / d;
		}

		/// <summary>
		/// 전치 행렬을 반환합니다.
		/// </summary>
		inline mat2 transpose() const { return mat2(_11, _21, _12, _22); }

		/// <summary>
		/// 행 우선 순서로 된 배열을 리턴합니다.
		/// </summary>
		inline operator float* () { return a; }
	};

	/// <summary>
	/// 3차원 모델의 회전연산 혹은 2차원 이미지의 아핀 변환을 위한 3x3 행렬입니다.
	/// </summary>
	struct mat3 {
		union { float a[9]; struct { float _11, _12, _13, _21, _22, _23, _31, _32, _33; }; };

		/// <summary>
		/// 단위행렬을 생성합니다.
		/// </summary>
		inline mat3() { memset(a, 0, sizeof(a)); _11 = _22 = _33 = 1; }

		/// <summary>
		/// 행 우선 순서로 매개변수를 주어 행렬을 생성합니다.
		/// </summary>
		inline mat3(float _11, float _12, float _13, float _21, float _22, float _23, float _31, float _32, float _33) :_11(_11), _12(_12), _13(_13), _21(_21), _22(_22), _23(_23), _31(_31), _32(_32), _33(_33) { }

		/// <summary>
		/// 행렬의 성분을 복사해서 생성합니다.
		/// </summary>
		inline mat3(const mat3& m) { memcpy(a, m.a, sizeof(a)); }

		/// <summary>
		/// 인덱스 연산자
		/// </summary>
		inline float& operator[](ptrdiff_t i) { return a[i]; }
		inline const float& operator[](ptrdiff_t i) const { return a[i]; }

		/// <summary>
		/// 행렬을 단위행렬로 바꿉니다.
		/// </summary>
		inline void toI() { memset(a, 0, sizeof(a)); _11 = _22 = _33 = 1; }

		/// <summary>
		/// 다른 행렬과 성분별로 더하거나 뺍니다.
		/// </summary>
		inline mat3& operator+=(const mat3& m) { for (int i = 0; i < 9; i++)a[i] += m[i]; return *this; }
		inline mat3& operator-=(const mat3& m) { for (int i = 0; i < 9; i++)a[i] -= m[i]; return *this; }
		inline mat3 operator+(const mat3& m) const { auto r = mat3(*this); r += m; return r; }
		inline mat3 operator-(const mat3& m) const { auto r = mat3(*this); r -= m; return r; }

		/// <summary>
		/// n행 벡터를 리턴합니다. 1~3만 입력 가능합니다.
		/// </summary>
		/// <param name="i">행 인덱스(1 base)</param>
		inline vec3 row(int i) const { assert(i <= 3 && i >= 1); int st = 3 * i - 3; return vec3(a + st); }

		/// <summary>
		/// n열 벡터를 리턴합니다. 1~4만 입력 가능합니다.
		/// </summary>
		/// <param name="i">열 인덱스(1 base)</param>
		inline vec3 col(int i) const { assert(i <= 3 && i >= 1); return vec3(a[i - 1], a[i + 2], a[i + 5]); }

		/// <summary>
		/// 행렬곱을 수행합니다.
		/// </summary>
		inline mat3 operator*(const mat3& m) const {
			mat3 ret;
			for (int i = 1, ent = 0; i <= 3; i++) {
				vec3 r = row(i);
				for (int j = 1; j <= 3; j++, ent++) {
					vec3 c = m.col(j);
					ret[ent] = r.dot2(c);
				}
			}
			return ret;
		}

		/// <summary>
		/// 행렬곱을 수행합니다.
		/// </summary>
		inline mat3& operator*=(const mat3& m) { return *this = operator*(m); }

		/// <summary>
		/// 벡터에 선형변환을 적용하여 리턴합니다.
		/// </summary>
		inline vec3 operator*(const vec3& v) const { return vec3(row(1).dot2(v), row(2).dot2(v), row(3).dot2(v)); }

		/// <summary>
		/// 행렬에 실수배를 합니다.
		/// </summary>
		inline mat3& operator*=(float f) { mulAll(a, f, 9); return *this; }
		inline mat3 operator*(float f) const { mat3 r(*this); r *= f; return r; }
		inline mat3& operator/=(float f) { divAll(a, f, 9); return *this; }
		inline mat3 operator/(float f) const { mat3 r(*this); r /= f; return r; }

		/// <summary>
		/// 행렬식을 반환합니다.
		/// </summary>
		inline float det() const { return _11 * (_22 * _33 - _23 * _32) + _12 * (_23 * _31 - _21 * _33) + _13 * (_21 * _32 - _22 * _31); }

		/// <summary>
		/// 행렬 대각 성분의 합을 반환합니다.
		/// </summary>
		inline float trace() const { return _11 + _22 + _33; }

		/// <summary>
		/// 역행렬을 반환합니다.
		/// </summary>
		inline mat3 inverse() const {
			float d = det();
#ifdef DEBUG
			if (d == 0) printf("%p: 이 행렬은 역행렬이 없거나 매우 큰 성분을 가집니다. NaN에 의해 예기치 못한 동작이 발생할 수 있습니다.\n", this);
#endif // DEBUG
			return mat3(
				(_22 * _33 - _32 * _23), (_13 * _32 - _12 * _33), (_12 * _23 - _13 * _22),
				(_23 * _31 - _21 * _33), (_11 * _33 - _13 * _31), (_21 * _13 - _11 * _23),
				(_21 * _32 - _31 * _22), (_31 * _12 - _11 * _32), (_11 * _22 - _21 * _12)
			) / d;
		}

		/// <summary>
		/// 전치 행렬을 반환합니다.
		/// </summary>
		inline mat3 transpose() const { return mat3(_11, _21, _31, _12, _22, _32, _13, _23, _33); }

		/// <summary>
		/// 행 우선 순서로 된 배열을 리턴합니다.
		/// </summary>
		inline operator float* () { return a; }

		/// <summary>
		/// 좌측 상단 2x2 행렬로 캐스트합니다.
		/// </summary>
		inline operator mat2() { return mat2(_11, _12, _21, _22); }

		/// <summary>
		/// 2차원 병진 행렬을 계산합니다.
		/// </summary>
		inline static mat3 translate(const vec2& t) {
			return mat3(
				1, 0, t.x,
				0, 1, t.y,
				0, 0, 1
			);
		}
		/// <summary>
		/// 2차원 병진 행렬을 계산합니다.
		/// </summary>
		inline static mat3 translate(float x, float y) {
			return mat3(
				1, 0, x,
				0, 1, y,
				0, 0, 1
			);
		}

		/// <summary>
		/// 2차원 크기 변환 행렬을 계산합니다.
		/// </summary>
		inline static mat3 scale(const vec2& t) {
			return mat3(
				t.x, 0, 0,
				0, t.y, 0,
				0, 0, 1
			);
		}
		/// <summary>
		/// 2차원 크기 변환 행렬을 계산합니다.
		/// </summary>
		inline static mat3 scale(float x, float y) {
			return mat3(
				x, 0, 0,
				0, y, 0,
				0, 0, 1
			);
		}

		/// <summary>
		/// Z축 기준의 2차원 회전을 리턴합니다. X, Y 축 방향을 원하는 경우 3x3 아핀 변환이 아닌 4x4 아핀 변환을 사용해야 합니다.
		/// </summary>
		inline static mat3 rotate(float z) {
			return mat3(
				cosf(z), -sinf(z), 0,
				sinf(z), cosf(z), 0,
				0, 0, 1
			);
		}

		/// <summary>
		/// 3차원 회전을 리턴합니다.
		/// </summary>
		/// <param name="roll">roll(X축 방향 회전)</param>
		/// <param name="pitch">pitch(Y축 방향 회전)</param>
		/// <param name="yaw">yaw(Z축 방향 회전)</param>
		inline static mat3 rotate(float roll, float pitch, float yaw);
	};

	/// <summary>
	/// 3차원 모델의 아핀 변환을 위한 4x4 행렬입니다.
	/// </summary>
	struct mat4 {
		union { float a[16]; struct { float _11, _12, _13, _14, _21, _22, _23, _24, _31, _32, _33, _34, _41, _42, _43, _44; }; };

		/// <summary>
		/// 단위행렬을 생성합니다.
		/// </summary>
		inline mat4() { memset(a, 0, sizeof(a)); _11 = _22 = _33 = _44 = 1; }

		/// <summary>
		/// 행 우선 순서로 매개변수를 주어 행렬을 생성합니다.
		/// </summary>
		inline mat4(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44) :_11(_11), _12(_12), _13(_13), _14(_14), _21(_21), _22(_22), _23(_23), _24(_24), _31(_31), _32(_32), _33(_33), _34(_34), _41(_41), _42(_42), _43(_43), _44(_44) { }

		/// <summary>
		/// 행렬의 성분을 복사해서 생성합니다.
		/// </summary>
		inline mat4(const mat4& m) { memcpy(a, m.a, sizeof(a)); }

		/// <summary>
		/// 인덱스 연산자
		/// </summary>
		inline float& operator[](ptrdiff_t i) { return a[i]; }
		inline const float& operator[](ptrdiff_t i) const { return a[i]; }

		/// <summary>
		/// 행렬을 단위행렬로 바꿉니다.
		/// </summary>
		inline void toI() { memset(a, 0, sizeof(a)); _11 = _22 = _33 = _44 = 1; }

		/// <summary>
		/// 단위행렬이면 true를 리턴합니다.
		/// </summary>
		inline bool isI() const { return memcmp(mat4().a, a, sizeof(a)) == 0; }

		/// <summary>
		/// 다른 행렬과 성분별로 더하거나 뺍니다.
		/// </summary>
		inline mat4& operator+=(const mat4& m) { add4<float>(a, m.a); add4<float>(a + 4, m.a + 4); add4<float>(a + 8, m.a + 8); add4<float>(a + 12, m.a + 12); return *this; }
		inline mat4& operator-=(const mat4& m) { sub4<float>(a, m.a); sub4<float>(a + 4, m.a + 4); sub4<float>(a + 8, m.a + 8); sub4<float>(a + 12, m.a + 12); return *this; }
		inline mat4 operator+(const mat4& m) const { auto r = mat4(*this); r += m; return r; }
		inline mat4 operator-(const mat4& m) const { auto r = mat4(*this); r -= m; return r; }

		/// <summary>
		/// n행 벡터를 리턴합니다. 1~4만 입력 가능합니다.
		/// </summary>
		/// <param name="i">행 인덱스(1 base)</param>
		inline vec4 row(int i) const { assert(i <= 4 && i >= 1); int st = 4 * i - 4; vec4 ret; memcpy(ret.entry, a + st, sizeof(ret.entry)); return ret; }

		/// <summary>
		/// n열 벡터를 리턴합니다. 1~4만 입력 가능합니다.
		/// </summary>
		/// <param name="i">열 인덱스(1 base)</param>
		inline vec4 col(int i) const { assert(i <= 4 && i >= 1); return vec4(a[i - 1], a[i + 3], a[i + 7], a[i + 11]); }

		/// <summary>
		/// 행렬곱을 수행합니다.
		/// </summary>
		inline mat4 operator*(const mat4& m) const {
			mat4 ret;
			for (int i = 1, ent = 0; i <= 4; i++) {
				vec4 r = row(i);
				for (int j = 1; j <= 4; j++, ent++) {
					vec4 c = m.col(j);
					ret[ent] = r.dot2(c);
				}
			}
			return ret;
		}

		/// <summary>
		/// 행렬곱을 수행합니다.
		/// </summary>
		inline mat4& operator*=(const mat4& m) { return *this = operator*(m); }

		/// <summary>
		/// 벡터에 선형변환을 적용하여 리턴합니다.
		/// </summary>
		inline vec4 operator*(const vec4& v) const {
			return vec4(row(1).dot2(v), row(2).dot2(v), row(3).dot2(v), row(4).dot2(v));
		}

		/// <summary>
		/// 행렬에 실수배를 합니다.
		/// </summary>
		inline mat4& operator*=(float f) { mulAll(a, f, 16); return *this; }
		inline mat4 operator*(float f) const { mat4 r(*this); r *= f; return r; }
		inline mat4& operator/=(float f) { divAll(a, f, 16); return *this; }
		inline mat4 operator/(float f) const { mat4 r(*this); r /= f; return r; }

		/// <summary>
		/// 행렬식을 반환합니다.
		/// </summary>
		inline float det() const {
			return
				_41 * _32 * _23 * _14 - _31 * _42 * _23 * _14 - _41 * _22 * _33 * _14 + _21 * _42 * _33 * _14 +
				_31 * _22 * _43 * _14 - _21 * _32 * _43 * _14 - _41 * _32 * _13 * _24 + _31 * _42 * _13 * _24 +
				_41 * _12 * _33 * _24 - _11 * _42 * _33 * _24 - _31 * _12 * _43 * _24 + _11 * _32 * _43 * _24 +
				_41 * _22 * _13 * _34 - _21 * _42 * _13 * _34 - _41 * _12 * _23 * _34 + _11 * _42 * _23 * _34 +
				_21 * _12 * _43 * _34 - _11 * _22 * _43 * _34 - _31 * _22 * _13 * _44 + _21 * _32 * _13 * _44 +
				_31 * _12 * _23 * _44 - _11 * _32 * _23 * _44 - _21 * _12 * _33 * _44 + _11 * _22 * _33 * _44;
		}

		/// <summary>
		/// 행렬의 대각선 성분 합을 리턴합니다.
		/// </summary>
		inline float trace() const { return _11 + _22 + _33 + _44; }

		/// <summary>
		/// 좌측 상단 3x3 행렬로 캐스트합니다.
		/// </summary>
		inline operator mat3() const { return mat3(_11, _12, _13, _21, _22, _23, _31, _32, _33); }

		/// <summary>
		/// 아핀 변환의 역행렬을 조금 더 효율적으로 구합니다.
		/// 참조: mat4::iTRS()
		/// </summary>
		inline mat4 affineInverse() const {
			//https://stackoverflow.com/questions/2624422/efficient-4x4-matrix-inverse-affine-transform
			mat3 ir(mat3(*this).inverse());
			vec3 p = ir * (-this->col(4));
			return mat4(
				ir._11, ir._12, ir._13, p.x,
				ir._21, ir._22, ir._23, p.y,
				ir._31, ir._32, ir._33, p.z,
				0, 0, 0, 1
			);
		}

		/// <summary>
		/// 역행렬을 반환합니다.
		/// </summary>
		inline mat4 inverse() const {
			float d = det();
#ifdef DEBUG
			if (d == 0) printf("%p: 이 행렬은 역행렬이 없거나 매우 큰 성분을 가집니다. NaN에 의해 예기치 못한 동작이 발생할 수 있습니다.\n", this);
#endif // DEBUG
			return mat4(
				(_32 * _43 * _24 - _42 * _33 * _24 + _42 * _23 * _34 - _22 * _43 * _34 - _32 * _23 * _44 + _22 * _33 * _44),
				(_42 * _33 * _14 - _32 * _43 * _14 - _42 * _13 * _34 + _12 * _43 * _34 + _32 * _13 * _44 - _12 * _33 * _44),
				(_22 * _43 * _14 - _42 * _23 * _14 + _42 * _13 * _24 - _12 * _43 * _24 - _22 * _13 * _44 + _12 * _23 * _44),
				(_32 * _23 * _14 - _22 * _33 * _14 - _32 * _13 * _24 + _12 * _33 * _24 + _22 * _13 * _34 - _12 * _23 * _34),

				(_41 * _33 * _24 - _31 * _43 * _24 - _41 * _23 * _34 + _21 * _43 * _34 + _31 * _23 * _44 - _21 * _33 * _44),
				(_31 * _43 * _14 - _41 * _33 * _14 + _41 * _13 * _34 - _11 * _43 * _34 - _31 * _13 * _44 + _11 * _33 * _44),
				(_41 * _23 * _14 - _21 * _43 * _14 - _41 * _13 * _24 + _11 * _43 * _24 + _21 * _13 * _44 - _11 * _23 * _44),
				(_21 * _33 * _14 - _31 * _23 * _14 + _31 * _13 * _24 - _11 * _33 * _24 - _21 * _13 * _34 + _11 * _23 * _34),

				(_31 * _42 * _24 - _41 * _32 * _24 + _41 * _22 * _34 - _21 * _42 * _34 - _31 * _22 * _44 + _21 * _32 * _44),
				(_41 * _32 * _14 - _31 * _42 * _14 - _41 * _12 * _34 + _11 * _42 * _34 + _31 * _12 * _44 - _11 * _32 * _44),
				(_21 * _42 * _14 - _41 * _22 * _14 + _41 * _12 * _24 - _11 * _42 * _24 - _21 * _12 * _44 + _11 * _22 * _44),
				(_31 * _22 * _14 - _21 * _32 * _14 - _31 * _12 * _24 + _11 * _32 * _24 + _21 * _12 * _34 - _11 * _22 * _34),

				(_41 * _32 * _23 - _31 * _42 * _23 - _41 * _22 * _33 + _21 * _42 * _33 + _31 * _22 * _43 - _21 * _32 * _43),
				(_31 * _42 * _13 - _41 * _32 * _13 + _41 * _12 * _33 - _11 * _42 * _33 - _31 * _12 * _43 + _11 * _32 * _43),
				(_41 * _22 * _13 - _21 * _42 * _13 - _41 * _12 * _23 + _11 * _42 * _23 + _21 * _12 * _43 - _11 * _22 * _43),
				(_21 * _32 * _13 - _31 * _22 * _13 + _31 * _12 * _23 - _11 * _32 * _23 - _21 * _12 * _33 + _11 * _22 * _33)) / d;
		}

		/// <summary>
		/// 전치 행렬을 반환합니다.
		/// </summary>
		inline mat4 transpose() const { return mat4(_11, _21, _31, _41, _12, _22, _32, _42, _13, _23, _33, _43, _14, _24, _34, _44); }

		/// <summary>
		/// 행 우선 순서로 된 배열을 리턴합니다.
		/// </summary>
		inline operator float* () { return a; }
		inline operator const float* () const { return a; }

		/// <summary>
		/// 3차원 병진 행렬을 계산합니다.
		/// </summary>
		inline static mat4 translate(const vec3& t) {
			return mat4(
				1, 0, 0, t.x,
				0, 1, 0, t.y,
				0, 0, 1, t.z,
				0, 0, 0, 1
			);
		}
		/// <summary>
		/// 3차원 병진 행렬을 계산합니다.
		/// </summary>
		inline static mat4 translate(float x, float y, float z) {
			return mat4(
				1, 0, 0, x,
				0, 1, 0, y,
				0, 0, 1, z,
				0, 0, 0, 1
			);
		}

		/// <summary>
		/// 3차원 크기 변환 행렬을 계산합니다.
		/// </summary>
		inline static mat4 scale(const vec3& t) {
			return mat4(
				t.x, 0, 0, 0,
				0, t.y, 0, 0,
				0, 0, t.z, 0,
				0, 0, 0, 1
			);
		}
		/// <summary>
		/// 3차원 크기 변환 행렬을 계산합니다.
		/// </summary>
		inline static mat4 scale(float x, float y, float z) {
			return mat4(
				x, 0, 0, 0,
				0, y, 0, 0,
				0, 0, z, 0,
				0, 0, 0, 1
			);
		}

		/// <summary>
		/// 3차원 회전 행렬을 계산합니다.
		/// </summary>
		inline static mat4 rotate(const vec3& axis, float angle);

		/// <summary>
		/// 3차원 회전 행렬을 계산합니다. cf) 오른손 법칙
		/// </summary>
		/// <param name="roll">roll(X축 방향 회전)</param>
		/// <param name="pitch">pitch(Y축 방향 회전)</param>
		/// <param name="yaw">yaw(Z축 방향 회전)</param>
		inline static mat4 rotate(float roll, float pitch, float yaw);

		/// <summary>
		/// 3차원 회전 사원수를 행렬 형태로 리턴합니다.
		/// </summary>
		inline static mat4 rotate(const Quaternion& q);

		/// <summary>
		/// lookAt 형식의 뷰 행렬을 계산합니다.
		/// </summary>
		/// <param name="eye">눈의 위치</param>
		/// <param name="at">피사체 위치</param>
		/// <param name="up">위쪽 방향: 화면 상에서 위쪽 방향이 이 벡터의 방향과 비슷해집니다.</param>
		inline static mat4 lookAt(const vec3& eye, const vec3& at, const vec3& up) {
			vec3 n = (eye - at).normal();
			vec3 u = cross(up, n).normal();
			vec3 v = cross(n, u).normal();
			return mat4(
				u.x, u.y, u.z, -(u.dot(eye)),
				v.x, v.y, v.z, -(v.dot(eye)),
				n.x, n.y, n.z, -(n.dot(eye)),
				0, 0, 0, 1
			);
		}
		/// <summary>
		/// 병진, 회전, 배율 행렬 T, R, S를 구한 후 곱하는 것보다 조금 더 빠르게 계산합니다.
		/// </summary>
		/// <param name="translation">병진</param>
		/// <param name="rotation">회전</param>
		/// <param name="scale">배율</param>
		inline static mat4 TRS(const vec3& translation, const Quaternion& rotation, const vec3& scale);
		/// <summary>
		/// 주어진 병진, 회전, 배율을 포함하는 아핀 변환의 역변환을 단순계산보다 조금 빠르게 계산합니다. 역변환이 없는 경우(ex: 배율에 영이 있음)
		/// 비정상적인 값이 리턴될 것입니다.
		/// </summary>
		/// <param name="translation">병진</param>
		/// <param name="rotation">회전</param>
		/// <param name="scale">배율</param>
		inline static mat4 iTRS(const vec3& translation, const Quaternion& rotation, const vec3& scale);
		/// <summary>
		/// 표준 뷰 볼륨 직육면체에 들어올 대상 뿔대(절두체)를 조절하는 투사 행렬을 계산합니다.
		/// 순수 2D 게임을 만드는 경우, 단위 행렬에 aspect만 적용하면 됩니다.
		/// </summary>
		/// <param name="fovy">field of view Y: 뿔대의 Y축 방향(화면 기준 세로) 라디안 각도입니다.</param>
		/// <param name="aspect">표시 뷰포트 비율(가로/세로)입니다.</param>
		/// <param name="dnear">뿔대에서 가장 가까운 거리입니다. 이 이하의 거리는 보는 눈보다 뒤에 있는 것으로 칩니다.</param>
		/// <param name="dfar">뿔대에서 가장 먼 거리입니다. 이 이상의 거리는 보이지 않습니다.</param>
		inline static mat4 perspective(float fovy, float aspect, float dnear, float dfar) {
			mat4 r(
				1, 0, 0, 0,
				0, 1 / tanf(fovy / 2), 0, 0,
				0, 0, (dnear + dfar) / (dnear - dfar), (2 * dnear * dfar) / (dnear - dfar),
				0, 0, -1, 0
			);
			r._11 = r._22 / aspect;
			return r;
		}

		/// <summary>
		/// 한 직사각형을 다른 직사각형으로 변환하는 행렬을 계산합니다. 직사각형의 형식은 좌-하-폭-높이입니다. z 좌표는 동일하다고 가정하여 xy 평면에서만 이동합니다.
		/// </summary>
		/// <param name="r1">변환 전 직사각형</param>
		/// <param name="r2">변환 후 직사각형</param>
		/// <param name="z">직사각형이 위치할 z좌표(-1이 가장 겉)</param>
		inline static mat4 r2r(const vec4& r1, const vec4& r2, float z = 0) {
			vec4 sc = r2 / r1;	vec4 tr = r2 - r1 * vec4(sc.z, sc.w);
			return mat4(
				sc.z, 0, 0, tr.x,
				0, sc.w, 0, tr.y,
				0, 0, 1, z,
				0, 0, 0, 1
			);
		}

		/// <summary>
		/// 본 엔진에서 Mesh::get("rect")로 제공되는 단위 직사각형(중심이 0,0이고 한 변의 길이가 1인 정사각형)을 다른 직사각형으로 변환하는 행렬을 계산합니다.
		/// 직사각형의 형식은 좌-하-폭-높이입니다. z 좌표는 동일하다고 가정하여 xy 평면에서만 이동합니다.
		/// </summary>
		/// <param name="r2">변환 후 직사각형</param>
		/// <param name="z">직사각형이 위치할 z좌표(-1이 가장 겉)</param>
		inline static mat4 r2r(const vec4& r2, float z = 0) {
			return r2r(vec4(-0.5f, -0.5f, 1, 1), r2, z);
		}

		/// <summary>
		/// 한 직사각형(L-D-W-H 형식)을 다른 직사각형의 안쪽에 맞게 변환합니다. 즉 중심을 공유하며, 원본 직사각형의 종횡비는 유지하면서 가장 큰 직사각형이 되도록 리턴합니다.
		/// </summary>
		/// <param name="r1">변환 전 직사각형</param>
		/// <param name="r2">변환 후 직사각형</param>
		/// <param name="z">직사각형이 위치할 z좌표(-1이 가장 겉)</param>
		inline static mat4 r2r2(const vec4& r1, const vec4& r2, float z = 0) {
			float r = r1.width / r1.height;
			vec4 targ(r2);
			if (targ.width < targ.height * r) {	// 세로선을 맞출 것
				targ.down += (targ.height - targ.width / r) / 2;
				targ.height = targ.width / r;
			}
			else {	// 가로선을 맞출 것
				targ.left += (targ.width - targ.height * r) / 2;
				targ.width = targ.height * r;
			}
			return r2r(r1, targ, z);
		}
	};

	/// <summary>
	/// 행렬 클래스의 일반화입니다. 행 우선 순서이며 정사각 행렬 mat2, mat3, mat4와 다르게 기본적인 연산만 지원됩니다. 영행렬 및 복사 생성만 허용되며 이외에는 직접 초기화만 지원합니다.
	/// </summary>
	template <unsigned R, unsigned C>
	struct mat {
		float entry[R][C];

		inline mat() { memset(entry, 0, sizeof(entry)); }
		inline mat(const mat& m) { memcpy(entry, m.entry, sizeof(entry)); }
		inline mat(const mat2& m2) { static_assert(R == 2 && C == 2, "The declared one is not a 2x2 matrix."); memcpy(entry, m2.a, sizeof(entry)); }
		inline mat(const mat3& m3) { static_assert(R == 3 && C == 3, "The declared one is not a 3x3 matrix."); memcpy(entry, m3.a, sizeof(entry)); }
		inline mat(const mat4& m4) { static_assert(R == 4 && C == 4, "The declared one is not a 4x4 matrix."); memcpy(entry, m4.a, sizeof(entry)); }
		inline const float* operator[](ptrdiff_t i) const { return entry[i]; }
		inline float* operator[](ptrdiff_t i) { return entry[i]; }
		/// <summary>
		/// r행 벡터를 리턴합니다. 1~(행 수) 외의 입력에 대한 결과는 정의되지 않았습니다.
		/// </summary>
		inline nvec<C, float> row(unsigned r) const { assert(r >= 1 && r <= R); return nvec<C, float>(entry[r - 1]); }
		/// <summary>
		/// c열 벡터를 리턴합니다. 1~(열 수) 외의 입력에 대한 결과는 정의되지 않았습니다.
		/// </summary>
		inline nvec<R, float> col(unsigned c) const { assert(c >= 1 && c <= C);	nvec<R, float> ret;	for (unsigned i = 0; i < R; i++)ret[i] = entry[i][c - 1]; return ret; }
		/// <summary>
		/// 열벡터를 행렬의 우측에 곱합니다.
		/// 이 모듈은 다른 차원 벡터와의 암시적 형 변환을 허용하므로 올바른 차원을 사용하도록 주의하세요.
		/// </summary>
		inline nvec<R, float> operator*(const nvec<C, float>& v) const {
			nvec<R, float> ret;
			for (unsigned i = 0; i < R; i++) {
				nvec<C, float> rw(entry[i]);
				ret[i] = rw.dot(v);
			}
			return ret;
		}
		/// <summary>
		/// 전치 행렬을 리턴합니다.
		/// </summary>
		inline mat<C, R> transpose() const {
			mat<C, R> ret;
			for (unsigned i = 0; i < R; i++) {
				for (unsigned j = 0; j < C; j++) {
					ret[j][i] = entry[i][j];
				}
			}
			return ret;
		}
		/// <summary>
		/// 주어진 다른 행렬을 우측에 곱한 결과를 리턴합니다.
		/// </summary>
		template <unsigned C2>
		inline mat<R, C2> operator*(const mat<C, C2>& m) {
			mat<R, C2> ret;
			mat<C2, C> mm(std::move(m.transpose()));
			for (unsigned i = 0; i < R; i++) {
				nvec<C, float> r(entry[i]);
				for (unsigned j = 0; j < C2; j++) {
					ret[i][j] = r.dot2(mm.row(j + 1));
				}
			}
			return ret;
		}
		/// <summary>
		/// 유사역행렬을 구합니다. 없으면 영행렬을 리턴합니다.
		/// </summary>
		inline mat<C, R> pseudoInverse() const {
			mat<C, R> ret;
			// TODO: 할 수도 안 할 수도
			return ret;
		}
	};

	using mat2x3 = mat<2, 3>;	using mat3x2 = mat<3, 2>;

	/// <summary>
	/// 3차원 회전 등을 표현하는 사원수입니다. 1, i, j, k 부분에 해당하는 c1, ci, cj, ck 멤버를 가집니다.
	/// </summary>
	struct Quaternion {
		union {
			struct { float c1, ci, cj, ck; };
			struct { float w, x, y, z; };
		};

		/// <summary>
		/// 사원수를 생성합니다.
		/// </summary>
		/// <param name="o">실수부분</param>
		/// <param name="i">i부분</param>
		/// <param name="j">j부분</param>
		/// <param name="k">k부분</param>
		inline Quaternion(float o = 1, float i = 0, float j = 0, float k = 0) :c1(o), ci(i), cj(j), ck(k) {};

		/// <summary>
		/// 각속도 벡터(그 크기가 초당 회전각, 방향이 회전축인 벡터)에 대응하는 사원수를 생성합니다.
		/// </summary>
		inline Quaternion(const vec3& av) :c1(0) { memcpy(&ci, av.entry, sizeof(float) * 3); }

		/// <summary>
		/// 사원수를 복사해서 생성합니다.
		/// </summary>
		inline Quaternion(const Quaternion& q) { set4<float>(&c1, &(q.c1)); }

		/// <summary>
		/// 사원수 크기의 제곱을 리턴합니다.
		/// </summary>
		inline float abs2() const { return reinterpret_cast<const vec4*>(this)->length2(); }

		/// <summary>
		/// 사원수 크기를 리턴합니다.
		/// </summary
		inline float abs() const { return sqrtf(abs2()); }

		/// <summary>
		/// 사원수가 무회전인지 확인합니다.
		/// </summary>
		inline bool is1() const { return c1 == 1 && ci == 0 && cj == 0 && ck == 0; }

		/// <summary>
		/// 켤레(공액)사원수를 리턴합니다.
		/// </summary>
		inline Quaternion conjugate() const { return Quaternion(c1, -ci, -cj, -ck); }

		/// <summary>
		/// 사원수의 우측에 곱하면 1이 되는 값을 리턴합니다.
		/// </summary>
		inline Quaternion inverse() const { return conjugate() / abs2(); }

		/// <summary>
		/// 사원수 곱셈 연산자입니다.
		/// </summary>
		inline Quaternion operator*(const Quaternion& q) const {
			Quaternion q_c1 = q * c1;
			Quaternion q_ci = Quaternion(-q.ci, q.c1, -q.ck, q.cj) * ci;
			Quaternion q_cj = Quaternion(-q.cj, q.ck, q.c1, -q.ci) * cj;
			Quaternion q_ck = Quaternion(-q.ck, -q.cj, q.ci, q.c1) * ck;
			return q_c1 + q_ci + q_cj + q_ck;
		}

		/// <summary>
		/// 사원수 간 사칙 연산입니다. 모든 연산은 이 사원수를 기준으로 우측에 적용됩니다.
		/// </summary>
		inline Quaternion& operator+=(const Quaternion& q) { add4<float>(&c1, &(q.c1)); return *this; }
		inline Quaternion& operator-=(const Quaternion& q) { sub4<float>(&c1, &(q.c1)); return *this; }
		inline Quaternion& operator*=(const Quaternion& q) { *this = *this * q; return *this; }	// multiplying on the left is more commonly used operation
		inline Quaternion& operator/=(const Quaternion& q) { *this = *this * q.inverse(); return *this; }
		inline Quaternion operator*(float f) const { Quaternion ret(*this); mul4<float>(&(ret.c1), f); return ret; }
		inline Quaternion operator/(float f) const { Quaternion ret(*this); div4<float>(&(ret.c1), f); return ret; }
		inline Quaternion& operator*=(float f) { *this = *this * f; return *this; }
		inline Quaternion& operator/=(float f) { *this = *this / f; return *this; }
		inline Quaternion operator+(const Quaternion& q) const { Quaternion r(*this); r += q; return r; }
		inline Quaternion operator-(const Quaternion& q) const { Quaternion r(*this); r -= q; return r; }
		inline Quaternion operator/(const Quaternion& q) const { Quaternion r(*this); r /= q; return r; }

		/// <summary>
		/// 이 사원수의 회전 사원수 버전을 리턴합니다.
		/// </summary>
		inline Quaternion normal() const { return (*this) * (1 / abs()); }

		/// <summary>
		/// 이 사원수의 절댓값을 1로 만듭니다.
		/// </summary>
		inline void normalize() { operator*=(1 / abs()); }

		/// <summary>
		/// 사원수의 부호를 반대로 합니다. 180도 뒤집은 것과 동일합니다.
		/// </summary>
		inline Quaternion operator-() const { return Quaternion(-c1, -ci, -cj, -ck); }

		/// <summary>
		/// 사원수 회전을 합칩니다. 기존 사원수가 먼저 적용되며 크기 1임을 확인하지 않습니다.
		/// </summary>
		inline void compound(const Quaternion& q) { *this = q * (*this); }

		/// <summary>
		/// 사원수 회전을 합칩니다. 기존 사원수가 먼저 적용됩니다.
		/// </summary>
		/// <param name="axis">회전축</param>
		/// <param name="angle">회전각(라디안)</param>
		inline void compound(const vec3& axis, float angle) { auto q = rotation(axis, angle); compound(q); }

		/// <summary>
		/// 사원수를 회전 행렬로 변형합니다.
		/// </summary>
		inline mat4 toMat4() const {
			Quaternion i = (*this) * ci;
			Quaternion j = (*this) * cj;
			Quaternion k = (*this) * ck;
			return mat4(
				1 - 2 * (j.cj + k.ck), 2 * (i.cj - k.c1), 2 * (i.ck + j.c1), 0,
				2 * (i.cj + k.c1), 1 - 2 * (i.ci + k.ck), 2 * (j.ck - i.c1), 0,
				2 * (i.ck - j.c1), 2 * (j.ck + i.c1), 1 - 2 * (i.ci + j.cj), 0,
				0, 0, 0, 1
			);
		}

		inline mat3 toMat3() const {
			Quaternion i = (*this) * ci;
			Quaternion j = (*this) * cj;
			Quaternion k = (*this) * ck;
			return mat3(
				1 - 2 * (j.cj + k.ck), 2 * (i.cj - k.c1), 2 * (i.ck + j.c1),
				2 * (i.cj + k.c1), 1 - 2 * (i.ci + k.ck), 2 * (j.ck - i.c1),
				2 * (i.ck - j.c1), 2 * (j.ck + i.c1), 1 - 2 * (i.ci + j.cj)
			);
		}

		/// <summary>
		/// 첫 성분에 회전각(라디안), 나머지 성분에 3차원 회전축을 담아 리턴합니다.
		/// 부동소수점 정밀도 문제를 고려하여 정규화하여 계산합니다. 회전사원수가 아니라도 nan이 발생하지 않으므로 주의하세요.
		/// </summary>
		inline vec4 axis() const {
			Quaternion ax = *this / abs();
			float angle = acosf(ax.c1) * 2;
			float sinha = sqrtf(1 - ax.c1 * ax.c1);
			ax /= sinha;
			ax.c1 = angle;
			return vec4(*(reinterpret_cast<vec4*>(&ax)));
		}

		/// <summary>
		/// 이 회전의 오일러 각 (x,y,z순)의 형태로 리턴합니다.
		/// 부동소수점 정밀도 문제를 고려하여 정규화하여 계산합니다. 회전사원수가 아니라도 nan이 발생하지 않으므로 주의하세요.
		/// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_angles_conversion
		/// </summary>
		inline vec3 toEuler() const {
			Quaternion q = *this / abs();
			vec3 a;
			float sinrcosp = 2 * (q.c1 * q.ci + q.cj * q.ck);
			float cosrcosp = 1 - 2 * (q.ci * q.ci + q.cj * q.cj);
			a.x = atan2f(sinrcosp, cosrcosp);
			float sinp = 2 * (q.c1 * q.cj - q.ck * q.ci);
			if (sinp >= 1) a.y = PI / 2;
			else if (sinp <= -1) a.y = -PI / 2;
			else a.y = asinf(sinp);
			float sinycosp = 2 * (q.c1 * q.ck + q.ci * q.cj);
			float cosycosp = 1 - 2 * (q.cj * q.cj + q.ck * q.ck);
			a.z = atan2f(sinycosp, cosycosp);
			return a;
		}

		/// <summary>
		/// 축과 각이 주어졌을 때 회전값을 리턴합니다.
		/// 회전축은 자동으로 정규화됩니다.
		/// </summary>
		/// <param name="axis">회전축</param>
		/// <param name="angle">회전각(라디안)</param>
		inline static Quaternion rotation(const vec3& axis, float angle) {
			angle *= 0.5f;
			float c = cosf(angle), s = sinf(angle);
			vec3 nv = axis.normal() * s;
			return Quaternion(c, nv.x, nv.y, nv.z);
		}

		/// <summary>
		/// 축과 각이 주어졌을 때 회전값을 리턴합니다.
		/// 입력된 회전축은 정규화되지 않습니다.
		/// </summary>
		/// <param name="uaxis">회전축(단위벡터)</param>
		/// <param name="angle">회전각(라디안)</param>
		/// <returns></returns>
		inline static Quaternion rotationByUnit(const vec3& uaxis, float angle) {
			angle *= 0.5f;
			float c = cosf(angle), s = sinf(angle);
			vec3 nv(uaxis); nv *= s;
			return Quaternion(c, nv.x, nv.y, nv.z);
		}

		/// <summary>
		/// 주어진 방향을 바라보도록 하는 회전 사원수를 생성합니다.
		/// </summary>
		/// <param name="after">원하는 방향(회전축이 아닙니다.)</param>
		/// <param name="after">현재 방향(회전축이 아닙니다.)</param>
		inline static Quaternion direction(const vec3& after, const vec3& before) {
			vec3 mid(after);
			mid *= sqrtf(before.length2() / after.length2());
			mid += before;
			mid /= mid.length();
			return Quaternion(0, mid.x, mid.y, mid.z);
		}

		/// <summary>
		/// 주어진 방향을 바라보도록 하는 회전 사원수를 생성합니다.
		/// 입력 벡터들의 길이는 반드시 동일해야 합니다.
		/// </summary>
		/// <param name="after">원하는 방향(회전축이 아닙니다.)</param>
		/// <param name="after">현재 방향(회전축이 아닙니다.)</param>
		inline static Quaternion directionByUnit(const vec3& after, const vec3& before) {
			vec3 mid(after);
			mid += before;
			mid /= mid.length();
			return Quaternion(0, mid.x, mid.y, mid.z);
		}

		/// <summary>
		/// 오일러 회전에 해당하는 사원수를 생성합니다. cf) 오른손 법칙
		/// <para>https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles</para>
		/// </summary>
		/// <param name="roll">roll(X축 방향 회전)</param>
		/// <param name="yaw">yaw(Z축 방향 회전)</param>
		/// <param name="pitch">pitch(Y축 방향 회전)</param>
		inline static Quaternion euler(float roll, float pitch, float yaw) {
			float cy = cosf(yaw / 2);	float sy = sinf(yaw / 2);
			float cp = cosf(pitch / 2);	float sp = sinf(pitch / 2);
			float cr = cosf(roll / 2);	float sr = sinf(roll / 2);

			return Quaternion(cr * cp * cy + sr * sp * sy, sr * cp * cy - cr * sp * sy, cr * sp * cy + sr * cp * sy, cr * cp * sy - sr * sp * cy);
		}

	};

	/// <summary>
	/// 사원수의 선형 보간을 리턴합니다.
	/// </summary>
	/// <param name="q1">선형 보간 대상 1(t=0에 가까울수록 이것에 가깝습니다.)</param>
	/// <param name="q2">선형 보간 대상 2(t=1에 가까울수록 이것에 가깝습니다.)</param>
	/// <param name="t">선형 보간 값</param>
	inline Quaternion lerp(const Quaternion& q1, const Quaternion& q2, float t) {
		Quaternion ret = q1 * (1 - t) + q2 * t;
		return ret / ret.abs();
	}

	/// <summary>
	/// 사원수의 구면 선형 보간을 리턴합니다.
	/// </summary>
	/// <param name="q1">선형 보간 대상 1(t=0에 가까울수록 이것에 가깝습니다.)</param>
	/// <param name="q2">선형 보간 대상 2(t=1에 가까울수록 이것에 가깝습니다.)</param>
	/// <param name="t">선형 보간 값</param>
	inline Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t) {
		float Wa, Wb;
		float costh = reinterpret_cast<const vec4*>(&q1)->dot(*reinterpret_cast<const vec4*>(&q2)) / q1.abs() / q2.abs();
		// 정밀도 오차로 인한 nan 방지
		if (costh > 1) costh = 1;
		else if (costh < -1) costh = -1;
		float theta = acos(costh);
		float sn = sin(theta);

		// q1=q2이거나 180도 차이인 경우
		if (sn <= FLT_EPSILON) return q1;
		Wa = sin((1 - t) * theta) / sn;
		Wb = sin(t * theta) / sn;

		Quaternion r = q1 * Wa + q2 * Wb;
		return r / r.abs();
	}

	inline mat3 mat3::rotate(float roll, float pitch, float yaw) { return mat3(mat4::rotate(roll, pitch, yaw)); }
	inline mat4 mat4::rotate(const vec3& axis, float angle) { return Quaternion::rotation(axis, angle).toMat4(); }
	inline mat4 mat4::rotate(float roll, float pitch, float yaw) { return Quaternion::euler(roll, pitch, yaw).toMat4(); }
	inline mat4 mat4::rotate(const Quaternion& q) { return q.toMat4(); }
	inline mat4 mat4::TRS(const vec3& translation, const Quaternion& rotation, const vec3& scale) {
		// SIMD 미적용 시 곱 30회/합 6회, T*R*S 따로 하는 경우 곱 149회/합 102회
		// SIMD 적용 시 곱 15회/합 6회, 따로 하는 경우 곱 44회/합 102회
		mat4 r = rotation.toMat4();
		vec4 sc(scale);
		mul4<float>(r.a, sc);
		mul4<float>(r.a + 4, sc);
		mul4<float>(r.a + 8, sc);
		r[3] = translation.x;
		r[7] = translation.y;
		r[11] = translation.z;
		return r;
	}

	inline mat4 mat4::iTRS(const vec3& translation, const Quaternion& rotation, const vec3& scale) {
		// SIMD 적용 시 곱 19회/합 18회
		mat4 r = rotation.conjugate().toMat4();	// 공액사원수=역회전
		vec3 sc(1); sc /= scale;
		mul4(r.a, sc.x);
		mul4(r.a + 4, sc.y);
		mul4(r.a + 8, sc.z);
		vec3 itr = r * (-translation);
		r[3] = itr.x;
		r[7] = itr.y;
		r[11] = itr.z;
		return r;
	}

	// 연산자 추가 오버로딩
	template<unsigned D, class T>inline nvec<D, T> operator+(float f, const nvec<D, T>& v) { return v + f; }
	template<unsigned D, class T>inline nvec<D, T> operator*(float f, const nvec<D, T>& v) { return v * f; }
	inline mat4 operator*(float f, const mat4& m) { return m * f; }
	inline Quaternion operator*(float f, const Quaternion& q) { return q * f; }

	/// <summary>
	/// 3x2 행렬의 유사역행렬을 리턴합니다. 없으면 영행렬을 리턴합니다.
	/// </summary>
	inline mat2x3 pseudoInverse(const mat3x2& m) {
		const float& a = m[0][0], & b = m[0][1], & c = m[1][0], & d = m[1][1], & e = m[2][0], & f = m[2][1];
		float dd = b * (c * (b * c - a * d) + e * (b * e - a * f)) + a * (a * d * d + a * f * f - b * c * d - b * e * f) + (d * e - c * f) * (d * e - c * f);
		mat2x3 ret;
		if (dd != 0) {
			dd = 1 / dd;
			ret[0][0] = d * d * a + f * f * a - d * b * c - f * b * e;
			ret[0][1] = -b * a * d + b * b * c + f * f * c - f * d * e;
			ret[0][2] = -b * a * f + b * b * e - d * c * f + d * d * e;
			ret[1][0] = -c * a * d - e * a * f + c * c * b + e * e * b;
			ret[1][1] = -a * b * c + a * a * d - e * c * f + e * e * d;
			ret[1][2] = -a * b * e + a * a * f - c * d * e + c * c * f;
			mulAll(ret[0], dd, 6);
		}
		return ret;
	}

	/// <summary>
	/// 최대/최소 제한을 두어 자른 값을 리턴합니다. 첫 값은 복사생성되니 기본자료형이 아닌 것을 사용할 때는 주의하세요.
	/// </summary>
	/// <param name="t">자르기 전 값</param>
	/// <param name="min">최솟값</param>
	/// <param name="max">최댓값</param>
	/// <returns></returns>
	template<class T>inline T clamp(T t, const T& min, const T& max) {
		t = t < min ? min : t;
		t = t > max ? max : t;
		return t;
	};

	/// <summary>
	/// 2차원 평면에서 2개 선분의 교점을 리턴합니다.
	/// 교점이 없는 경우 (nan, nan)이 리턴됩니다.
	/// 나란한 경우 교점 여부에 무관하게 (nan, 0)이 리턴됩니다.
	/// https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection#Given_two_points_on_each_line_segment
	/// </summary>
	/// <param name="p1">선분 1의 끝점 1</param>
	/// <param name="p2">선분 1의 끝점 2</param>
	/// <param name="q1">선분 2의 끝점 1</param>
	/// <param name="q2">선분 2의 끝점 2</param>
	inline vec2 intersect(const vec2& p1, const vec2& p2, const vec2& q1, const vec2& q2) {
		// cf) n개 중 접촉하는 선분 https://www.geeksforgeeks.org/given-a-set-of-line-segments-find-if-any-two-segments-intersect/
		vec2 _13(p1 - q1);
		vec2 _34(q1 - q2);
		vec2 _12(p1 - p2);
		float d = cross2(_12, _34);
		if (d == 0) return vec2(_NAN, 0);
		float vn = cross2(_13, _12);
		float tn = cross2(_13, _34);
		if (d < 0) {
			if (tn <= 0 && tn >= d && vn <= 0 && vn >= d) return p1 - _12 * (tn / d);
			else return _NAN;
		}
		else {
			if (tn >= 0 && tn <= d && vn >= 0 && vn <= d) return p1 - _12 * (tn / d);
			else return _NAN;
		}
	}

	/// <summary>
	/// 2차원 평면에서 2개 선분이 만나는지 여부만 확인합니다.
	/// </summary>
	/// <param name="p1">선분 1의 끝점 1</param>
	/// <param name="p2">선분 1의 끝점 2</param>
	/// <param name="q1">선분 2의 끝점 1</param>
	/// <param name="q2">선분 2의 끝점 2</param>
	inline bool intersect2(const vec2& p1, const vec2& p2, const vec2& q1, const vec2& q2) {
		vec2 b1(p1 - q1);
		vec2 b2(p2 - q1);
		float dt = b1.dot(b2);
		if (dt < 0 && fabs(dt * dt - b1.length2() * b2.length2()) < FLT_EPSILON) return true;
		vec2 qq2(q2 - q1);
		mat2 bm(b1.x, b2.x, b1.y, b2.y);
		bm = bm.inverse();
		vec2 st = bm * qq2;
		return (st.x >= 0 && st.y >= 0 && st.x + st.y >= 1.0f);
	}

	/// <summary>
	/// 3차원 공간 상의 점이 동일 평면 내 삼각형 안에 있는지 확인합니다.
	/// 삼각형과 점이 동일 평면 내에 있지 않은 경우에 대한 결과는 별도로 정의되지 않았습니다.
	/// </summary>
	/// <param name="p">점</param>
	/// <param name="t1">삼각형 꼭짓점 1</param>
	/// <param name="t2">삼각형 꼭짓점 2</param>
	/// <param name="t3">삼각형 꼭짓점 3</param>
	inline bool pointInTriangle(const vec3& p, const vec3& t1, const vec3& t2, const vec3& t3) {
		vec3 _12(t2 - t1);
		vec3 _23(t3 - t2);
		vec3 p1(p - t1);
		vec3 p2(p - t2);
		vec3 p12(std::move(cross(_12, p1)));
		vec3 p23(std::move(cross(_12, p1)));
		{
			vec3 p123(p12 * p23);
			if (p123.x < 0 || p123.y < 0 || p123.z < 0) {
				return false;
			}
		}
		vec3 p3(p - t3);
		vec3 _31(t1 - t3);
		vec3 p31(std::move(cross(_12, p1)));
		{
			vec3 p123(p12 * p31);
			return !(p123.x < 0 || p123.y < 0 || p123.z < 0);
		}
	}

	inline bool pointInTriangle2(const vec3& p, const vec3& t1, const vec3& t2, const vec3& t3) {
		// pseudo inverse 필요.
	}

	/// <summary>
	/// 편리한 디버그를 위한 값 출력 함수입니다.
	/// </summary>
	/// <param name="v">표시할 변수</param>
	/// <param name="tag">이름</param>
	inline void print(const vec2& v, const char* tag = "", char end = '\n') { printf("%s: %f %f%c", tag, v.x, v.y, end); }
	inline void print(const ivec2& v, const char* tag = "", char end = '\n') { printf("%s: %d %d%c", tag, v.x, v.y, end); }
	inline void print(const uvec2& v, const char* tag = "", char end = '\n') { printf("%s: %u %u%c", tag, v.x, v.y, end); }
	inline void print(const dvec2& v, const char* tag = "", char end = '\n') { printf("%s: %f %f%c", tag, v.x, v.y, end); }
	inline void print(const vec3& v, const char* tag = "", char end = '\n') { printf("%s: %f %f %f%c", tag, v.x, v.y, v.z, end); }
	inline void print(const ivec3& v, const char* tag = "", char end = '\n') { printf("%s: %d %d %d%c", tag, v.x, v.y, v.z, end); }
	inline void print(const uvec3& v, const char* tag = "", char end = '\n') { printf("%s: %u %u %u%c", tag, v.x, v.y, v.z, end); }
	inline void print(const dvec3& v, const char* tag = "", char end = '\n') { printf("%s: %f %f %f%c", tag, v.x, v.y, v.z, end); }
	inline void print(const vec4& v, const char* tag = "", char end = '\n') { printf("%s: %f %f %f %f%c", tag, v.x, v.y, v.z, v.w, end); }
	inline void print(const ivec4& v, const char* tag = "", char end = '\n') { printf("%s: %d %d %d %d%c", tag, v.x, v.y, v.z, v.w, end); }
	inline void print(const uvec4& v, const char* tag = "", char end = '\n') { printf("%s: %u %u %u %u%c", tag, v.x, v.y, v.z, v.w, end); }
	inline void print(const dvec4& v, const char* tag = "", char end = '\n') { printf("%s: %f %f %f %f%c", tag, v.x, v.y, v.z, v.w, end); }
	inline void print(const Quaternion& q, const char* tag = "", char end = '\n') { printf("%s: %f + %fi + %fj + %fk%c", tag, q.c1, q.ci, q.cj, q.ck, end); }
	inline void print(const mat4& m, const char* tag = "") { printf("%s:\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n", tag, m._11, m._12, m._13, m._14, m._21, m._22, m._23, m._24, m._31, m._32, m._33, m._34, m._41, m._42, m._43, m._44); }
	inline void print(int i, const char* tag = "", char end = '\n') { printf("%s: %d%c", tag, i, end); }
	inline void print(int64_t i, const char* tag = "", char end = '\n') { printf("%s: %lld%c", tag, i, end); }
	inline void print(unsigned i, const char* tag = "", char end = '\n') { printf("%s: %d%c", tag, i, end); }
	inline void print(uint64_t i, const char* tag = "", char end = '\n') { printf("%s: %lld%c", tag, i, end); }
	inline void print(float i, const char* tag = "", char end = '\n') { printf("%s: %f%c", tag, i, end); }
}

#endif // !__OAGLEM_H__