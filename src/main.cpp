// Based on: http://www.cs.unm.edu/~angel/BOOK/INTERACTIVE_COMPUTER_GRAPHICS/SIXTH_EDITION/CODE/CHAPTER03/WINDOWS_VERSIONS/example2.cpp
// Modified to isolate the main program and use GLM

#include "common.h"
#include <iostream>

// Create a NULL-terminated string by reading the provided file
static char*
readShaderSource(const char* shaderFile)
{
   FILE* fp = fopen(shaderFile, "rb");

   if ( fp == NULL ) { return NULL; }

   fseek(fp, 0L, SEEK_END);
   long size = ftell(fp);

   fseek(fp, 0L, SEEK_SET);
   char* buf = new char[size + 1];
   fread(buf, 1, size, fp);

   buf[size] = '\0';
   fclose(fp);

   return buf;
}


// Create a GLSL program object from vertex and fragment shader files
GLuint
InitShader(const char* vShaderFile, const char* fShaderFile)
{
   struct Shader {
      const char*  filename;
      GLenum       type;
      GLchar*      source;
   }  shaders[2] = {
      { vShaderFile, GL_VERTEX_SHADER, NULL },
      { fShaderFile, GL_FRAGMENT_SHADER, NULL }
   };

   GLuint program = glCreateProgram();
    
   for ( int i = 0; i < 2; ++i ) {
      Shader& s = shaders[i];
      s.source = readShaderSource( s.filename );
      if ( shaders[i].source == NULL ) {
         std::cerr << "Failed to read " << s.filename << std::endl;
         exit( EXIT_FAILURE );
      }

      GLuint shader = glCreateShader( s.type );
      glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );
      glCompileShader( shader );

      GLint  compiled;
      glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
      if ( !compiled ) {
         std::cerr << s.filename << " failed to compile:" << std::endl;
         GLint  logSize;
         glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
         char* logMsg = new char[logSize];
         glGetShaderInfoLog( shader, logSize, NULL, logMsg );
         std::cerr << logMsg << std::endl;
         delete [] logMsg;

         exit( EXIT_FAILURE );
      }

      delete [] s.source;

      glAttachShader( program, shader );
   }

   /* link  and error check */
   glLinkProgram(program);

   GLint  linked;
   glGetProgramiv( program, GL_LINK_STATUS, &linked );
   if ( !linked ) {
      std::cerr << "Shader program failed to link" << std::endl;
      GLint  logSize;
      glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
      char* logMsg = new char[logSize];
      glGetProgramInfoLog( program, logSize, NULL, logMsg );
      std::cerr << logMsg << std::endl;
      delete [] logMsg;

      exit( EXIT_FAILURE );
   }

   /* use program object */
   glUseProgram(program);

   return program;
}

void
timer(int unused)
{
   update();
   glutPostRedisplay();
   glutTimerFunc( FRAME_RATE_MS, timer, 0 );
}

int
main( int argc, char **argv )
{
   glutInit( &argc, argv );
   glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
   glutInitWindowSize( 712, 712 );
   glutInitContextVersion( 3, 2 );
   glutInitContextProfile( GLUT_CORE_PROFILE );
   glutCreateWindow( WINDOW_TITLE );

   glewInit();

   init();

   glutDisplayFunc( display );
   glutKeyboardFunc( keyboard );
   glutMouseFunc( mouse );
   glutReshapeFunc( reshape );
   glutTimerFunc( FRAME_RATE_MS, timer, 0 );
   
   glutMainLoop();
   return 0;
}