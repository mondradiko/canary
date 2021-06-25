/** @file gles_renderer.c
 */

#include "gles_renderer.h"
#include "draw_list.h"
#include "panel.h"

#include <stdlib.h>

#include <GLES2/gl2.h>

static const char *VERTEX_SHADER
    = "precision mediump float;\n"
      "attribute vec2 position;\n"
      "attribute vec4 vert_color;\n"
      "varying vec4 frag_color;\n"
      "void main() {\n"
      "  frag_color = vert_color;\n"
      "  gl_Position = vec4(position, 0.0, 1.0);\n"
      "}\n";

static const char *FRAGMENT_SHADER = "precision mediump float;\n"
                                     "varying vec4 frag_color;\n"
                                     "void main() {\n"
                                     "  gl_FragColor = frag_color;\n"
                                     "}\n";

struct gles_renderer_s
{
  canary_panel_t *panel;

  GLuint program;

  GLuint vertex_array;
  GLuint vbo;
  GLuint ibo;
};

static void
check_shader_compile_status (GLuint shader)
{
  GLint result = GL_FALSE;

  glGetShaderiv (shader, GL_COMPILE_STATUS, &result);
  if (result == GL_TRUE)
    return;

  int log_length;
  glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &log_length);

  char *message = malloc (log_length + 1);
  glGetShaderInfoLog (shader, log_length, NULL, message);
  LOG_ERR ("shader validation failed:\n%s", message);
  free (message);
}

static int
validate_program (GLuint program)
{
  GLint result = GL_FALSE;
  glValidateProgram (program);
  glGetProgramiv (program, GL_VALIDATE_STATUS, &result);

  if (result == GL_TRUE)
    return 1;

  int log_length;
  glGetProgramiv (program, GL_INFO_LOG_LENGTH, &log_length);

  if (log_length == 0)
    {
      LOG_ERR ("no program validation info log given");
    }
  else
    {
      char *message = malloc (log_length + 1);
      glGetProgramInfoLog (program, log_length, NULL, message);
      LOG_ERR ("program validation failed:\n%s", message);
      free (message);
    }

  return 0;
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

  glBindAttribLocation (ren->program, 0, "position");
  glBindAttribLocation (ren->program, 1, "vert_color");

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
  canary_draw_list_t *draw_list = canary_panel_get_draw_list (ren->panel);
  if (!draw_list)
    return;

  size_t vertex_count = canary_draw_list_vertex_count (draw_list);
  canary_draw_vertex_t *vertices = canary_draw_list_vertex_buffer (draw_list);

  size_t index_count = canary_draw_list_index_count (draw_list);
  canary_draw_index_t *indices = canary_draw_list_index_buffer (draw_list);

  glUseProgram (ren->program);

  glBindBuffer (GL_ARRAY_BUFFER, ren->vbo);
  glBufferData (GL_ARRAY_BUFFER, vertex_count * sizeof (canary_draw_vertex_t),
                vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray (0);
  glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE,
                         sizeof (canary_draw_vertex_t),
                         (void *)offsetof (canary_draw_vertex_t, position));

  glEnableVertexAttribArray (1);
  glVertexAttribPointer (1, 4, GL_FLOAT, GL_FALSE,
                         sizeof (canary_draw_vertex_t),
                         (void *)offsetof (canary_draw_vertex_t, color));

  if (validate_program (ren->program))
    {
      glDrawElements (GL_TRIANGLES, index_count, GL_UNSIGNED_INT, indices);
    }

  glDisableVertexAttribArray (0);
  glDisableVertexAttribArray (1);
}
