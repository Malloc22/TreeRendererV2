//---------------------------

#ifndef INCLUDED_LIBRARIES_H_INCLUDED
#define INCLUDED_LIBRARIES_H_INCLUDED

//---------------------------

#include <map>
#include <set>
#include <stack>
#include <vector>
#include <cmath>
#include <deque>
#include <string>
#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

//---------------------------

#define TITLE "TreeRenderer"

//---------------------------

#define NOT_USED(x) ((void)(x))
#define ERROR_LOG(object, x) std::cerr << "-error: " << #object << ' ' << x << std::endl;
#define LOG(object, x) std::cout << "-log: " << #object << ' ' << x << std::endl;

//---------------------------

#ifdef debug_v
#define VERSION "[debug 2.0v]"
#endif

//---------------------------

#ifdef release_v
#define VERSION "[alpha 0.2v]"
#endif

//---------------------------

#endif // INCLUDED_LIBRARIES_H_INCLUDED
