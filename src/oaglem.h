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
* ���� �Լ�
* ����:
* nvec, mat2, mat3, mat4, Quaternion
* �ٸ� ���� ��ü�ϴ� ��:
*	namespace onart{
*		using mat4 =(�ٸ���� 4x4���);
*	}
* ����: mat4�� ��� �� ��⿡���� �� �켱 �����Դϴ�. �� uniform ���� �� ��ġ�� �����ϵ��� �����ϴµ�, ���� ���� �ٸ� mat4�� ����ϴ� ���
* shader �ڵ忡�� mat4�� ���� �� ��ġ�� �������� �ʵ��� �ؾ� �մϴ�.
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
/// ������ �Է��ϸ� �� ������ �����մϴ�.
/// </summary>
constexpr float deg(float rad) { return rad * 180 / PI; }
/// <summary>
/// �� ������ �Է��ϸ� �������� �����մϴ�.
/// </summary>
constexpr float rad(float deg) { return deg / 180 * PI; }

namespace onart {
	using byte = unsigned char;
	struct Quaternion;
	/// <summary>
	/// N���� �����Դϴ�. ���̿� ������� ��ȣ ��ȯ�� �����մϴ�.
	/// </summary>
	/// <typeparam name="T">���� ������ Ÿ���Դϴ�. ��Ģ ���� �� ��ȣ ������ �����Ͽ��� �մϴ�.</typeparam>
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
		/// �����͸� �����մϴ�.
		/// </summary>
		inline nvec() { static_assert(D > 1, "Vectors must be at least two dimensions."); memset(entry, 0, sizeof(entry)); }

		/// <summary>
		/// ������ ��� ���� �ϳ��� ������ �ʱ�ȭ�մϴ�.
		/// </summary>
		inline nvec(T a) { static_assert(D > 1, "Vectors must be at least two dimensions."); setAll(entry, a, D > 4 ? D : 4); }

		/// <summary>
		/// ������ �� �� �� 2~4���� �ʱ�ȭ�մϴ�.
		/// </summary>
		inline nvec(T x, T y, T z = 0, T w = 0) : x(x), y(y), z(z), w(w) { static_assert(D > 1, "Vectors must be at least two dimensions."); }

		/// <summary>
		/// ���� �������Դϴ�.
		/// </summary>
		inline nvec(const nvec& v) { memcpy(entry, v.entry, sizeof(entry)); }

		/// <summary>
		/// �迭�� �̿��Ͽ� ���͸� �����մϴ�.
		/// </summary>
		inline nvec(const T* v) { static_assert(D > 1, "Vectors must be at least two dimensions."); memcpy(entry, v, sizeof(entry)); }

		/// <summary>
		/// �� ���� ���� ���͸� �̿��Ͽ� �����մϴ�.
		/// </summary>
		/// <param name="v">�� ���� ���� ����</param>
		/// <param name="a">������ �ڸ��� ���� ��</param>
		inline nvec(const nvec<D - 1, T>& v, T a) { memcpy(entry, v.entry, sizeof(entry) - sizeof(T)); entry[D - 1] = a; }

		/// <summary>
		/// �ٸ� ������ ���͸� ����ϴ� ���� �������Դϴ�. ������ ���� ��ҿ��� ����ϴ� ���� �����ϴ�.
		/// </summary>
		template <unsigned E> inline nvec(const nvec<E, T>& v) { static_assert(D > 1, "Vectors must be at least two dimensions."); constexpr unsigned min = D > E ? E : D; memcpy(entry, v.entry, min * sizeof(T)); }

		/// <summary>
		/// ������ ��� ������ �ϳ��� ������ �ʱ�ȭ�մϴ�. operator=�� �����մϴ�.
		/// </summary>
		inline void set(T a) { setAll(entry, a, D > 4 ? D : 4); }

		/// <summary>
		/// �ٸ� ������ ���� ������ �ɴϴ�. operator=�� �����մϴ�.
		/// </summary>
		/// <param name="v"></param>
		inline void set(const nvec& v) { memcpy(entry, v.entry, sizeof(entry));; }

		/// <summary>
		/// �ٸ� ������ ���� ������ �ɴϴ�. operator=�� �����մϴ�.
		/// </summary>
		template <unsigned E> inline void set(const nvec<E, T>& v) { constexpr unsigned min = D > E ? E : D; memcpy(entry, v.entry, min * sizeof(T)); }

		/// <summary>
		/// ������ ��� ������ �ϳ��� ������ �ʱ�ȭ�մϴ�. set()�� �����մϴ�.
		/// </summary>
		inline nvec& operator=(T a) { set(a); return *this; }

		/// <summary>
		/// �ٸ� ������ ���� ������ �ɴϴ�. set()�� �����մϴ�.
		/// </summary>
		inline nvec& operator=(const nvec& v) { set(v); return *this; }

		/// <summary>
		/// �ٸ� ������ ���� ������ �ɴϴ�. set()�� �����մϴ�.
		/// </summary>
		template <unsigned E> inline nvec& operator=(const nvec<E, T>& v) { set(v); return *this; }

		/// <summary>
		/// �ٸ� ���Ϳ� ���к� ������ �������� ���� ���� �������� �մϴ�.
		/// </summary>
		template <unsigned E> inline nvec& operator+=(const nvec<E, T>& v) { constexpr unsigned min = D > E ? E : D; addAll(entry, v.entry, min); return *this; }
		template <unsigned E> inline nvec& operator-=(const nvec<E, T>& v) { constexpr unsigned min = D > E ? E : D; subAll(entry, v.entry, min); return *this; }
		template <unsigned E> inline nvec& operator*=(const nvec<E, T>& v) { constexpr unsigned min = D > E ? E : D; mulAll(entry, v.entry, min); return *this; }
		template <unsigned E> inline nvec& operator/=(const nvec<E, T>& v) { constexpr unsigned min = D > E ? E : D; divAll(entry, v.entry, min); return *this; }

		/// <summary>
		/// ���� ������ �ٸ� ���Ϳ� ���к� ������ �մϴ�.
		/// </summary>
		inline nvec& operator+=(const nvec& v) { if constexpr (D <= 4) add4(entry, v.entry); else addAll(entry, v.entry, D); return *this; }
		inline nvec& operator-=(const nvec& v) { if constexpr (D <= 4) sub4(entry, v.entry); else subAll(entry, v.entry, D); return *this; }
		inline nvec& operator*=(const nvec& v) { if constexpr (D <= 4) mul4(entry, v.entry); else mulAll(entry, v.entry, D); return *this; }
		inline nvec& operator/=(const nvec& v) { if constexpr (D <= 4) div4(entry, v.entry); else divAll(entry, v.entry, D); return *this; }

		/// <summary>
		/// ������ ��� ���п� ���Ͽ� �־��� ���� �����մϴ�.
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
		/// ������ ��� ������ ������ ��� ���� �����մϴ�. �ٸ� ũ���� ���Ϳ��� �񱳸� �������� �ʽ��ϴ�.
		/// </summary>
		inline bool operator==(const nvec& v) const { return memcmp(entry, v.entry, sizeof(nvec)) == 0; }
		inline bool operator!=(const nvec& v) const { return !operator==(v); }

		/// <summary>
		/// �ٸ� ���Ϳ� ���к� ������ �������� ���� ���� �������� �մϴ�. ���� �������� �׻� �º����� �����մϴ�.
		/// </summary>
		template <unsigned E> inline nvec operator+(const nvec<E, T>& v) const { auto r(*this); r += v; return r; }
		template <unsigned E> inline nvec operator-(const nvec<E, T>& v) const { auto r(*this); r -= v; return r; }
		template <unsigned E> inline nvec operator*(const nvec<E, T>& v) const { auto r(*this); r *= v; return r; }
		template <unsigned E> inline nvec operator/(const nvec<E, T>& v) const { auto r(*this); r /= v; return r; }

		/// <summary>
		/// T�� �迭�� ����� �� �ֵ��� �����͸� �����մϴ�.
		/// </summary>
		inline operator T* () { return &x; }
		inline operator const T* () const { return &x; }

		/// <summary>
		/// ����� ĳ��Ʈ�� ������ Ÿ���̶�� ���͵� ��������� ĳ��Ʈ�� �����մϴ�.
		/// ���к��� ĳ��Ʈ�ؼ� ���� ����� �Ͱ� ���Ͽ� Ư���� ������ �鿡�� ���� �κ��� ������
		/// ������ �ڵ常�� ���� �߰��Ͽ����ϴ�.
		/// </summary>
		template <class T2> inline operator nvec<D, T2>() const { nvec<D, T2> n; for (unsigned i = 0; i < D; i++) { n[i] = (T2)entry[i]; } return n; }

		/// <summary>
		/// ��ȣ�� �������� �����մϴ�.
		/// </summary>
		inline nvec operator-() const { return (*this) * -1; }

		/// <summary>
		/// �ε��� ������
		/// </summary>
		inline T& operator[](ptrdiff_t i) { assert(i < D); return entry[i]; }
		inline const T& operator[](ptrdiff_t i) const { assert(i < D); return entry[i]; }

		/// <summary>
		/// ������ ������ �����ϰ� ���̸� 1�� ���� ���� �����մϴ�. ���� ���Ϳ����� ����� �� �����ϴ�.
		/// </summary>
		inline nvec normal() const { return (*this) / length(); }

		/// <summary>
		/// ������ ���̸� 1�� ����ϴ�.
		/// </summary>
		inline void normalize() { operator/=(length()); }

		/// <summary>
		/// �ٸ� ���Ϳ��� ������ �����մϴ�. �ٸ� �������� ������ �������� �ʽ��ϴ�.
		/// dot �Լ��� ���� ��� �� �� �� ��� x ���Ϳ��� ����ϱ⿡ �����ϴ�. (������ �ľ� ���Դϴ�)
		/// </summary>
		inline T dot2(const nvec& v) const {
			auto nv = (*this) * v; T s = 0;
			if constexpr (D == 2) return nv[0] + nv[1];
			else if constexpr (D == 3) return nv[0] + nv[1] + nv[2];
			else if constexpr (D == 4) return nv[0] + nv[1] + nv[2] + nv[3];
			else for (unsigned i = 0; i < D; i++)s += nv[i]; return s;
		}

		/// <summary>
		/// �ٸ� ���Ϳ��� ������ �����մϴ�. �ٸ� �������� ������ �������� �ʽ��ϴ�.
		/// dot2 �Լ��� ���� �ܼ��� ���Ϳ��� ����ϱ⿡ �����ϴ�. (������ �ľ� ���Դϴ�)
		/// </summary>
		inline T dot(const nvec& v) const { return std::transform_reduce(entry, entry + D, v.entry, (T)0); }

		/// <summary>
		/// ���� ������ ������ �����մϴ�.
		/// </summary>
		inline float length2() const { return dot(*this); }

		/// <summary>
		/// ���� ���̸� �����մϴ�.
		/// </summary>
		inline float length() const { return sqrtf(length2()); }

		/// <summary>
		/// �ٸ� ���Ϳ��� �Ÿ� ������ �����մϴ�.
		/// </summary>
		inline float distance2(const nvec& v) const { return (*this - v).length2(); }

		/// <summary>
		/// �ٸ� ���Ϳ��� �Ÿ��� �����մϴ�.
		/// </summary>
		inline float distance(const nvec& v) const { return sqrtf(distance(v)); }
	};

	using vec2 = nvec<2>;					using vec3 = nvec<3>;					using vec4 = nvec<4>;
	using ivec2 = nvec<2, int>;				using ivec3 = nvec<3, int>;				using ivec4 = nvec<4, int>;
	using uvec2 = nvec<2, unsigned>;		using uvec3 = nvec<3, unsigned>;		using uvec4 = nvec<4, unsigned>;
	using dvec2 = nvec<2, double>;			using dvec3 = nvec<3, double>;			using dvec4 = nvec<4, double>;

	/// <summary>
	/// 2�� 2���� �Ǽ� ������ ������ z�� ������ ����մϴ�.
	/// </summary>
	inline float cross2(const vec2& a, const vec2& b) { return a.x * b.y - a.y * b.x; }

	/// <summary>
	/// 2�� 3���� �Ǽ� ������ ������ ����մϴ�.
	/// </summary>
	inline vec3 cross(const vec3& a, const vec3& b) { return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }

	/// <summary>
	/// 2���� ���͸� ���� �����մϴ�.
	/// </summary>
	/// <param name="a">���� ���� ��� 1(t=0�� �������� �� ���Ϳ� �������ϴ�.)</param>
	/// <param name="b">���� ���� ��� 2(t=1�� �������� �� ���Ϳ� �������ϴ�.)</param>
	/// <param name="t">���� ���� ��</param>	
	template <unsigned D, class T> inline nvec<D, T> lerp(const nvec<D, T>& a, const nvec<D, T>& b, const nvec<D>& t) { return a * (1 - t) + b * t; }

	/// <summary>
	/// 2���� ���͸� ���� �����մϴ�.
	/// </summary>
	/// <param name="a">���� ���� ��� 1(t=0�� �������� �� ���Ϳ� �������ϴ�.)</param>
	/// <param name="b">���� ���� ��� 2(t=1�� �������� �� ���Ϳ� �������ϴ�.)</param>
	/// <param name="t">���� ���� ��</param>
	template <unsigned D, class T> inline nvec<D, T> lerp(const nvec<D, T>& a, const nvec<D, T>& b, float t) { return a * (1 - t) + b * t; }

	/// <summary>
	/// 2�� ���� ���͸� ���� ���� �����մϴ�.
	/// </summary>
	/// <param name="a">���� ���� ���� ��� 1(t=0�� �������� �� ���Ϳ� �������ϴ�.)</param>
	/// <param name="b">���� ���� ���� ��� 2(t=1�� �������� �� ���Ϳ� �������ϴ�.)</param>
	/// <param name="t">���� ���� ���� ��</param>
	inline vec3 slerp(const vec3 a, const vec3& b, float t) {
		float sinx = cross(a, b).length();
		float theta = asinf(sinx);
		if (theta <= FLT_EPSILON)return a;
		return a * sinf(theta * (1 - t)) + b * sinf(theta * t);
	}

	/// <summary>
	/// 2���� �̹����� ȸ�������� ���� 2x2 ����Դϴ�. ��, 3���� ������ z���� 0���� �����ϴ� ���� �� �Ϲ����� ����Դϴ�.
	/// </summary>
	struct mat2 {
		union { float a[4]; struct { float _11, _12, _21, _22; }; };

		/// <summary>
		/// ��������� �����մϴ�.
		/// </summary>
		inline mat2() { _11 = _22 = 1; _12 = _21 = 0; }

		/// <summary>
		/// �� �켱 ������ �Ű������� �־� ����� �����մϴ�.
		/// </summary>
		inline mat2(float _11, float _12, float _21, float _22) :_11(_11), _12(_12), _21(_21), _22(_22) { }

		/// <summary>
		/// �ε��� ������
		/// </summary>
		inline float& operator[](ptrdiff_t i) { return a[i]; }
		inline const float& operator[](ptrdiff_t i) const { return a[i]; }

		/// <summary>
		/// ���� ����
		/// </summary>
		inline mat2(const mat2& m) { for (int i = 0; i < 4; i++) { a[i] = m[i]; } }

		/// <summary>
		/// ����� ������ķ� �ٲߴϴ�.
		/// </summary>
		inline void toI() { _11 = _22 = 1; _12 = _21 = 0; }

		/// <summary>
		/// �ٸ� ��İ� ���к��� ���ϰų� ���ϴ�.
		/// </summary>
		inline mat2& operator+=(const mat2& m) { for (int i = 0; i < 4; i++)a[i] += m[i]; return *this; }
		inline mat2& operator-=(const mat2& m) { for (int i = 0; i < 4; i++)a[i] -= m[i]; return *this; }
		inline mat2 operator+(const mat2& m) const { auto r = mat2(*this); r += m; return r; }
		inline mat2 operator-(const mat2& m) const { auto r = mat2(*this); r -= m; return r; }

		/// <summary>
		/// ��İ��� �����մϴ�.
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
		/// ���Ϳ� ������ȯ�� �����Ͽ� �����մϴ�.
		/// </summary>
		inline vec2 operator*(const vec2& v) const { return vec2(_11 * v.x + _12 * v.y, _21 * v.x + _22 * v.y); }

		/// <summary>
		/// ��Ŀ� �Ǽ��踦 �մϴ�.
		/// </summary>
		inline mat2& operator*=(float f) { for (int i = 0; i < 4; i++) a[i] *= f; return *this; }
		inline mat2 operator*(float f) const { mat2 r(*this); r *= f; return r; }
		inline mat2& operator/=(float f) { for (int i = 0; i < 4; i++) a[i] /= f; return *this; }
		inline mat2 operator/(float f) const { mat2 r(*this); r /= f; return r; }

		/// <summary>
		/// ��Ľ��� ��ȯ�մϴ�.
		/// </summary>
		inline float det() const { return _11 * _22 - _12 * _21; }

		/// <summary>
		/// ������� ��ȯ�մϴ�.
		/// </summary>
		inline mat2 inverse() const {
			float d = det();
#ifdef DEBUG
			if (d == 0) printf("%p: �� ����� ������� ���ų� �ſ� ū ������ �����ϴ�. NaN�� ���� ����ġ ���� ������ �߻��� �� �ֽ��ϴ�.\n", this);
#endif // DEBUG
			return mat2(_22, -_12, -_21, _11) / d;
		}

		/// <summary>
		/// ��ġ ����� ��ȯ�մϴ�.
		/// </summary>
		inline mat2 transpose() const { return mat2(_11, _21, _12, _22); }

		/// <summary>
		/// �� �켱 ������ �� �迭�� �����մϴ�.
		/// </summary>
		inline operator float* () { return a; }
	};

	/// <summary>
	/// 3���� ���� ȸ������ Ȥ�� 2���� �̹����� ���� ��ȯ�� ���� 3x3 ����Դϴ�.
	/// </summary>
	struct mat3 {
		union { float a[9]; struct { float _11, _12, _13, _21, _22, _23, _31, _32, _33; }; };

		/// <summary>
		/// ��������� �����մϴ�.
		/// </summary>
		inline mat3() { memset(a, 0, sizeof(a)); _11 = _22 = _33 = 1; }

		/// <summary>
		/// �� �켱 ������ �Ű������� �־� ����� �����մϴ�.
		/// </summary>
		inline mat3(float _11, float _12, float _13, float _21, float _22, float _23, float _31, float _32, float _33) :_11(_11), _12(_12), _13(_13), _21(_21), _22(_22), _23(_23), _31(_31), _32(_32), _33(_33) { }

		/// <summary>
		/// ����� ������ �����ؼ� �����մϴ�.
		/// </summary>
		inline mat3(const mat3& m) { memcpy(a, m.a, sizeof(a)); }

		/// <summary>
		/// �ε��� ������
		/// </summary>
		inline float& operator[](ptrdiff_t i) { return a[i]; }
		inline const float& operator[](ptrdiff_t i) const { return a[i]; }

		/// <summary>
		/// ����� ������ķ� �ٲߴϴ�.
		/// </summary>
		inline void toI() { memset(a, 0, sizeof(a)); _11 = _22 = _33 = 1; }

		/// <summary>
		/// �ٸ� ��İ� ���к��� ���ϰų� ���ϴ�.
		/// </summary>
		inline mat3& operator+=(const mat3& m) { for (int i = 0; i < 9; i++)a[i] += m[i]; return *this; }
		inline mat3& operator-=(const mat3& m) { for (int i = 0; i < 9; i++)a[i] -= m[i]; return *this; }
		inline mat3 operator+(const mat3& m) const { auto r = mat3(*this); r += m; return r; }
		inline mat3 operator-(const mat3& m) const { auto r = mat3(*this); r -= m; return r; }

		/// <summary>
		/// n�� ���͸� �����մϴ�. 1~3�� �Է� �����մϴ�.
		/// </summary>
		/// <param name="i">�� �ε���(1 base)</param>
		inline vec3 row(int i) const { assert(i <= 3 && i >= 1); int st = 3 * i - 3; return vec3(a + st); }

		/// <summary>
		/// n�� ���͸� �����մϴ�. 1~4�� �Է� �����մϴ�.
		/// </summary>
		/// <param name="i">�� �ε���(1 base)</param>
		inline vec3 col(int i) const { assert(i <= 3 && i >= 1); return vec3(a[i - 1], a[i + 2], a[i + 5]); }

		/// <summary>
		/// ��İ��� �����մϴ�.
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
		/// ��İ��� �����մϴ�.
		/// </summary>
		inline mat3& operator*=(const mat3& m) { return *this = operator*(m); }

		/// <summary>
		/// ���Ϳ� ������ȯ�� �����Ͽ� �����մϴ�.
		/// </summary>
		inline vec3 operator*(const vec3& v) const { return vec3(row(1).dot2(v), row(2).dot2(v), row(3).dot2(v)); }

		/// <summary>
		/// ��Ŀ� �Ǽ��踦 �մϴ�.
		/// </summary>
		inline mat3& operator*=(float f) { mulAll(a, f, 9); return *this; }
		inline mat3 operator*(float f) const { mat3 r(*this); r *= f; return r; }
		inline mat3& operator/=(float f) { divAll(a, f, 9); return *this; }
		inline mat3 operator/(float f) const { mat3 r(*this); r /= f; return r; }

		/// <summary>
		/// ��Ľ��� ��ȯ�մϴ�.
		/// </summary>
		inline float det() const { return _11 * (_22 * _33 - _23 * _32) + _12 * (_23 * _31 - _21 * _33) + _13 * (_21 * _32 - _22 * _31); }

		/// <summary>
		/// ��� �밢 ������ ���� ��ȯ�մϴ�.
		/// </summary>
		inline float trace() const { return _11 + _22 + _33; }

		/// <summary>
		/// ������� ��ȯ�մϴ�.
		/// </summary>
		inline mat3 inverse() const {
			float d = det();
#ifdef DEBUG
			if (d == 0) printf("%p: �� ����� ������� ���ų� �ſ� ū ������ �����ϴ�. NaN�� ���� ����ġ ���� ������ �߻��� �� �ֽ��ϴ�.\n", this);
#endif // DEBUG
			return mat3(
				(_22 * _33 - _32 * _23), (_13 * _32 - _12 * _33), (_12 * _23 - _13 * _22),
				(_23 * _31 - _21 * _33), (_11 * _33 - _13 * _31), (_21 * _13 - _11 * _23),
				(_21 * _32 - _31 * _22), (_31 * _12 - _11 * _32), (_11 * _22 - _21 * _12)
			) / d;
		}

		/// <summary>
		/// ��ġ ����� ��ȯ�մϴ�.
		/// </summary>
		inline mat3 transpose() const { return mat3(_11, _21, _31, _12, _22, _32, _13, _23, _33); }

		/// <summary>
		/// �� �켱 ������ �� �迭�� �����մϴ�.
		/// </summary>
		inline operator float* () { return a; }

		/// <summary>
		/// ���� ��� 2x2 ��ķ� ĳ��Ʈ�մϴ�.
		/// </summary>
		inline operator mat2() { return mat2(_11, _12, _21, _22); }

		/// <summary>
		/// 2���� ���� ����� ����մϴ�.
		/// </summary>
		inline static mat3 translate(const vec2& t) {
			return mat3(
				1, 0, t.x,
				0, 1, t.y,
				0, 0, 1
			);
		}
		/// <summary>
		/// 2���� ���� ����� ����մϴ�.
		/// </summary>
		inline static mat3 translate(float x, float y) {
			return mat3(
				1, 0, x,
				0, 1, y,
				0, 0, 1
			);
		}

		/// <summary>
		/// 2���� ũ�� ��ȯ ����� ����մϴ�.
		/// </summary>
		inline static mat3 scale(const vec2& t) {
			return mat3(
				t.x, 0, 0,
				0, t.y, 0,
				0, 0, 1
			);
		}
		/// <summary>
		/// 2���� ũ�� ��ȯ ����� ����մϴ�.
		/// </summary>
		inline static mat3 scale(float x, float y) {
			return mat3(
				x, 0, 0,
				0, y, 0,
				0, 0, 1
			);
		}

		/// <summary>
		/// Z�� ������ 2���� ȸ���� �����մϴ�. X, Y �� ������ ���ϴ� ��� 3x3 ���� ��ȯ�� �ƴ� 4x4 ���� ��ȯ�� ����ؾ� �մϴ�.
		/// </summary>
		inline static mat3 rotate(float z) {
			return mat3(
				cosf(z), -sinf(z), 0,
				sinf(z), cosf(z), 0,
				0, 0, 1
			);
		}

		/// <summary>
		/// 3���� ȸ���� �����մϴ�.
		/// </summary>
		/// <param name="roll">roll(X�� ���� ȸ��)</param>
		/// <param name="pitch">pitch(Y�� ���� ȸ��)</param>
		/// <param name="yaw">yaw(Z�� ���� ȸ��)</param>
		inline static mat3 rotate(float roll, float pitch, float yaw);
	};

	/// <summary>
	/// 3���� ���� ���� ��ȯ�� ���� 4x4 ����Դϴ�.
	/// </summary>
	struct mat4 {
		union { float a[16]; struct { float _11, _12, _13, _14, _21, _22, _23, _24, _31, _32, _33, _34, _41, _42, _43, _44; }; };

		/// <summary>
		/// ��������� �����մϴ�.
		/// </summary>
		inline mat4() { memset(a, 0, sizeof(a)); _11 = _22 = _33 = _44 = 1; }

		/// <summary>
		/// �� �켱 ������ �Ű������� �־� ����� �����մϴ�.
		/// </summary>
		inline mat4(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44) :_11(_11), _12(_12), _13(_13), _14(_14), _21(_21), _22(_22), _23(_23), _24(_24), _31(_31), _32(_32), _33(_33), _34(_34), _41(_41), _42(_42), _43(_43), _44(_44) { }

		/// <summary>
		/// ����� ������ �����ؼ� �����մϴ�.
		/// </summary>
		inline mat4(const mat4& m) { memcpy(a, m.a, sizeof(a)); }

		/// <summary>
		/// �ε��� ������
		/// </summary>
		inline float& operator[](ptrdiff_t i) { return a[i]; }
		inline const float& operator[](ptrdiff_t i) const { return a[i]; }

		/// <summary>
		/// ����� ������ķ� �ٲߴϴ�.
		/// </summary>
		inline void toI() { memset(a, 0, sizeof(a)); _11 = _22 = _33 = _44 = 1; }

		/// <summary>
		/// ��������̸� true�� �����մϴ�.
		/// </summary>
		inline bool isI() const { return memcmp(mat4().a, a, sizeof(a)) == 0; }

		/// <summary>
		/// �ٸ� ��İ� ���к��� ���ϰų� ���ϴ�.
		/// </summary>
		inline mat4& operator+=(const mat4& m) { add4<float>(a, m.a); add4<float>(a + 4, m.a + 4); add4<float>(a + 8, m.a + 8); add4<float>(a + 12, m.a + 12); return *this; }
		inline mat4& operator-=(const mat4& m) { sub4<float>(a, m.a); sub4<float>(a + 4, m.a + 4); sub4<float>(a + 8, m.a + 8); sub4<float>(a + 12, m.a + 12); return *this; }
		inline mat4 operator+(const mat4& m) const { auto r = mat4(*this); r += m; return r; }
		inline mat4 operator-(const mat4& m) const { auto r = mat4(*this); r -= m; return r; }

		/// <summary>
		/// n�� ���͸� �����մϴ�. 1~4�� �Է� �����մϴ�.
		/// </summary>
		/// <param name="i">�� �ε���(1 base)</param>
		inline vec4 row(int i) const { assert(i <= 4 && i >= 1); int st = 4 * i - 4; vec4 ret; memcpy(ret.entry, a + st, sizeof(ret.entry)); return ret; }

		/// <summary>
		/// n�� ���͸� �����մϴ�. 1~4�� �Է� �����մϴ�.
		/// </summary>
		/// <param name="i">�� �ε���(1 base)</param>
		inline vec4 col(int i) const { assert(i <= 4 && i >= 1); return vec4(a[i - 1], a[i + 3], a[i + 7], a[i + 11]); }

		/// <summary>
		/// ��İ��� �����մϴ�.
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
		/// ��İ��� �����մϴ�.
		/// </summary>
		inline mat4& operator*=(const mat4& m) { return *this = operator*(m); }

		/// <summary>
		/// ���Ϳ� ������ȯ�� �����Ͽ� �����մϴ�.
		/// </summary>
		inline vec4 operator*(const vec4& v) const {
			return vec4(row(1).dot2(v), row(2).dot2(v), row(3).dot2(v), row(4).dot2(v));
		}

		/// <summary>
		/// ��Ŀ� �Ǽ��踦 �մϴ�.
		/// </summary>
		inline mat4& operator*=(float f) { mulAll(a, f, 16); return *this; }
		inline mat4 operator*(float f) const { mat4 r(*this); r *= f; return r; }
		inline mat4& operator/=(float f) { divAll(a, f, 16); return *this; }
		inline mat4 operator/(float f) const { mat4 r(*this); r /= f; return r; }

		/// <summary>
		/// ��Ľ��� ��ȯ�մϴ�.
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
		/// ����� �밢�� ���� ���� �����մϴ�.
		/// </summary>
		inline float trace() const { return _11 + _22 + _33 + _44; }

		/// <summary>
		/// ���� ��� 3x3 ��ķ� ĳ��Ʈ�մϴ�.
		/// </summary>
		inline operator mat3() const { return mat3(_11, _12, _13, _21, _22, _23, _31, _32, _33); }

		/// <summary>
		/// ���� ��ȯ�� ������� ���� �� ȿ�������� ���մϴ�.
		/// ����: mat4::iTRS()
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
		/// ������� ��ȯ�մϴ�.
		/// </summary>
		inline mat4 inverse() const {
			float d = det();
#ifdef DEBUG
			if (d == 0) printf("%p: �� ����� ������� ���ų� �ſ� ū ������ �����ϴ�. NaN�� ���� ����ġ ���� ������ �߻��� �� �ֽ��ϴ�.\n", this);
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
		/// ��ġ ����� ��ȯ�մϴ�.
		/// </summary>
		inline mat4 transpose() const { return mat4(_11, _21, _31, _41, _12, _22, _32, _42, _13, _23, _33, _43, _14, _24, _34, _44); }

		/// <summary>
		/// �� �켱 ������ �� �迭�� �����մϴ�.
		/// </summary>
		inline operator float* () { return a; }
		inline operator const float* () const { return a; }

		/// <summary>
		/// 3���� ���� ����� ����մϴ�.
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
		/// 3���� ���� ����� ����մϴ�.
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
		/// 3���� ũ�� ��ȯ ����� ����մϴ�.
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
		/// 3���� ũ�� ��ȯ ����� ����մϴ�.
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
		/// 3���� ȸ�� ����� ����մϴ�.
		/// </summary>
		inline static mat4 rotate(const vec3& axis, float angle);

		/// <summary>
		/// 3���� ȸ�� ����� ����մϴ�. cf) ������ ��Ģ
		/// </summary>
		/// <param name="roll">roll(X�� ���� ȸ��)</param>
		/// <param name="pitch">pitch(Y�� ���� ȸ��)</param>
		/// <param name="yaw">yaw(Z�� ���� ȸ��)</param>
		inline static mat4 rotate(float roll, float pitch, float yaw);

		/// <summary>
		/// 3���� ȸ�� ������� ��� ���·� �����մϴ�.
		/// </summary>
		inline static mat4 rotate(const Quaternion& q);

		/// <summary>
		/// lookAt ������ �� ����� ����մϴ�.
		/// </summary>
		/// <param name="eye">���� ��ġ</param>
		/// <param name="at">�ǻ�ü ��ġ</param>
		/// <param name="up">���� ����: ȭ�� �󿡼� ���� ������ �� ������ ����� ��������ϴ�.</param>
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
		/// ����, ȸ��, ���� ��� T, R, S�� ���� �� ���ϴ� �ͺ��� ���� �� ������ ����մϴ�.
		/// </summary>
		/// <param name="translation">����</param>
		/// <param name="rotation">ȸ��</param>
		/// <param name="scale">����</param>
		inline static mat4 TRS(const vec3& translation, const Quaternion& rotation, const vec3& scale);
		/// <summary>
		/// �־��� ����, ȸ��, ������ �����ϴ� ���� ��ȯ�� ����ȯ�� �ܼ���꺸�� ���� ������ ����մϴ�. ����ȯ�� ���� ���(ex: ������ ���� ����)
		/// ���������� ���� ���ϵ� ���Դϴ�.
		/// </summary>
		/// <param name="translation">����</param>
		/// <param name="rotation">ȸ��</param>
		/// <param name="scale">����</param>
		inline static mat4 iTRS(const vec3& translation, const Quaternion& rotation, const vec3& scale);
		/// <summary>
		/// ǥ�� �� ���� ������ü�� ���� ��� �Դ�(����ü)�� �����ϴ� ���� ����� ����մϴ�.
		/// ���� 2D ������ ����� ���, ���� ��Ŀ� aspect�� �����ϸ� �˴ϴ�.
		/// </summary>
		/// <param name="fovy">field of view Y: �Դ��� Y�� ����(ȭ�� ���� ����) ���� �����Դϴ�.</param>
		/// <param name="aspect">ǥ�� ����Ʈ ����(����/����)�Դϴ�.</param>
		/// <param name="dnear">�Դ뿡�� ���� ����� �Ÿ��Դϴ�. �� ������ �Ÿ��� ���� ������ �ڿ� �ִ� ������ Ĩ�ϴ�.</param>
		/// <param name="dfar">�Դ뿡�� ���� �� �Ÿ��Դϴ�. �� �̻��� �Ÿ��� ������ �ʽ��ϴ�.</param>
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
		/// �� ���簢���� �ٸ� ���簢������ ��ȯ�ϴ� ����� ����մϴ�. ���簢���� ������ ��-��-��-�����Դϴ�. z ��ǥ�� �����ϴٰ� �����Ͽ� xy ��鿡���� �̵��մϴ�.
		/// </summary>
		/// <param name="r1">��ȯ �� ���簢��</param>
		/// <param name="r2">��ȯ �� ���簢��</param>
		/// <param name="z">���簢���� ��ġ�� z��ǥ(-1�� ���� ��)</param>
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
		/// �� �������� Mesh::get("rect")�� �����Ǵ� ���� ���簢��(�߽��� 0,0�̰� �� ���� ���̰� 1�� ���簢��)�� �ٸ� ���簢������ ��ȯ�ϴ� ����� ����մϴ�.
		/// ���簢���� ������ ��-��-��-�����Դϴ�. z ��ǥ�� �����ϴٰ� �����Ͽ� xy ��鿡���� �̵��մϴ�.
		/// </summary>
		/// <param name="r2">��ȯ �� ���簢��</param>
		/// <param name="z">���簢���� ��ġ�� z��ǥ(-1�� ���� ��)</param>
		inline static mat4 r2r(const vec4& r2, float z = 0) {
			return r2r(vec4(-0.5f, -0.5f, 1, 1), r2, z);
		}

		/// <summary>
		/// �� ���簢��(L-D-W-H ����)�� �ٸ� ���簢���� ���ʿ� �°� ��ȯ�մϴ�. �� �߽��� �����ϸ�, ���� ���簢���� ��Ⱦ��� �����ϸ鼭 ���� ū ���簢���� �ǵ��� �����մϴ�.
		/// </summary>
		/// <param name="r1">��ȯ �� ���簢��</param>
		/// <param name="r2">��ȯ �� ���簢��</param>
		/// <param name="z">���簢���� ��ġ�� z��ǥ(-1�� ���� ��)</param>
		inline static mat4 r2r2(const vec4& r1, const vec4& r2, float z = 0) {
			float r = r1.width / r1.height;
			vec4 targ(r2);
			if (targ.width < targ.height * r) {	// ���μ��� ���� ��
				targ.down += (targ.height - targ.width / r) / 2;
				targ.height = targ.width / r;
			}
			else {	// ���μ��� ���� ��
				targ.left += (targ.width - targ.height * r) / 2;
				targ.width = targ.height * r;
			}
			return r2r(r1, targ, z);
		}
	};

	/// <summary>
	/// ��� Ŭ������ �Ϲ�ȭ�Դϴ�. �� �켱 �����̸� ���簢 ��� mat2, mat3, mat4�� �ٸ��� �⺻���� ���길 �����˴ϴ�. ����� �� ���� ������ ���Ǹ� �̿ܿ��� ���� �ʱ�ȭ�� �����մϴ�.
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
		/// r�� ���͸� �����մϴ�. 1~(�� ��) ���� �Է¿� ���� ����� ���ǵ��� �ʾҽ��ϴ�.
		/// </summary>
		inline nvec<C, float> row(unsigned r) const { assert(r >= 1 && r <= R); return nvec<C, float>(entry[r - 1]); }
		/// <summary>
		/// c�� ���͸� �����մϴ�. 1~(�� ��) ���� �Է¿� ���� ����� ���ǵ��� �ʾҽ��ϴ�.
		/// </summary>
		inline nvec<R, float> col(unsigned c) const { assert(c >= 1 && c <= C);	nvec<R, float> ret;	for (unsigned i = 0; i < R; i++)ret[i] = entry[i][c - 1]; return ret; }
		/// <summary>
		/// �����͸� ����� ������ ���մϴ�.
		/// �� ����� �ٸ� ���� ���Ϳ��� �Ͻ��� �� ��ȯ�� ����ϹǷ� �ùٸ� ������ ����ϵ��� �����ϼ���.
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
		/// ��ġ ����� �����մϴ�.
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
		/// �־��� �ٸ� ����� ������ ���� ����� �����մϴ�.
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
		/// ���翪����� ���մϴ�. ������ ������� �����մϴ�.
		/// </summary>
		inline mat<C, R> pseudoInverse() const {
			mat<C, R> ret;
			// TODO: �� ���� �� �� ����
			return ret;
		}
	};

	using mat2x3 = mat<2, 3>;	using mat3x2 = mat<3, 2>;

	/// <summary>
	/// 3���� ȸ�� ���� ǥ���ϴ� ������Դϴ�. 1, i, j, k �κп� �ش��ϴ� c1, ci, cj, ck ����� �����ϴ�.
	/// </summary>
	struct Quaternion {
		union {
			struct { float c1, ci, cj, ck; };
			struct { float w, x, y, z; };
		};

		/// <summary>
		/// ������� �����մϴ�.
		/// </summary>
		/// <param name="o">�Ǽ��κ�</param>
		/// <param name="i">i�κ�</param>
		/// <param name="j">j�κ�</param>
		/// <param name="k">k�κ�</param>
		inline Quaternion(float o = 1, float i = 0, float j = 0, float k = 0) :c1(o), ci(i), cj(j), ck(k) {};

		/// <summary>
		/// ���ӵ� ����(�� ũ�Ⱑ �ʴ� ȸ����, ������ ȸ������ ����)�� �����ϴ� ������� �����մϴ�.
		/// </summary>
		inline Quaternion(const vec3& av) :c1(0) { memcpy(&ci, av.entry, sizeof(float) * 3); }

		/// <summary>
		/// ������� �����ؼ� �����մϴ�.
		/// </summary>
		inline Quaternion(const Quaternion& q) { set4<float>(&c1, &(q.c1)); }

		/// <summary>
		/// ����� ũ���� ������ �����մϴ�.
		/// </summary>
		inline float abs2() const { return reinterpret_cast<const vec4*>(this)->length2(); }

		/// <summary>
		/// ����� ũ�⸦ �����մϴ�.
		/// </summary
		inline float abs() const { return sqrtf(abs2()); }

		/// <summary>
		/// ������� ��ȸ������ Ȯ���մϴ�.
		/// </summary>
		inline bool is1() const { return c1 == 1 && ci == 0 && cj == 0 && ck == 0; }

		/// <summary>
		/// �ӷ�(����)������� �����մϴ�.
		/// </summary>
		inline Quaternion conjugate() const { return Quaternion(c1, -ci, -cj, -ck); }

		/// <summary>
		/// ������� ������ ���ϸ� 1�� �Ǵ� ���� �����մϴ�.
		/// </summary>
		inline Quaternion inverse() const { return conjugate() / abs2(); }

		/// <summary>
		/// ����� ���� �������Դϴ�.
		/// </summary>
		inline Quaternion operator*(const Quaternion& q) const {
			Quaternion q_c1 = q * c1;
			Quaternion q_ci = Quaternion(-q.ci, q.c1, -q.ck, q.cj) * ci;
			Quaternion q_cj = Quaternion(-q.cj, q.ck, q.c1, -q.ci) * cj;
			Quaternion q_ck = Quaternion(-q.ck, -q.cj, q.ci, q.c1) * ck;
			return q_c1 + q_ci + q_cj + q_ck;
		}

		/// <summary>
		/// ����� �� ��Ģ �����Դϴ�. ��� ������ �� ������� �������� ������ ����˴ϴ�.
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
		/// �� ������� ȸ�� ����� ������ �����մϴ�.
		/// </summary>
		inline Quaternion normal() const { return (*this) * (1 / abs()); }

		/// <summary>
		/// �� ������� ������ 1�� ����ϴ�.
		/// </summary>
		inline void normalize() { operator*=(1 / abs()); }

		/// <summary>
		/// ������� ��ȣ�� �ݴ�� �մϴ�. 180�� ������ �Ͱ� �����մϴ�.
		/// </summary>
		inline Quaternion operator-() const { return Quaternion(-c1, -ci, -cj, -ck); }

		/// <summary>
		/// ����� ȸ���� ��Ĩ�ϴ�. ���� ������� ���� ����Ǹ� ũ�� 1���� Ȯ������ �ʽ��ϴ�.
		/// </summary>
		inline void compound(const Quaternion& q) { *this = q * (*this); }

		/// <summary>
		/// ����� ȸ���� ��Ĩ�ϴ�. ���� ������� ���� ����˴ϴ�.
		/// </summary>
		/// <param name="axis">ȸ����</param>
		/// <param name="angle">ȸ����(����)</param>
		inline void compound(const vec3& axis, float angle) { auto q = rotation(axis, angle); compound(q); }

		/// <summary>
		/// ������� ȸ�� ��ķ� �����մϴ�.
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
		/// ù ���п� ȸ����(����), ������ ���п� 3���� ȸ������ ��� �����մϴ�.
		/// �ε��Ҽ��� ���е� ������ ����Ͽ� ����ȭ�Ͽ� ����մϴ�. ȸ��������� �ƴ϶� nan�� �߻����� �����Ƿ� �����ϼ���.
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
		/// �� ȸ���� ���Ϸ� �� (x,y,z��)�� ���·� �����մϴ�.
		/// �ε��Ҽ��� ���е� ������ ����Ͽ� ����ȭ�Ͽ� ����մϴ�. ȸ��������� �ƴ϶� nan�� �߻����� �����Ƿ� �����ϼ���.
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
		/// ��� ���� �־����� �� ȸ������ �����մϴ�.
		/// ȸ������ �ڵ����� ����ȭ�˴ϴ�.
		/// </summary>
		/// <param name="axis">ȸ����</param>
		/// <param name="angle">ȸ����(����)</param>
		inline static Quaternion rotation(const vec3& axis, float angle) {
			angle *= 0.5f;
			float c = cosf(angle), s = sinf(angle);
			vec3 nv = axis.normal() * s;
			return Quaternion(c, nv.x, nv.y, nv.z);
		}

		/// <summary>
		/// ��� ���� �־����� �� ȸ������ �����մϴ�.
		/// �Էµ� ȸ������ ����ȭ���� �ʽ��ϴ�.
		/// </summary>
		/// <param name="uaxis">ȸ����(��������)</param>
		/// <param name="angle">ȸ����(����)</param>
		/// <returns></returns>
		inline static Quaternion rotationByUnit(const vec3& uaxis, float angle) {
			angle *= 0.5f;
			float c = cosf(angle), s = sinf(angle);
			vec3 nv(uaxis); nv *= s;
			return Quaternion(c, nv.x, nv.y, nv.z);
		}

		/// <summary>
		/// �־��� ������ �ٶ󺸵��� �ϴ� ȸ�� ������� �����մϴ�.
		/// </summary>
		/// <param name="after">���ϴ� ����(ȸ������ �ƴմϴ�.)</param>
		/// <param name="after">���� ����(ȸ������ �ƴմϴ�.)</param>
		inline static Quaternion direction(const vec3& after, const vec3& before) {
			vec3 mid(after);
			mid *= sqrtf(before.length2() / after.length2());
			mid += before;
			mid /= mid.length();
			return Quaternion(0, mid.x, mid.y, mid.z);
		}

		/// <summary>
		/// �־��� ������ �ٶ󺸵��� �ϴ� ȸ�� ������� �����մϴ�.
		/// �Է� ���͵��� ���̴� �ݵ�� �����ؾ� �մϴ�.
		/// </summary>
		/// <param name="after">���ϴ� ����(ȸ������ �ƴմϴ�.)</param>
		/// <param name="after">���� ����(ȸ������ �ƴմϴ�.)</param>
		inline static Quaternion directionByUnit(const vec3& after, const vec3& before) {
			vec3 mid(after);
			mid += before;
			mid /= mid.length();
			return Quaternion(0, mid.x, mid.y, mid.z);
		}

		/// <summary>
		/// ���Ϸ� ȸ���� �ش��ϴ� ������� �����մϴ�. cf) ������ ��Ģ
		/// <para>https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles</para>
		/// </summary>
		/// <param name="roll">roll(X�� ���� ȸ��)</param>
		/// <param name="yaw">yaw(Z�� ���� ȸ��)</param>
		/// <param name="pitch">pitch(Y�� ���� ȸ��)</param>
		inline static Quaternion euler(float roll, float pitch, float yaw) {
			float cy = cosf(yaw / 2);	float sy = sinf(yaw / 2);
			float cp = cosf(pitch / 2);	float sp = sinf(pitch / 2);
			float cr = cosf(roll / 2);	float sr = sinf(roll / 2);

			return Quaternion(cr * cp * cy + sr * sp * sy, sr * cp * cy - cr * sp * sy, cr * sp * cy + sr * cp * sy, cr * cp * sy - sr * sp * cy);
		}

	};

	/// <summary>
	/// ������� ���� ������ �����մϴ�.
	/// </summary>
	/// <param name="q1">���� ���� ��� 1(t=0�� �������� �̰Ϳ� �������ϴ�.)</param>
	/// <param name="q2">���� ���� ��� 2(t=1�� �������� �̰Ϳ� �������ϴ�.)</param>
	/// <param name="t">���� ���� ��</param>
	inline Quaternion lerp(const Quaternion& q1, const Quaternion& q2, float t) {
		Quaternion ret = q1 * (1 - t) + q2 * t;
		return ret / ret.abs();
	}

	/// <summary>
	/// ������� ���� ���� ������ �����մϴ�.
	/// </summary>
	/// <param name="q1">���� ���� ��� 1(t=0�� �������� �̰Ϳ� �������ϴ�.)</param>
	/// <param name="q2">���� ���� ��� 2(t=1�� �������� �̰Ϳ� �������ϴ�.)</param>
	/// <param name="t">���� ���� ��</param>
	inline Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t) {
		float Wa, Wb;
		float costh = reinterpret_cast<const vec4*>(&q1)->dot(*reinterpret_cast<const vec4*>(&q2)) / q1.abs() / q2.abs();
		// ���е� ������ ���� nan ����
		if (costh > 1) costh = 1;
		else if (costh < -1) costh = -1;
		float theta = acos(costh);
		float sn = sin(theta);

		// q1=q2�̰ų� 180�� ������ ���
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
		// SIMD ������ �� �� 30ȸ/�� 6ȸ, T*R*S ���� �ϴ� ��� �� 149ȸ/�� 102ȸ
		// SIMD ���� �� �� 15ȸ/�� 6ȸ, ���� �ϴ� ��� �� 44ȸ/�� 102ȸ
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
		// SIMD ���� �� �� 19ȸ/�� 18ȸ
		mat4 r = rotation.conjugate().toMat4();	// ���׻����=��ȸ��
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

	// ������ �߰� �����ε�
	template<unsigned D, class T>inline nvec<D, T> operator+(float f, const nvec<D, T>& v) { return v + f; }
	template<unsigned D, class T>inline nvec<D, T> operator*(float f, const nvec<D, T>& v) { return v * f; }
	inline mat4 operator*(float f, const mat4& m) { return m * f; }
	inline Quaternion operator*(float f, const Quaternion& q) { return q * f; }

	/// <summary>
	/// 3x2 ����� ���翪����� �����մϴ�. ������ ������� �����մϴ�.
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
	/// �ִ�/�ּ� ������ �ξ� �ڸ� ���� �����մϴ�. ù ���� ��������Ǵ� �⺻�ڷ����� �ƴ� ���� ����� ���� �����ϼ���.
	/// </summary>
	/// <param name="t">�ڸ��� �� ��</param>
	/// <param name="min">�ּڰ�</param>
	/// <param name="max">�ִ�</param>
	/// <returns></returns>
	template<class T>inline T clamp(T t, const T& min, const T& max) {
		t = t < min ? min : t;
		t = t > max ? max : t;
		return t;
	};

	/// <summary>
	/// 2���� ��鿡�� 2�� ������ ������ �����մϴ�.
	/// ������ ���� ��� (nan, nan)�� ���ϵ˴ϴ�.
	/// ������ ��� ���� ���ο� �����ϰ� (nan, 0)�� ���ϵ˴ϴ�.
	/// https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection#Given_two_points_on_each_line_segment
	/// </summary>
	/// <param name="p1">���� 1�� ���� 1</param>
	/// <param name="p2">���� 1�� ���� 2</param>
	/// <param name="q1">���� 2�� ���� 1</param>
	/// <param name="q2">���� 2�� ���� 2</param>
	inline vec2 intersect(const vec2& p1, const vec2& p2, const vec2& q1, const vec2& q2) {
		// cf) n�� �� �����ϴ� ���� https://www.geeksforgeeks.org/given-a-set-of-line-segments-find-if-any-two-segments-intersect/
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
	/// 2���� ��鿡�� 2�� ������ �������� ���θ� Ȯ���մϴ�.
	/// </summary>
	/// <param name="p1">���� 1�� ���� 1</param>
	/// <param name="p2">���� 1�� ���� 2</param>
	/// <param name="q1">���� 2�� ���� 1</param>
	/// <param name="q2">���� 2�� ���� 2</param>
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
	/// 3���� ���� ���� ���� ���� ��� �� �ﰢ�� �ȿ� �ִ��� Ȯ���մϴ�.
	/// �ﰢ���� ���� ���� ��� ���� ���� ���� ��쿡 ���� ����� ������ ���ǵ��� �ʾҽ��ϴ�.
	/// </summary>
	/// <param name="p">��</param>
	/// <param name="t1">�ﰢ�� ������ 1</param>
	/// <param name="t2">�ﰢ�� ������ 2</param>
	/// <param name="t3">�ﰢ�� ������ 3</param>
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
		// pseudo inverse �ʿ�.
	}

	/// <summary>
	/// ���� ����׸� ���� �� ��� �Լ��Դϴ�.
	/// </summary>
	/// <param name="v">ǥ���� ����</param>
	/// <param name="tag">�̸�</param>
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