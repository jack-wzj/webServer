#include "Ship.h"

void Ship::createShip(int length)   //createShip function that takes in length for parameter
{
  m_length = length;            //set m_length to the passed in length
  m_ship = new std::string[m_length];   //create a new array of strings of m_length (number of ships)
}

Ship::Ship()
{
  m_ship = nullptr;
  m_length = 0;
  m_damage = 0;
}

void Ship::setLength(int length)
{
  m_length = length;
}

// 获取船的长度
int Ship::getLength() const
{
  return m_length;
}

// 设置索引的坐标函数
void Ship::setCoordinate(std::string coordinate, int index)
{
  m_ship[index] = coordinate;

}

// 获取索引的坐标函数
std::string Ship::getCoordinate(int index) const
{
  return m_ship[index];
}

Ship::~Ship()
{
  delete[] m_ship;
}

// 被击中，伤害+1
void Ship::addDamage()
{
  m_damage++;
}

int Ship::getDamage() const   //getDamage function that returns m_damage
{
  return m_damage;    //return m_damage
}

// 将m_damage设置为新伤害
void Ship::setDamage(int newDamage)   
{
  m_damage = newDamage;
}

// 如果损坏等于船的长度, 返回 true
bool Ship::isSunk()
{
    return m_damage == m_length;
}
