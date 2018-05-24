include $(top_srcdir)/src/PATHS

noinst_LIBRARIES = libpseudocode.a
libpseudocode_a_SOURCES = pseudocode.cpp 

libpseudocode_a_CPPFLAGS = -I$(model_dir) \
						   -I$(reduce_dir) \
						   -I$(parse_dir) \
						   -I$(loopsid_dir) \
						   -I$(loopsmg_dir) \
						   -I$(putils_dir) \
						   -I$(tutils_dir) \
						   -I$(mlpack_dir)/include \
						   $(WX_CXXFLAGS) \
						   $(BOOST_CPPFLAGS) \
						   $(BOOST_LDFLAGS)
