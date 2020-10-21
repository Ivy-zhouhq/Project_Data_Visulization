#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

// A very modest transform object class
struct Transform
{
	static Transform Mix(const Transform& t0, const Transform& t1, float t);
	static Transform LookAt(const glm::vec3& eye, const glm::vec3& point, const glm::vec3& up = glm::vec3(0, 1, 0));

	Transform(const glm::vec3& pos = glm::vec3(0.0f), const glm::quat& rot = glm::quat(), glm::vec3 scl = glm::vec3(1.0f));
	Transform(const Transform& other) = default;
	Transform(const glm::mat3& m);
	Transform(const glm::mat4& m);

	void Translate(const glm::vec3& v);
	void Move(const glm::vec3& v);
	void Rotate(const glm::quat& q);
	void Rotate(const glm::vec3& euler);

	glm::mat4 Matrix() const;
	glm::mat4 InvMatrix() const;
	glm::vec3 EulerAngles() const;

	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 scale;
};

inline Transform::Transform(const glm::vec3& trans, const glm::quat& rot, glm::vec3 scl) :
	rotation(rot), translation(trans), scale(scl) {}

inline Transform::Transform(const glm::mat3& m)
{
	rotation = glm::quat_cast(m);
	translation = glm::vec3(m[3]);
	scale = glm::vec3(glm::length(m[0]), glm::length(m[1]), glm::length(m[2]));
}

inline Transform::Transform(const glm::mat4& m)
{
	rotation = glm::quat_cast(m);
	translation = glm::vec3(m[3]);
	scale = glm::vec3(glm::length(glm::vec3(m[0])),
					  glm::length(glm::vec3(m[1])),
					  glm::length(glm::vec3(m[2])));
}

inline void Transform::Translate(const glm::vec3& v)
{
	translation += v;
}

inline void Transform::Move(const glm::vec3& v)
{
	translation += glm::mat3_cast(rotation) * v;
}

inline void Transform::Rotate(const glm::quat& q)
{
	rotation = rotation * q;
}

inline void Transform::Rotate(const glm::vec3& euler)
{
	rotation = rotation * glm::quat(glm::vec3(euler.y, euler.x, euler.z));
}

inline glm::mat4 Transform::Matrix() const
{
	const glm::mat4 T = glm::translate(glm::mat4(), translation);
	const glm::mat4 R = glm::mat4_cast(rotation);
	const glm::mat4 S = glm::scale(glm::mat4(), scale);
	return T * R * S;
}

inline glm::mat4 Transform::InvMatrix() const
{
	//assert(glm::all(glm::greaterThan(glm::abs(scale), vec3(0.001f))));
	const glm::mat4 T = glm::translate(glm::mat4(), -translation);
	const glm::mat4 R = glm::mat4_cast(glm::conjugate(rotation));
	const glm::mat4 S = glm::scale(glm::mat4(), 1.0f / scale);
	return S * R * T;
}

inline glm::vec3 Transform::EulerAngles() const
{
	return glm::eulerAngles(rotation);
}

inline Transform Transform::Mix(const Transform& t0, const Transform &t1, float t)
{
	glm::clamp(t, 0.0f, 1.0f);
	return Transform(glm::mix(t0.translation, t1.translation, t),
					 glm::mix(t0.rotation, t1.rotation, t),
					 glm::mix(t0.scale, t1.scale, t));
}

inline Transform Transform::LookAt(const glm::vec3& eye, const glm::vec3& point, const glm::vec3& up)
{
	glm::quat q = glm::conjugate(glm::quat_cast(glm::lookAt(eye, point, up)));
	return Transform(eye, q);
}