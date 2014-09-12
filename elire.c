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
  if (info->type == EINA_FILE_REG)
    return EINA_TRUE;
  else
    return EINA_FALSE;
}

static void
_add_to_library_cb(void *data, Eio_File *handler, const Eina_File_Direct_Info *info)
{
  eBiblio *biblio = data;

  if (info->path != NULL)
    biblio->books = eina_list_append(biblio->books, info->path);
}

static void
_done_cb(void *data, Eio_File *handler)
{
  eBiblio *biblio = data;
  ebiblio_show(biblio);
  ecore_main_loop_quit();
}

 static void
_error_cb(void *data, Eio_File *handler, int error)
{
  fprintf(stderr, "Something wrong has happend:%s\n", strerror(error));
  ecore_main_loop_quit();
}

/*
static void _get_epub_list2(eBiblio *biblio, char *dir)
{
  Eina_Iterator *files;
  Eina_File_Direct_Info *info;
  char *path;

  files = eina_file_stat_ls(dir);

  EINA_ITERATOR_FOREACH(files, info)
  {
    path = eina_file_path_sanitize(info->path);

    if (info->type == EINA_FILE_REG)
    {
      
      biblio->books = eina_list_append(biblio->books, path);
      printf("Regular File %s\n",path);
    }
    else if (info->type == EINA_FILE_DIR)
    {
      //_get_epub_list2(biblio, path);
      _get_epub_list2(biblio, path);
      printf("Directory : %s\n",path);
    }
    else
      continue;

    //free(path);
  }
}
*/

eBiblio *ebiblio_new(char* path, eBiblio *biblio)
{

  if (biblio == NULL)
    biblio = calloc(1, sizeof(eBiblio));

  biblio->path = path;

  if (ecore_file_is_dir(biblio->path))
    //biblio->books = _get_epub_list(path);
    eio_dir_stat_ls(path, _filter_epub_cb, _add_to_library_cb, _done_cb, _error_cb, biblio);
  else
    printf("%s : No such file or directory\n", biblio->path);

  return biblio;

}

static void ebiblio_free(eBiblio *bib)
{
  free(bib);
}

int main(int argc, char **argv)
{
  eBiblio *library;

  elire_init();

  ebiblio_new(BOOKPATH, library);
  ebiblio_free(library);

  ecore_main_loop_begin();

  elire_shutdown();
  return 0;
}
