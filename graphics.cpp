// $Id: graphics.cpp,v 1.4 2019-02-28 15:24:20-08 - - $

#include <iostream>
using namespace std;

#include <GL/freeglut.h>
#include <cmath> // remove

#include "graphics.h"
#include "util.h"

int window::width = 640; // in pixels
int window::height = 480; // in pixels
GLfloat window::thickness = 0;
GLfloat window::move_by = 4;
bool window::selected {false};
rgbcolor window::border_color;
vector<object> window::objects;
size_t window::selected_obj = 0;
mouse window::mus;

// Executed when window system signals to shut down.
void window::close() {
   DEBUGF ('g', sys_info::execname() << ": exit ("
           << sys_info::exit_status() << ")");
   exit (sys_info::exit_status());
}

// Executed when mouse enters or leaves window.
void window::entry (int mouse_entered) {
   DEBUGF ('g', "mouse_entered=" << mouse_entered);
   window::mus.entered = mouse_entered;
   if (window::mus.entered == GLUT_ENTERED) {
      DEBUGF ('g', sys_info::execname() << ": width=" << window::width
           << ", height=" << window::height);
   }
   glutPostRedisplay();
}

// Called to display the objects in the window.
void window::display() {
   glClear (GL_COLOR_BUFFER_BIT);

   // draw border of selected object
   objects.at(selected_obj).draw();

   // draw rest of objects
   selected = false;
   for (auto& object: window::objects) {
      object.draw();
   }

   mus.draw();
   glutSwapBuffers();
}

// Called when window is opened and when resized.
void window::reshape (int width, int height) {
   DEBUGF ('g', "width=" << width << ", height=" << height);
   window::width = width;
   window::height = height;
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D (0, window::width, 0, window::height);
   glMatrixMode (GL_MODELVIEW);
   glViewport (0, 0, window::width, window::height);
   glClearColor (0.25, 0.25, 0.25, 1.0);
   glutPostRedisplay();
}

// Executed when a regular keyboard key is pressed.
void window::keyboard (GLubyte key, int x, int y) {
   enum {BS = 8, TAB = 9, ESC = 27, SPACE = 32, DEL = 127};
   DEBUGF ('g', "key=" << unsigned (key) << ", x=" << x << ", y=" << y);
   window::mus.set (x, y);
   selected = true;
   switch (key) {
      case 'Q': case 'q': case ESC:
         window::close();
         break;
      case 'H': case 'h':
         // move_selected_object left
         objects.at(selected_obj).move(-move_by,0.0);

         // reset position if moved off screen
         if(objects.at(selected_obj).get_pos().xpos < -50) {
            objects.at(selected_obj).set_pos(width, 
                  objects.at(selected_obj).get_pos().ypos);
         }
         break;
      case 'J': case 'j':
         //move_selected_object down
         objects.at(selected_obj).move(0.0,-move_by);

         // reset position if moved off screen
         if(objects.at(selected_obj).get_pos().ypos > height+50) {
            objects.at(selected_obj).set_pos(
                  objects.at(selected_obj).get_pos().xpos, 0);
         }
         break;
      case 'K': case 'k':
         //move_selected_object up
         objects.at(selected_obj).move(0.0, move_by);

         // reset position if moved off screen
         if(objects.at(selected_obj).get_pos().ypos < -50) {
            objects.at(selected_obj).set_pos(
                  objects.at(selected_obj).get_pos().xpos, height);
         }         
         break;
      case 'L': case 'l':
         // move_selected_object right
         objects.at(selected_obj).move(move_by,0.0);

         // reset position if moved off screen
         if(objects.at(selected_obj).get_pos().xpos > width+50) {
            objects.at(selected_obj).set_pos(0, 
                  objects.at(selected_obj).get_pos().ypos);
         }
         break;
      case 'N': case 'n': case SPACE: case TAB:
         if(selected_obj == objects.size()-1) {
            selected_obj = 0;
         } else
            ++selected_obj; 

         selected = true;
         break;
      case 'P': case 'p': case BS:
         if(selected_obj == 0) 
            selected_obj = objects.size()-1;
         else
            --selected_obj;   

         selected = true;
         break;
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
         // convert to size_t digit
         selected_obj = key-'0';
         if(selected_obj < objects.size()) {
            selected = true;
         } else {
            cerr << "Object does not exist at " << key << endl;
            selected_obj = 0;
            selected = false;
         }
         break;
      default:
         cerr << unsigned (key) << ": invalid keystroke" << endl;
         break;
   }
   glutPostRedisplay();
}

// Executed when a special function key is pressed.
void window::special (int key, int x, int y) {
   DEBUGF ('g', "key=" << key << ", x=" << x << ", y=" << y);
   window::mus.set (x, y);
   selected = true;
   switch (key) {
      case GLUT_KEY_LEFT: 
         // move_selected_object left
         objects.at(selected_obj).move(-move_by,0.0);

         // reset position if moved off screen
         if(objects.at(selected_obj).get_pos().xpos < -50) {
            objects.at(selected_obj).set_pos(width, 
                  objects.at(selected_obj).get_pos().ypos);
         }
         break;
      case GLUT_KEY_DOWN: 
         //move_selected_object down
         objects.at(selected_obj).move(0.0,-move_by);

         // reset position if moved off screen
         if(objects.at(selected_obj).get_pos().ypos > height+50) {
            objects.at(selected_obj).set_pos(
                  objects.at(selected_obj).get_pos().xpos, 0);
         }
         break;
      case GLUT_KEY_UP: 
         //move_selected_object up
         objects.at(selected_obj).move(0.0, move_by);

         // reset position if moved off screen
         if(objects.at(selected_obj).get_pos().ypos < -50) {
            objects.at(selected_obj).set_pos(
                  objects.at(selected_obj).get_pos().xpos, height);
         }    
         break;
      case GLUT_KEY_RIGHT: 
         // move_selected_object right
         objects.at(selected_obj).move(move_by,0.0);

         // reset position if moved off screen
         if(objects.at(selected_obj).get_pos().xpos > width+50) {
            objects.at(selected_obj).set_pos(0, 
                  objects.at(selected_obj).get_pos().ypos);
         }
         break;
      case GLUT_KEY_F1: 
         // convert to size_t digit and select_object 1
         selected_obj = 1;
         if(selected_obj < objects.size()) {
            selected = true;
         } else {
            cerr << "Object does not exist at " << key << endl;
            selected_obj = 0;
            selected = false;
         }
         break;
      case GLUT_KEY_F2: 
         // convert to size_t digit and select_object 2
         selected_obj = 2;
         if(selected_obj < objects.size()) {
            selected = true;
         } else {
            cerr << "Object does not exist at " << key << endl;
            selected_obj = 0;
            selected = false;
         }
         break;
      case GLUT_KEY_F3: 
         // convert to size_t digit and select_object 3
         selected_obj = 3;
         if(selected_obj < objects.size()) {
            selected = true;
         } else {
            cerr << "Object does not exist at " << key << endl;
            selected_obj = 0;
            selected = false;
         }
         break;
      case GLUT_KEY_F4: 
         // convert to size_t digit and select_object 4
         selected_obj = 4;
         if(selected_obj < objects.size()) {
            selected = true;
         } else {
            cerr << "Object does not exist at " << key << endl;
            selected_obj = 0;
            selected = false;
         }
         break;
      case GLUT_KEY_F5: 
         // convert to size_t digit and select_object 5
         selected_obj = 5;
         if(selected_obj < objects.size()) {
            selected = true;
         } else {
            cerr << "Object does not exist at " << key << endl;
            selected_obj = 0;
            selected = false;
         }
         break;
      case GLUT_KEY_F6: 
         // convert to size_t digit and select_object 6
         selected_obj = 6;
         if(selected_obj < objects.size()) {
            selected = true;
         } else {
            cerr << "Object does not exist at " << key << endl;
            selected_obj = 0;
            selected = false;
         }
         break;
      case GLUT_KEY_F7: 
         // convert to size_t digit and select_object 7
         selected_obj = 7;
         if(selected_obj < objects.size()) {
            selected = true;
         } else {
            cerr << "Object does not exist at " << key << endl;
            selected_obj = 0;
            selected = false;
         }
         break;
      case GLUT_KEY_F8: 
         // convert to size_t digit and select_object 8
         selected_obj = 8;
         if(selected_obj < objects.size()) {
            selected = true;
         } else {
            cerr << "Object does not exist at " << key << endl;
            selected_obj = 0;
            selected = false;
         }
         break;
      case GLUT_KEY_F9: 
         // convert to size_t digit and select_object 9
         selected_obj = 9;
         if(selected_obj < objects.size()) {
            selected = true;
         } else {
            cerr << "Object does not exist at " << key << endl;
            selected_obj = 0;
            selected = false;
         }
         break;
      case GLUT_KEY_F10: 
         // convert to size_t digit and select_object 10
         selected_obj = 10;
         if(selected_obj < objects.size()) {
            selected = true;
         } else {
            cerr << "Object does not exist at " << key << endl;
            selected_obj = 0;
            selected = false;
         }
         break;
      case GLUT_KEY_F11: 
         // convert to size_t digit and select_object 11
         selected_obj = 11;
         if(selected_obj < objects.size()) {
            selected = true;
         } else {
            cerr << "Object does not exist at " << key << endl;
            selected_obj = 0;
            selected = false;
         }
         break;
      case GLUT_KEY_F12: 
         // convert to size_t digit and select_object 12
         selected_obj = 12;
         if(selected_obj < objects.size()) {
            selected = true;
         } else {
            cerr << "Object does not exist at " << key << endl;
            selected_obj = 0;
            selected = false;
         }
         break;
      default:
         cerr << unsigned (key) << ": invalid function key" << endl;
         break;
   }
   glutPostRedisplay();
}


void window::motion (int x, int y) {
   DEBUGF ('g', "x=" << x << ", y=" << y);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::passivemotion (int x, int y) {
   DEBUGF ('g', "x=" << x << ", y=" << y);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::mousefn (int button, int state, int x, int y) {
   DEBUGF ('g', "button=" << button << ", state=" << state
           << ", x=" << x << ", y=" << y);
   window::mus.state (button, state);
   window::mus.set (x, y);
   glutPostRedisplay();
}

void window::main () {
   static int argc = 0;
   glutInit (&argc, nullptr);
   glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE);
   glutInitWindowSize (window::width, window::height);
   glutInitWindowPosition (128, 128);
   glutCreateWindow (sys_info::execname().c_str());
   glutCloseFunc (window::close);
   glutEntryFunc (window::entry);
   glutDisplayFunc (window::display);
   glutReshapeFunc (window::reshape);
   glutKeyboardFunc (window::keyboard);
   glutSpecialFunc (window::special);
   glutMotionFunc (window::motion);
   glutPassiveMotionFunc (window::passivemotion);
   glutMouseFunc (window::mousefn);
   DEBUGF ('g', "Calling glutMainLoop()");
   glutMainLoop();
}


void mouse::state (int button, int state) {
   switch (button) {
      case GLUT_LEFT_BUTTON: left_state = state; break;
      case GLUT_MIDDLE_BUTTON: middle_state = state; break;
      case GLUT_RIGHT_BUTTON: right_state = state; break;
   }
}

void mouse::draw() {
   static rgbcolor color ("green");
   ostringstream text;
   text << "(" << xpos << "," << window::height - ypos << ")";
   if (left_state == GLUT_DOWN) text << "L"; 
   if (middle_state == GLUT_DOWN) text << "M"; 
   if (right_state == GLUT_DOWN) text << "R"; 
   if (entered == GLUT_ENTERED) {
      void* font = GLUT_BITMAP_HELVETICA_18;
      glColor3ubv (color.ubvec);
      glRasterPos2i (10, 10);
      auto ubytes = reinterpret_cast<const GLubyte*>
                    (text.str().c_str());
      glutBitmapString (font, ubytes);
   }
}

