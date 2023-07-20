#pragma once

#include <string>
#include <iostream>
#include <fstream>

/**
* 船舶等级代表所有型号的船舶，从尺寸 1 到 5
* 所有与船舶相关的方法都包含在此类中
*/
class Ship
{
private:
std::string* m_ship;
int m_length;
int m_damage;

public:
  Ship();
  ~Ship();

  // 创建所需长度的船
  void createShip(int length);
  // 船放置的坐标和索引
  void setCoordinate(std::string coordinate, int index);

  // 返回 index 的船坐标
  std::string getCoordinate(int index) const;

  // 设置船的长度 <= 5
  void setLength(int length);

  // 返回船的长度
  int getLength() const;

  // 船是否被击沉
  bool isSunk();

  // 击中敌舰并对其造成伤害
  void addDamage();

  // 获得对船舶的损坏
  int getDamage() const;

  // 设置船损坏条件
  void setDamage(int newDamage);

};
