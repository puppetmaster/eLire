//STDLIB
#include <stdio.h>

// EFL
#include <Eina.h>
#include <Eio.h>
#include <Ecore.h>
#include <Efreet_Mime.h>
#include <Ecore_File.h>

// OTHER

#define BOOKPATH "./books"
#define EPUB_MIME_TYPE "application/epub+zip"

typedef struct 
{
  char *path;
  Eina_List *books;
} eBiblio;

static void elire_init()
{
  eina_init();
  ecore_init();
  eio_init();
  efreet_mime_init();

}

static void elire_shutdown()
{
  eio_shutdown();
  ecore_shutdown();
  efreet_mime_shutdown();
  eina_shutdown();
}

static void ebiblio_free(eBiblio *bib)
{
  free(bib);
}

static void ebiblio_show(eBiblio *bib)
{
  Eina_List *book;
  char *data; 

  EINA_LIST_FOREACH(bib->books, book, data)
  {
    printf("Book : %s\n", data);
  }
}

static Eina_Bool
_filter_epub_cb(void *data, Eio_File *handler, const Eina_File_Direct_Info *info)
{
  char *mime;
  char *path;

  path = eina_file_path_sanitize(info->path);
  
  if (info->type == EINA_FILE_REG )
  {
    mime = efreet_mime_type_get(path);
    if (!strcmp(mime, EPUB_MIME_TYPE))
      return EINA_TRUE;
    else
      return EINA_FALSE;
  }
  else if (info->type == EINA_FILE_DIR)
    return EINA_TRUE;
  else 
    return EINA_FALSE;
}

static void
_add_to_library_cb(void *data, Eio_File *handler, const Eina_File_Direct_Info *info)
{
  eBiblio *biblio = data;
  char *path;

  path = eina_file_path_sanitize(info->path);
  if (info->type == EINA_FILE_REG)
    biblio->books = eina_list_append(biblio->books, path);
}

static void
_done_cb(void *data, Eio_File *handler)
{
  eBiblio *biblio = data;
  ebiblio_show(biblio);
  ebiblio_free(biblio);
  ecore_main_loop_quit();
}

 static void
_error_cb(void *data, Eio_File *handler, int error)
{
  fprintf(stderr, "Something wrong has happend:%s\n", strerror(error));
  ecore_main_loop_quit();
}

static void ebiblio_create(char* path, eBiblio *biblio)
{
  printf("ICI`n");
  if (biblio == NULL)
    biblio = calloc(1, sizeof(eBiblio));

  biblio->path = path;

  if (ecore_file_is_dir(biblio->path))
    eio_dir_stat_ls(path, _filter_epub_cb, _add_to_library_cb, _done_cb, _error_cb, biblio);
  else
    printf("%s : No such file or directory\n", biblio->path);

}

int main(int argc, char **argv)
{
  eBiblio *library = NULL;

  elire_init();

  ebiblio_create(BOOKPATH, library);

  ecore_main_loop_begin();

  elire_shutdown();
  return 0;
}
