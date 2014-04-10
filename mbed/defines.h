#ifndef DEFINES_H
#define DEFINES_H

const int kMaxMsgSize = 100;

// Helper Functions
template <typename T>
inline T min(T x,T y) { return (x < y) ? x : y; }
template <typename T>
inline T max(T x,T y) { return (x > y) ? x : y; }
template <typename T>
inline T abs(T x) { return (x < 0) ? -x : x; }
template <typename T>
inline T constrain(T x, T min, T max){
  return x > max ? max : (x < min ? min : x); }
template <typename T>
inline T mod(T m, T n) { return m >= 0 ? m % n : ( n - abs ( m % n ) ) % n; }
template <typename T>
inline T smaller_ang(T x) { return mod(x + 180, 360) - 180; }
template <typename T>
inline T closer_ang(T x,T a) { return a + smaller_ang(x - a); }

#endif
