# frozen_string_literal: true

require "mkmf-rice"

# -march=native not supported with ARM Mac
default_optflags = RbConfig::CONFIG["host_os"] =~ /darwin/i && RbConfig::CONFIG["host_cpu"] =~ /arm|aarch64/i ? "" : "-march=native"
# -pthread and -O3 set by default
$CXXFLAGS << " -std=c++17 $(optflags) -funroll-loops " << with_config("optflags", default_optflags)

ext = File.expand_path(".", __dir__)
saxon = File.expand_path("../../vendor/libsaxon-hec-12.3/Saxon.C.API", __dir__)

$srcs = Dir["{#{ext},#{saxon}}/**/*.{c,cc,cpp}"] - Dir["#{saxon}/**/php*"] - Dir["#{saxon}/**/Cython*"]
$INCFLAGS << " -I#{saxon} -I#{saxon}/graalvm"
$VPATH << saxon << "#{saxon}/graalvm"

create_makefile("saxon/saxon")
