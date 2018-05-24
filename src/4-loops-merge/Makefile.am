include $(top_srcdir)/src/PATHS

noinst_LIBRARIES = libloopsmerge.a
libloopsmerge_a_SOURCES = loops_merge.cpp 

libloopsmerge_a_CPPFLAGS = -I$(model_dir) \
						   -I$(reduce_dir) \
						   -I$(parse_dir) \
						   -I$(loopsid_dir) \
						   -I$(putils_dir) \
						   -I$(tutils_dir) \
						   -I$(mlpack_dir)/include \
						   $(BOOST_CPPFLAGS) \
						   $(BOOST_LDFLAGS)
