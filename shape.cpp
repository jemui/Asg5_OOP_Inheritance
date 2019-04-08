// $Id: shape.cpp,v 1.2 2019-02-28 15:24:20-08 - - $

#include <typeinfo>
#include <unordered_map>
#include <cmath>
using namespace std;

#include <GL/freeglut.h> // might not need
#include "graphics.h"
#include <stdio.h>
#include "interp.h"

#include "shape.h"
#include "util.h"

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font_, const string& textdata_):
      glut_bitmap_font(glut_bitmap_font_), textdata(textdata_) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
      dimension ({width, height} ){
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}

polygon::polygon (const vertex_list& vertices_): vertices(vertices_) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
            polygon( make_coords(width, height)) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

equilateral::equilateral (GLfloat width): 
      polygon( make_coords(width)  ) {
   DEBUGF ('c', this);
}

diamond::diamond (GLfloat width, GLfloat height):
            polygon( make_coords(width, height)  ) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");

   void* font = glut_bitmap_font;
   auto ubytes = reinterpret_cast<const GLubyte*>
                      (textdata.c_str());

   glColor3ubv(color.ubvec);
   glRasterPos2f(center.xpos, center.ypos);
   glutBitmapString (font, ubytes);

}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");

   const float delta = 2 * M_PI / 32;
   float w = dimension.xpos / 3;
   float h = dimension.ypos / 3;

   // draw border if selected
   if(window::is_selected()) {
      glColor3ubv(window::get_border().ubvec);
      glLineWidth(window::get_thick());
      glBegin (GL_LINE_LOOP);
      for (float theta = 0; theta < 2 * M_PI; theta += delta) {
         float xpos = w * cos (theta) + center.xpos;
         float ypos = h * sin (theta) + center.ypos;
         glVertex2f (xpos, ypos);
      }
      glEnd();
   }

   // draw ellipse
   glColor3ubv (color.ubvec);
   glBegin (GL_POLYGON);
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      float xpos = w * cos (theta) + center.xpos;
      float ypos = h * sin (theta) + center.ypos;
      glVertex2f (xpos, ypos);
   }
   
   glEnd();
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");

   // calculate center of polygon
   GLfloat avg_x = 0; 
   GLfloat avg_y = 0; 

   for(unsigned int i = 0; i < vertices.size(); ++i) {
      avg_x += vertices[i].xpos; 
      avg_y += vertices[i].ypos; 
   }
   
   avg_x /= vertices.size();
   avg_y /= vertices.size();

   // draw border if selected
   if(window::is_selected()) {
      glColor3ubv(window::get_border().ubvec);
      glLineWidth(window::get_thick());

      glBegin (GL_LINE_LOOP);
      for(unsigned int i = 0; i < vertices.size(); ++i) {
         glVertex2f (center.xpos + (vertices[i].xpos-avg_x), 
               center.ypos + (vertices[i].ypos-avg_y));
      }
      glEnd();
   }

   // draw polygon
   glColor3ubv (color.ubvec);
   glBegin (GL_POLYGON);
   for(unsigned int i = 0; i < vertices.size(); ++i) {
      glVertex2f (center.xpos + (vertices[i].xpos-avg_x), 
            center.ypos + (vertices[i].ypos-avg_y));
   }
   glEnd();
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

vertex_list rectangle::make_coords (GLfloat width, GLfloat height) {
   vertex_list v; 
   vertex pair; 

   // top left
   pair.xpos = width/2;
   pair.ypos = height/2; 
   v.push_back(pair);
   
   // top right
   pair.xpos = width;
   pair.ypos = height/2; 
   v.push_back(pair);

   // bottom right
   pair.xpos = width;
   pair.ypos = height; 
   v.push_back(pair);

   // bottom left
   pair.xpos = width/2;
   pair.ypos = height; 
   v.push_back(pair);

   return v;
}

vertex_list diamond::make_coords (GLfloat width, GLfloat height) {
   vertex_list v; 
   vertex pair; 

   // top center
   pair.xpos = width/2;
   pair.ypos = height; 
   v.push_back(pair);
   
   // right
   pair.xpos = width;
   pair.ypos = height/2; 
   v.push_back(pair);

   // bottom center
   pair.xpos = width/2;
   pair.ypos = 0; 
   v.push_back(pair);

    // left
   pair.xpos = 0;
   pair.ypos = height/2; 
   v.push_back(pair);
  
   return v;
}

vertex_list equilateral::make_coords (GLfloat width) {
   vertex_list v; 
   vertex pair; 

   // left
   pair.xpos = 0;
   pair.ypos = 0; 
   v.push_back(pair);
   
   // top
   pair.xpos = width/2;
   pair.ypos = width/2; 
   v.push_back(pair);

   // right
   pair.xpos = width;
   pair.ypos = 0; 
   v.push_back(pair);
  
   return v;
}

