// $Id: interp.cpp,v 1.2 2015-07-16 16:57:30-07 - - $

#include <iostream> //need?
#include <fstream>

#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"border" , &interpreter::do_border },
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"moveby" , &interpreter::do_moveby },
};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"       , &interpreter::make_text       },
   {"circle"     , &interpreter::make_circle     },
   {"diamond"    , &interpreter::make_diamond    },
   {"ellipse"    , &interpreter::make_ellipse    },
   {"equilateral", &interpreter::make_equilateral},  
   {"polygon"    , &interpreter::make_polygon    },
   {"rectangle"  , &interpreter::make_rectangle  },
   {"square"     , &interpreter::make_square     },
   {"triangle"   , &interpreter::make_polygon    },
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

void interpreter::do_border (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   rgbcolor border_color {begin[0]};

   window::set_border(border_color);
   window::set_thick(from_string<GLfloat> (begin[1]));
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.emplace (name, make_shape (++begin, end));
}

void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      cerr << name + ": no such shape" << endl;
      return;
      throw runtime_error (name + ": no such shape");
   }
   rgbcolor color {begin[0]};
   rgbcolor border_color {"red"};
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};

   // add shape object to display window
   object shape; 
   shape.set(itor->second, where, color);

   // set default border color and line thickness for select
   window::set_border(border_color);
   window::set_thick(4.0);
   window::push_back(shape);
}

void interpreter::do_moveby (param begin, param end) {
   DEBUGF ('f', range (begin, end));

   window::set_move(from_string<GLfloat> (begin[0]));
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   void* glut_bitmap_font = nullptr;
   string s = "";
   int count = 0;

   for(auto i = begin; i != end; ++i) ++count;

   // case checking 
   if (begin[0] == "Fixed-8x13")
      glut_bitmap_font = GLUT_BITMAP_8_BY_13;
   else if (begin[0] == "Fixed-9x15")
      glut_bitmap_font = GLUT_BITMAP_9_BY_15;
   else if (begin[0] == "Helvetica-10")
      glut_bitmap_font = GLUT_BITMAP_HELVETICA_10;
   else if (begin[0] == "Helvetica-12")
      glut_bitmap_font = GLUT_BITMAP_HELVETICA_12;
   else if (begin[0] == "Helvetica-18")
      glut_bitmap_font = GLUT_BITMAP_HELVETICA_18;
   else if (begin[0] == "Times-Roman-10")
      glut_bitmap_font = GLUT_BITMAP_TIMES_ROMAN_10;
   else if (begin[0] == "Times-Roman-24")
      glut_bitmap_font = GLUT_BITMAP_TIMES_ROMAN_24;
 
   // store text data
   count -= 1;
   for(int i = 1; i != count; ++i) {
      s.append(begin[i] + " ");
   }
   s.append(begin[count]); 

   return make_shared<text> (glut_bitmap_font, s);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));

   GLfloat w = from_string<GLfloat> (begin[0]);
   GLfloat h = from_string<GLfloat> (begin[1]); 

   return make_shared<ellipse> (w, h);
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<circle> (from_string<GLfloat> (begin[0]));
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));

  vertex_list v; 
  vertex pair; 
  int count = 0;

  for(auto i = begin; i != end; ++i) ++count;

  // get (x,y) coordinate for vertices of polygon
  for(auto i = 0; i != count; i+=2) {
     pair.xpos = from_string<GLfloat> (begin[i]);
     pair.ypos = from_string<GLfloat> (begin[i+1]);

     v.push_back(pair);
  } 

   return make_shared<polygon> (v);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));

   GLfloat w = from_string<GLfloat> (begin[0]);
   GLfloat h = from_string<GLfloat> (begin[1]);

   return make_shared<rectangle> (w, h);
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   return make_shared<square> (from_string<GLfloat> (begin[0]));
}

shape_ptr interpreter::make_diamond (param begin, param end) {
   DEBUGF ('f', range (begin, end));

   GLfloat w = from_string<GLfloat> (begin[0]);
   GLfloat h = from_string<GLfloat> (begin[1]);

   return make_shared<diamond> (w, h);
}

shape_ptr interpreter::make_equilateral (param begin, param end) {
   DEBUGF ('f', range (begin, end));

   GLfloat w = from_string<GLfloat> (begin[0]);

   return make_shared<equilateral> (w);
}

