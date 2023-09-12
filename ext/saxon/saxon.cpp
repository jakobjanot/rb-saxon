#include <rice/rice.hpp>
#include <rice/stl.hpp>

#include "SaxonCGlue.h"
#include "SaxonProcessor.h"

using namespace Rice;

extern "C"
void Init_saxon()
{
  Module rb_mSaxon = define_module("Saxon");
  Class rb_cSaxonProcessor = define_class_under<SaxonProcessor>(rb_mSaxon, "Processor")
    .define_constructor(Constructor<SaxonProcessor>());


}