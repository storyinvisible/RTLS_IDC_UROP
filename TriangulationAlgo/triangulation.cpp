#include <stdio.h>
#include <math.h>
#include <stdexcept>
#include <iostream>
#include <vector>

class MathVector {
public:
  double x;
  double y;
  double z;

  MathVector(double x1, double y1, double z1) {
    x = x1;
    y = y1;
    z = z1;
  }

  double getNorm() {
    return std::sqrt(x*x + y*y + z*z);
  }

  MathVector getVectorNorm() {
    double length = getNorm();
    return MathVector(x / length, y / length, z / length);
  }

  static MathVector add(MathVector V1, MathVector V2) {
    return MathVector(V1.x + V2.x, V1.y + V2.y, V1.z + V2.z);
  }

  static MathVector minus(MathVector V1, MathVector V2) {
    return MathVector(V1.x - V2.x, V1.y - V2.y, V1.z - V2.z);
  }

  static double dot(MathVector V1, MathVector V2) {
    return V1.x * V2.x + V1.y * V2.y + V1.z * V2.z;
  }

  MathVector multiply(double i) {
    return MathVector(x * i, y * i, z * i);
  }

  static MathVector crossProduct(MathVector V1, MathVector V2) {
    return MathVector(V1.y * V2.z - V1.z * V2.y,
      V1.z * V2.x - V1.x * V2.z,
      V1.x * V2.y - V1.y * V2.x);
  }
};

std::vector<MathVector> getVectors(MathVector P1, MathVector P2, MathVector P3,
  double r1, double r2, double r3) {

  MathVector D21 = MathVector::minus(P2, P1);
  MathVector EX = D21.getVectorNorm();
  MathVector D31 = MathVector::minus(P3, P1);
  double D31dotEX = MathVector::dot(D31, EX);
  MathVector T1 = MathVector::minus(D31, EX.multiply(D31dotEX));
  MathVector EY = T1.getVectorNorm();
  MathVector EZ = MathVector::crossProduct(EX, EY);
  double D21Norm = D21.getNorm();
  double EYdotD31 = MathVector::dot(EY, D31);
  double x = (r1 * r1 - r2 * r2 + D21Norm * D21Norm) / (2 * D21Norm);
  double y = ((r1 * r1 - r3 * r3 - 2 * D31dotEX * x 
    + D31dotEX * D31dotEX + EYdotD31 * EYdotD31)) / (2 * EYdotD31);
  double T2 = r1 * r1 - x * x - y * y;

  if (T2 < 0) throw std::invalid_argument("No intersection");

  double z = sqrt(T2);
  MathVector ANSNOZ = MathVector::add(MathVector::add(P1, EX.multiply(x)),
    EY.multiply(y));
  MathVector ANS1 = MathVector::add(ANSNOZ, EZ.multiply(z));
  MathVector ANS2 = MathVector::minus(ANSNOZ, EZ.multiply(z));
  std::vector<MathVector> result;
  result.push_back(ANS1);
  result.push_back(ANS2);
  return result;
}

int main() {
  MathVector P1(0, 0, 0);
  MathVector P2(2, 0, 0);
  MathVector P3(0, 1, 0);
  double r1 = 5;
  double r2 = 6;
  double r3 = 5;
  std::vector<MathVector> vectors = getVectors(P1, P2, P3, r1, r2, r3);
  MathVector vector1 = vectors.at(0);
  MathVector vector2 = vectors.at(1);
  std::cout << " x: " << vector1.x 
    << " y: " << vector1.y 
    << " z: " << vector1.z << std::endl;
  std::cout << " x: " << vector2.x 
    << " y: " << vector2.y 
    << " z: " << vector2.z << std::endl;
  return 0;
}