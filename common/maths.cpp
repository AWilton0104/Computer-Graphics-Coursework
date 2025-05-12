#include <common/maths.hpp>

glm::mat4 Maths::translate(const glm::vec3& v) {
    glm::mat4 translate(1.0f);
    translate[3][0] = v.x, translate[3][1] = v.y, translate[3][2] = v.z;
    return translate;
}

glm::mat4 Maths::scale(const glm::vec3& v) {
    glm::mat4 scale(1.0f);
    scale[0][0] = v.x, scale[1][1] = v.y, scale[2][2] = v.z;
    return scale;
}

float Maths::radians(float degrees) {
    return degrees * 3.141592165358979f / 180.0f;
}

glm::mat4 Maths::rotate(const float angle, glm::vec3& v)
{
    v = Maths::normalise(v);
    float c = cos(0.5f * angle);
    float s = sin(0.5f * angle);
    Quaternion q(c, s * v.x, s * v.y, s * v.z);

    return q.matrix();
}

//Students own implementation of GLM functions 

float Maths::magnitude(const glm::vec3& v)
{
    return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

float Maths::dot(const glm::vec3& v1, const glm::vec3& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

glm::vec3 Maths::cross(const glm::vec3& v1, const glm::vec3& v2)
{
    return glm::vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

glm::vec3 Maths::normalise(const glm::vec3& v)
{
    float mag = Maths::magnitude(v);
    return glm::vec3(v.x / mag, v.y / mag, v.z / mag);
}

glm::mat4 Maths::transpose(const glm::mat4 m) 
{
    return glm::mat4(m[0][0],m[1][0],m[2][0],m[3][0],m[0][1],m[1][1],m[2][1],m[3][1],m[0][2],m[1][2],m[2][2],m[3][3],m[0][3],m[1][3],m[2][3],m[3][3]);
}

//Students own implementation of view and projection matricies 

glm::mat4 Maths::lookAt(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& worldUp)
{
    glm::vec3 front = Maths::normalise(target - eye);
    glm::vec3 right = Maths::normalise(Maths::cross(front, worldUp));
    glm::vec3 up = Maths::cross(right, front);
    return glm::mat4(right.x, up.x, -front.x, 0, right.y, up.y, -front.y, 0, right.z, up.z, -front.z, 0, -Maths::dot(eye, right), -Maths::dot(eye, up), Maths::dot(eye, front), 1);
}

glm::mat4 Maths::perspective(float fov, float aspect, float near, float far)
{
    float top = near * tan(fov / 2);
    float right = aspect * top;
    return glm::mat4(near / right, 0, 0, 0, 0, near / top, 0, 0, 0, 0, -(far + near) / (far - near), -1, 0, 0, -(2 * far * near) / (far - near), 0);
}

// Quaternions
Quaternion::Quaternion() 
{
    this->w = 0.0f;
    this->x = 0.0f;
    this->y = 0.0f;
    this->z = 0.0f;
}

Quaternion::Quaternion(const float w, const float x, const float y, const float z)
{
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}

Quaternion::Quaternion(const float pitch, const float yaw)
{
    float cosPitch = cos(0.5f * pitch);
    float sinPitch = sin(0.5f * pitch);
    float cosYaw = cos(0.5f * yaw);
    float sinYaw = sin(0.5f * yaw);

    this->w = cosPitch * cosYaw;
    this->x = sinPitch * cosYaw;
    this->y = cosPitch * sinYaw;
    this->z = sinPitch * sinYaw;
}

glm::mat4 Quaternion::matrix()
{
    float s = 2.0f / (w * w + x * x + y * y + z * z);
    float xs = x * s, ys = y * s, zs = z * s;
    float xx = x * xs, xy = x * ys, xz = x * zs;
    float yy = y * ys, yz = y * zs, zz = z * zs;
    float xw = w * xs, yw = w * ys, zw = w * zs;

    glm::mat4 rotate;
    rotate[0][0] = 1.0f - (yy + zz);
    rotate[0][1] = xy + zw;
    rotate[0][2] = xz - yw;
    rotate[1][0] = xy - zw;
    rotate[1][1] = 1.0f - (xx + zz);
    rotate[1][2] = yz + xw;
    rotate[2][0] = xz + yw;
    rotate[2][1] = yz - xw;
    rotate[2][2] = 1.0f - (xx + yy);

    return rotate;
}

// SLERP
Quaternion Maths::SLERP(Quaternion q1, Quaternion q2, const float t)
{
    // Calculate cos(theta)
    float cosTheta = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;

    // If q1 and q2 are close together return q2 to avoid divide by zero errors
    if (cosTheta > 0.9999f)
        return q2;

    // Avoid taking the long path around the sphere by reversing sign of q2
    if (cosTheta < 0)
    {
        q2 = Quaternion(-q2.w, -q2.x, -q2.y, -q2.z);
        cosTheta = -cosTheta;
    }

    // Calculate SLERP
    Quaternion q;
    float theta = acos(cosTheta);
    float a = sin((1.0f - t) * theta) / sin(theta);
    float b = sin(t * theta) / sin(theta);
    q.w = a * q1.w + b * q2.w;
    q.x = a * q1.x + b * q2.x;
    q.y = a * q1.y + b * q2.y;
    q.z = a * q1.z + b * q2.z;

    return q;
}
