/** @file gles_renderer.c
 */

#include "gles_renderer.h"

#include <stdlib.h>

#include <GLES2/gl2.h>

static const char *VERTEX_SHADER = "precision mediump float;\n"
                                   "attribute vec3 position;\n"
                                   "// attribute vec4 color;\n"
                                   "void main() {\n"
                                   "  gl_Position = vec4(position, 1.0);\n"
                                   "}\n";

static const char *FRAGMENT_SHADER = "precision mediump float;\n"
                                     "void main() {\n"
                                     "  gl_FragColor = vec4(1);\n"
                                     "}\n";

struct gles_renderer_s
{
  canary_panel_t *panel;

  GLuint program;

  GLuint vbo;
  GLuint ibo;
};

static void
check_shader_compile_status (GLuint shader)
{
  GLint result = GL_FALSE;
  int log_length;

  glGetShaderiv (shader, GL_COMPILE_STATUS, &result);
  if (result == GL_TRUE)
    return;

  glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &log_length);

  char *message = malloc (log_length + 1);
  glGetShaderInfoLog (shader, log_length, NULL, message);
  LOG_ERR ("%s", message);
  free (message);
}

int
gles_renderer_create (gles_renderer_t **new_ren, canary_panel_t *panel)
{
  gles_renderer_t *ren = malloc (sizeof (gles_renderer_t));
  *new_ren = ren;

  ren->panel = panel;

  GLuint vertex_shader = glCreateShader (GL_VERTEX_SHADER);
  GLuint fragment_shader = glCreateShader (GL_FRAGMENT_SHADER);

  glShaderSource (vertex_shader, 1, &VERTEX_SHADER, NULL);
  glCompileShader (vertex_shader);
  check_shader_compile_status (vertex_shader);

  glShaderSource (fragment_shader, 1, &FRAGMENT_SHADER, NULL);
  glCompileShader (fragment_shader);
  check_shader_compile_status (fragment_shader);

  ren->program = glCreateProgram ();
  glAttachShader (ren->program, vertex_shader);
  glAttachShader (ren->program, fragment_shader);
  glLinkProgram (ren->program);

  glDetachShader (ren->program, vertex_shader);
  glDetachShader (ren->program, fragment_shader);

  glDeleteShader (vertex_shader);
  glDeleteShader (fragment_shader);

  glGenBuffers (1, &ren->vbo);
  glGenBuffers (1, &ren->ibo);

  return 0;
}

void
gles_renderer_delete (gles_renderer_t *ren)
{
  glDeleteBuffers (1, &ren->vbo);
  glDeleteBuffers (1, &ren->ibo);

  glDeleteProgram (ren->program);

  free (ren);
}

void
gles_renderer_render_frame (gles_renderer_t *ren)
{
  glBindBuffer (GL_ARRAY_BUFFER, ren->vbo);

  static const GLfloat vertices[]
      = { -1.0, -1.0, 0.0, 1.0, -1.0, 0.0, 0.0, 1.0, 0.0 };
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray (0);
  glBindBuffer (GL_ARRAY_BUFFER, ren->vbo);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glUseProgram (ren->program);
  glDrawArrays (GL_TRIANGLES, 0, 3);

  glDisableVertexAttribArray (0);
}
