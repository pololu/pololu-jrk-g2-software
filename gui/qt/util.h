#pragma once

#include <QList>

static bool ordered(const QList<int> & list)
{
  for (int i = 0; i < list.size() - 1; i++)
  {
    if (list[i] > list[i + 1]) { return false; }
  }
  return true;
}
