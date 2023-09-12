# frozen_string_literal: true

require "mkmf-rice"

# -march=native not supported with ARM Mac
default_optflags = RbConfig::CONFIG["host_os"] =~ /darwin/i && RbConfig::CONFIG["host_cpu"] =~ /arm|aarch64/i ? "" : "-march=native"
# -pthread and -O3 set by default
$CXXFLAGS << " -std=c++17 $(optflags) -funroll-loops " << with_config("optflags", default_optflags)

ext = File.expand_path(".", __dir__)
saxon = File.expand_path("../../vendor/libsaxon-hec-12.3", __dir__)
saxon_lib = case RbConfig::CONFIG["host_os"]
    when /darwin/i
        File.join(saxon, "libs", "osx", "libsaxon-hec-12.3.dylib")
    when /linux/i
        File.join(saxon, "libs", "nix", "libsaxon-hec-12.3.so")
    when /mswin|msys|mingw|cygwin|bccwin|wince|emc/i
        File.join(saxon, "libs", "win", "libsaxon-hec-12.3.dll")
    else
        raise "Unsupported OS: #{RbConfig::CONFIG["host_os"]}"
    end
puts "Using Saxon library: #{saxon_lib}"

$hdrs = Dir.glob("#{saxon}/Saxon.C.API/[!php,Cython]*.h") + Dir.glob("#{saxon}/Saxon.C.API/graalvm/*.h")
$srcs = %w(SaxonProcessor.cpp DocumentBuilder.cpp XQueryProcessor.cpp  XsltExecutable.cpp Xslt30Processor.cpp XPathProcessor.cpp SchemaValidator.cpp XdmValue.cpp XdmItem.cpp XdmNode.cpp XdmAtomicValue.cpp XdmFunctionItem.cpp XdmMap.cpp XdmArray.cpp SaxonApiException.cpp SaxonCGlue.c).map{ |f| File.join(saxon, "Saxon.C.API", f) }
$srcs += Dir.glob("#{ext}/*.cpp")
$VPATH += ["#{ext}", "#{saxon}/Saxon.C.API", "#{saxon}/Saxon.C.API/graalvm"]
$INCFLAGS << " -I#{saxon}/Saxon.C.API -I#{saxon}/Saxon.C.API/graalvm"

$LIBPATH << "#{saxon_lib}"

create_makefile("saxon/saxon")
