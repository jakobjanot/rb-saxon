#if defined __linux__ || defined __APPLE__
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HANDLE void *
#define LoadLibrary(x) dlopen(x, RTLD_LAZY)
#define GetProcAddress(x, y) dlsym(x, y)
#else
#include <windows.h>
#endif

#include "../Saxon.C.API/SaxonCGlue.h"

int validate(sxnc_environment *environi, int argc, const char *argv[]) {
  if (argc < 2) {
    printf("\nError: Not enough arguments in validate");
    return 0;
  }
  int64_t processorDataRef =
      createProcessorDataWithCapacity(environi->thread, argc);

  for (int i = 1; i < argc; i++) {
#ifdef DEBUG
    printf("\nArgument = %s", (char *)argv[i]);
#endif
    addProcessorProperty(environi->thread, (void *)processorDataRef,
                         (char *)argv[i]);
  }
#ifdef DEBUG
  printf("\nprocessorData = %i", (int)processorDataRef);
#endif
  long status = j_run_validate(environi->thread, (void *)processorDataRef);
#ifdef DEBUG
  printf("\nj_run_validate status=%i", (int)status);
  if (status == -1) {
    printf("\nProcessorDataRef is null");
  }
#endif

  return 0;
}

int main(int argc, const char *argv[]) {
  sxnc_environment *environi;
  environi = (sxnc_environment *)malloc(sizeof(sxnc_environment));
  create_graalvm_isolate(environi);
#ifdef DEBUG
  setDebugMode(environi->thread, 1);
#endif
  validate(environi, argc, argv);

  fflush(stdout);
  if (graal_detach_thread(environi->thread) != 0) {
    fprintf(stderr, "graal_detach_thread error\n");
    return -1;
  }

  return 0;
}
