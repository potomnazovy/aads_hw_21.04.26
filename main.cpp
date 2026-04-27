#include <iostream>
#include <tuple>
#include <utility>
#include <cstddef>
#include <string>

template < class T >
struct BiTree
{
  T value;
  BiTree< T >* lt;
  BiTree< T >* rt;
  BiTree< T >* parent;
  BiTree(T val);
};

template< class T >
BiTree< T >::BiTree(T val):
  value(val),
  lt(nullptr),
  rt(nullptr),
  parent(nullptr)
{}

template < class T >
struct InclusionIt
{
  std::pair< BiTree< T >*, BiTree< T >* > incl;
};

enum Dir
{
  fall_left,
  right_child,
  parent,
  end
};

template < class T >
std::pair< size_t, BiTree< T >* > fallLeft(BiTree< T >* node)
{
  size_t depth = 0;
  while (node && node->lt)
  {
    node = node->lt;
    ++depth;
  }
  return {depth, node};
}

template< class T >
std::pair< size_t , BiTree< T >* > findParent(BiTree< T >* node)
{
  if (!node)
  {
    return {0, nullptr};
  }

  size_t depth = 1;
  BiTree< T >* parent = node->parent;
  BiTree< T >* curr = node;
  while (parent && parent->lt != curr)
  {
    curr = parent;
    parent = parent->parent;
    ++depth;
  }

  if (parent && parent->lt == curr)
  {
    return {depth, parent};
  }
  return {0, nullptr};
}

template < class T >
std::tuple< Dir, size_t, BiTree< T >* > nextStruct(BiTree< T >* node)
{
  if (!node)
  {
    return {Dir::end, 0, nullptr};
  }

  if (node->rt)
  {
    auto result = fallLeft(node->rt);
    return {Dir::right_child, result.first + 1, result.second};
  }
  else
  {
    auto result = findParent(node);
    if (result.second)
    {
      return {Dir::parent, result.first, result.second};
    }
    else
    {
      return {Dir::end, 0, nullptr};
    }
  }
}

template < class T >
BiTree< T >* prev(BiTree< T >* curr)
{
  if (!curr)
  {
    return nullptr;
  }

  if (curr->lt)
  {
    BiTree< T >* node = curr->lt;
    while (node->rt)
    {
      node = node->rt;
    }
    return node;
  }

  BiTree< T >* parent = curr->parent;
  while (parent && parent->lt == curr)
  {
    curr = parent;
    parent = curr->parent;
  }
  if (parent && parent->rt == curr)
  {
    return parent;
  }
  return nullptr;
}

template< class T >
std::tuple< BiTree< T >*, BiTree< T >*, bool > isEqualStructStart(BiTree< T >* lhs_start, BiTree< T >* rhs_start)
{
  if ((lhs_start->lt != nullptr) != (rhs_start->lt != nullptr))
  {
    return {lhs_start, rhs_start, false};
  }
  if ((lhs_start->rt != nullptr) != (rhs_start->rt != nullptr))
  {
    return {lhs_start, rhs_start, false};
  }

  auto lhs_curr = nextStruct(lhs_start);
  auto rhs_curr = nextStruct(rhs_start);

  while (std::get< 0 >(lhs_curr) == std::get< 0 >(rhs_curr) && std::get< 1 >(lhs_curr) == std::get< 1 >(rhs_curr) && std::get< 2 >(lhs_curr) && std::get< 2 >(rhs_curr))
  {
    lhs_curr = nextStruct(std::get< 2 >(lhs_curr));
    rhs_curr = nextStruct(std::get< 2 >(rhs_curr));
  }

  bool pattern_ended = (std::get< 0 >(rhs_curr) == Dir::end);
  bool lhs_exited = pattern_ended ? (std::get< 0 >(lhs_curr) == Dir::end || std::get< 0 >(lhs_curr) == Dir::parent) : true;

  return {std::get< 2 >(lhs_curr), std::get< 2 >(rhs_curr), pattern_ended && lhs_exited};
}

template< class T >
InclusionIt< T > begin(BiTree< T >* lhs, BiTree< T >* pattern)
{
  if (!lhs || !pattern)
  {
    return InclusionIt< T >{std::make_pair(nullptr, nullptr)};
  }

  auto pat_info = fallLeft(pattern);
  BiTree< T >* pattern_start = pat_info.second;
  size_t pat_depth = pat_info.first;
  BiTree< T >* lhs_curr = fallLeft(lhs).second;

  while (lhs_curr)
  {
    auto result = isEqualStructStart(lhs_curr, pattern_start);
    if (std::get< 2 >(result))
    {
      BiTree< T >* next_node = std::get< 0 >(result);
      BiTree< T >* lhs_end = nullptr;

      if (next_node)
      {
        lhs_end = prev(next_node);
      }
      else
      {
        BiTree< T >* lhs_root = lhs_curr;
        for (size_t i = 0; i < pat_depth && lhs_root; ++i)
        {
          lhs_root = lhs_root->parent;
        }

        lhs_end = lhs_root;
        while (lhs_end && lhs_end->rt)
        {
          lhs_end = lhs_end->rt;
        }
      }
      return InclusionIt< T >{std::make_pair(lhs_curr, lhs_end)};
    }
    lhs_curr = std::get< 2 >(nextStruct(lhs_curr));
  }
  return InclusionIt< T >{std::make_pair(nullptr, nullptr)};
}

template< class T >
InclusionIt< T > next(InclusionIt< T > curr, BiTree< T >* pattern)
{
  if (!curr.incl.second || !pattern)
  {
    return InclusionIt< T >{std::make_pair(nullptr, nullptr)};
  }

  auto pat_info = fallLeft(pattern);
  BiTree< T >* pattern_start = pat_info.second;
  size_t pat_depth = pat_info.first;
  BiTree< T >* curr_start = std::get< 2 >(nextStruct(curr.incl.second));

  while (curr_start)
  {
    auto result = isEqualStructStart(curr_start, pattern_start);
    if (std::get< 2 >(result))
    {
      BiTree< T >* next_node = std::get< 0 >(result);
      BiTree< T >* lhs_end = nullptr;

      if (next_node)
      {
        lhs_end = prev(next_node);
      }
      else
      {
        BiTree< T >* lhs_root = curr_start;
        for (size_t i = 0; i < pat_depth && lhs_root; ++i)
        {
          lhs_root = lhs_root->parent;
        }

        lhs_end = lhs_root;
        while (lhs_end && lhs_end->rt)
        {
          lhs_end = lhs_end->rt;
        }
      }
      return InclusionIt< T >{std::make_pair(curr_start, lhs_end)};
    }
    curr_start = std::get< 2 >(nextStruct(curr_start));
  }
  return InclusionIt< T >{std::make_pair(nullptr, nullptr)};
}

template< class T >
bool hasNext(InclusionIt< T > curr, BiTree< T >* pattern)
{
  if (!curr.incl.second || !pattern)
  {
    return false;
  }

  auto it = next(curr, pattern);
  if (it.incl.first)
  {
    return true;
  }
  return false;
}

template < class T >
void printTree(BiTree< T >* n, int d = 0)
{
  if (!n)
  {
    return;
  }
  std::cout << std::string(d * 2, ' ') << n->value << "\n";
  if (n->lt)
  {
    std::cout << std::string(d * 2, ' ') << "L:";
    printTree(n->lt, d + 1);
  }
  if (n->rt)
  {
    std::cout << std::string(d * 2, ' ') << "R:";
    printTree(n->rt, d + 1);
  }
}

template < class T > void
setLeft(BiTree< T >* p, BiTree< T >* c)
{
  if(p && c)
  {
    p->lt = c;
    c->parent = p;
  }
}

template < class T >
void setRight(BiTree< T >* p, BiTree< T >* c)
{
  if (p && c)
  {
    p->rt = c; 
    c->parent = p;
  }
}

template < class T >
void deleteTree(BiTree< T >* root)
{
  if (!root)
  {
    return;
  }
  deleteTree(root->lt);
  deleteTree(root->rt);
  delete root;
}

int main()
{
  BiTree< int >* root = new BiTree< int >(20);
  auto n15 = new BiTree< int >(15);
  setLeft(root, n15);

  auto n30 = new BiTree< int >(30);

  setRight(root, n30);

  auto n40 = new BiTree< int >(40);
  setRight(n30, n40);

  auto n10 = new BiTree< int >(10);
  setLeft(n15, n10);

  auto n18 = new BiTree< int >(18);
  setRight(n15, n18);

  std::cout << "LHS Дерево\n";
  printTree(root);
  std::cout << "\n";

  std::cout << "ТЕСТ 1: Ищем узел с двумя детьми (Left и Right)\n";
  BiTree< int >* pat1 = new BiTree< int >(2);

  auto p1_l = new BiTree< int >(1);
  setLeft(pat1, p1_l);

  auto p1_r = new BiTree< int >(3);
  setRight(pat1, p1_r);
  
  int cnt1 = 1;
  for (auto it = begin(root, pat1); it.incl.first; it = next(it, pat1))
  {
    std::cout << "  Найдено вхождение: " << cnt1++ << " в узле (корень): " << it.incl.first->value << " " << it.incl.second->value << "\n";
  }
  if (cnt1 == 1)
  {
    std::cout << "  (Ничего не найдено)\n";
  }
  std::cout << "\n";


  std::cout << "ТЕСТ 2: Ищем узел ТОЛЬКО с левым ребенком\n";
  BiTree< int >* pat2 = new BiTree< int >(1);

  auto p2_l = new BiTree< int >(2);
  setLeft(pat2, p2_l);
 
  int cnt2 = 1;
  for (auto it = begin(root, pat2); it.incl.first; it = next(it, pat2))
  {
    std::cout << "  Найдено вхождение: " << cnt2++ << " в узле (корень): " << it.incl.first->value  << " " << it.incl.second->value << "\n";
  }
  if (cnt2 == 1)
  {
    std::cout << "  (Ничего не найдено)\n";
  }
  std::cout << "\n";

  std::cout << "ТЕСТ 3: Ищем узел ТОЛЬКО с правым ребенком\n";
  BiTree< int >* pat3 = new BiTree< int >(1);

  auto p3_r = new BiTree< int >(2);
  setRight(pat3, p3_r);

  int cnt3 = 1;
  for (auto it = begin(root, pat3); it.incl.first; it = next(it, pat3))
  {
    std::cout << "  Найдено вхождение: " << cnt3++ << " в узле (корень): " << it.incl.first->value << " " << it.incl.second->value << "\n";
  }
  if (cnt3 == 1)
  {
    std::cout << "  (Ничего не найдено)\n";
  }

  deleteTree(root);
  deleteTree(pat1);
  deleteTree(pat2);
  deleteTree(pat3);
  return 0;
}
