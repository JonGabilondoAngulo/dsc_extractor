//
//  main.c
//  dsc_extractor
//
//  Created by Jon on 15/07/2021.
//

#include <stdio.h>
#include <stddef.h>
#include <dlfcn.h>
#include <TargetConditionals.h>


typedef int (*extractor_proc)(const char* shared_cache_file_path, const char* extraction_root_path,
                              void (^progress)(unsigned current, unsigned total));

int main(int argc, const char* argv[])
{
    if ( argc != 3 ) {
        fprintf(stderr, "usage: dsc_extractor <path-to-cache-file> <path-to-device-dir>\n");
        return 1;
    }

#if TARGET_OS_OSX
    void* handle = dlopen("/usr/lib/dsc_extractor.bundle", RTLD_LAZY);
#else
    void* handle = dlopen("/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/usr/lib/dsc_extractor.bundle", RTLD_LAZY);
#endif
    
    if ( handle == NULL ) {
        fprintf(stderr, "dsc_extractor.bundle could not be loaded\n");
        return 1;
    }

    extractor_proc proc = (extractor_proc)dlsym(handle, "dyld_shared_cache_extract_dylibs_progress");
    if ( proc == NULL ) {
        fprintf(stderr, "dsc_extractor.bundle did not have dyld_shared_cache_extract_dylibs_progress symbol\n");
        return 1;
    }

    fprintf(stdout, "Extracting symbols ...\n");

    int result = (*proc)(argv[1], argv[2], ^(unsigned c, unsigned total) { printf("%d/%d\n", c, total); } );
    fprintf(stderr, "dyld_shared_cache_extract_dylibs_progress() => %d\n", result);
    return 0;
}
