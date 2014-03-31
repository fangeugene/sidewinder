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

#endif
